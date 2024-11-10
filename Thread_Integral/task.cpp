#include <cassert>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

struct task_t {
  double start;
  double end;
  double fn_start;
  double fn_end;
  double area;

  task_t() = default;

  task_t(double start_, double end_, double fn_start_, double fn_end_,
         double area_)
      : start(start_), end(end_), fn_start(fn_start_), fn_end(fn_end_),
        area(area_) {}
};

constexpr double first = 0.005;
constexpr double last = 4.;
constexpr auto func = [](double x) { return std::cos(1. / x); };

std::mutex g_stack_mtx;
std::mutex g_has_task_mtx;
std::mutex g_res_mtx;

std::vector<task_t> g_stack;
double g_result = 0;
size_t active_threads = 0;

size_t n_threads = 2;
double epsilon = 1e-7;

void thread_integrate() {
  task_t task;
  while (1) {
    // Get one interval from global stack
    {
      // Wait for a new task
      g_has_task_mtx.lock();

      std::lock_guard<std::mutex> scoped_lock{g_stack_mtx};

      // If we have task, there must be task in global stack
      assert(!g_stack.empty());
      task = g_stack.back();
      g_stack.pop_back();
      if (!g_stack.empty()) {
        g_has_task_mtx.unlock();
      }

      // It is terminating task
      if (task.start > task.end) {
        break;
      }
      active_threads++;
    }

    // Integrate one interval using local stack
    {
      double l_result = 0;
      std::vector<task_t> l_stack;

      while (1) {
        double mid = (task.start + task.end) / 2;
        double fn_mid = func(mid);

        double area_start_mid =
            (fn_mid + task.fn_start) * (mid - task.start) / 2;
        double area_mid_end = (task.fn_end + fn_mid) * (task.end - mid) / 2;
        double new_area = area_start_mid + area_mid_end;

        if (std::abs(new_area - task.area) >= epsilon * std::abs(new_area)) {
          l_stack.emplace_back(task.start, mid, task.fn_start, fn_mid,
                               area_start_mid);
          task.start = mid;
          task.fn_start = fn_mid;
          task.area = area_mid_end;
        } else {
          l_result += new_area;

          // No tasks left in local stack. Stop...
          if (l_stack.empty()) {
            break;
          }
          task = l_stack.back();
          l_stack.pop_back();
        }

        // Move tasks to the global stack
        if (l_stack.size() >= 10) {
          std::lock_guard<std::mutex> scoped_lock{g_stack_mtx};

          if (g_stack.empty()) {
            auto from = l_stack.begin();
            auto to = l_stack.end();
            g_stack.insert(g_stack.end(), std::make_move_iterator(from),
                           std::make_move_iterator(to));
            l_stack.clear();
            g_has_task_mtx.unlock();
          }
        }
      }

      // Add partial sum
      {
        std::lock_guard<std::mutex> scoped_lock{g_res_mtx};
        g_result += l_result;
      }
    }

    {
      std::lock_guard<std::mutex> scoped_lock{g_stack_mtx};
      active_threads--;

      if (active_threads == 0 && g_stack.empty()) {
        // Fill stack with terminating intervals
        for (size_t i = 0; i != n_threads; ++i) {
          g_stack.emplace_back(1, 0, 0, 0, 0);
        }
        g_has_task_mtx.unlock();
      }
    }
  }
}

auto main(int argc, const char *argv[]) -> int {
  try {
    switch (argc) {
    case 3:
      epsilon = std::atof(argv[2]);
    case 2:
      n_threads = std::atoll(argv[1]);
    case 1:
      break;
    default:
      throw std::runtime_error("Incorrect number of arguments");
    };

    double start = first;
    double end = last;
    double fn_start = func(start);
    double fn_end = func(end);
    double area = (fn_end + fn_start) * (end - start) / 2;
    g_stack.emplace_back(start, end, fn_start, fn_end, area);

    using namespace std::chrono;
    std::vector<std::thread> threads;
    threads.reserve(n_threads);
    auto time_start = high_resolution_clock::now();
    for (auto i = 0; i < n_threads; ++i) {
      threads.emplace_back(thread_integrate);
    }
    for (auto &&thr : threads) {
      thr.join();
    }
    auto time_end = high_resolution_clock::now();
    auto time_elapsed =
        duration_cast<milliseconds>(time_end - time_start).count();
    // std::cout << time_elapsed << std::endl;
    std::cout << std::setprecision(20) << "Integral is " << g_result
              << std::endl;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
