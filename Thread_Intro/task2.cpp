#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

std::mutex mtx;

void thread_sum(int first, int last, double &sum) {
  double local_sum = 0.;
  for (auto i = first; i < last; ++i) {
    local_sum += 1. / i;
  }
  mtx.lock();
  sum += local_sum;
  mtx.unlock();
}

auto main(int argc, const char *argv[]) -> int {
  try {
    if (argc != 3) {
      throw std::runtime_error("Incorrect number of arguments");
    }
    auto n_threads = std::stoi(argv[1]);
    auto n_max = std::stoi(argv[2]);
    auto distribute = [&](int i) -> std::pair<int, int> {
      auto n_diff = n_max / n_threads;
      auto n_first = n_diff * i;
      auto n_last = n_diff * (i + 1);
      if (n_max % n_threads != 0) {
        if (i < n_max % n_threads) {
          n_first += i;
          n_last += i + 1;
        } else {
          n_first += (n_max % n_threads);
          n_last += (n_max % n_threads);
        }
      }
      n_first = i == 0 ? n_first + 1 : n_first;
      return {n_first, n_last};
    };

    std::vector<std::thread> threads;
    threads.reserve(n_threads);
    double sum = 0.;
    for (auto i = 0; i < n_threads; ++i) {
      auto [first, last] = distribute(i);
      threads.emplace_back(thread_sum, first, last, std::ref(sum));
    }
    for (auto &&thr : threads) {
      thr.join();
    }
    std::cout << "Sum is " << sum << std::endl;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
