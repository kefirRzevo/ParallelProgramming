#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

std::mutex mtx;

void thread_handshake(int id, int &sum) {
  std::string hi_msg = "I am " + std::to_string(id) + "!";
  mtx.lock();
  sum++;
  std::cout << hi_msg << " The sum is " << sum << ".\n";
  mtx.unlock();
}

auto main(int argc, const char *argv[]) -> int {
  try {
    if (argc != 2) {
      throw std::runtime_error("Incorrect number of arguments");
    }
    auto n_threads = std::stoi(argv[1]);

    std::vector<std::thread> threads;
    threads.reserve(n_threads);
    int sum = 0;
    for (auto i = 0; i < n_threads; ++i) {
      threads.emplace_back(thread_handshake, i, std::ref(sum));
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
