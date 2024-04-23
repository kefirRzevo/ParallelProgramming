#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

std::mutex mtx;

void thread_hello(int id) {
  mtx.lock();
  std::cout << "Hello world! I am " << id << "\n";
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
    for (auto i = 0; i < n_threads; ++i) {
      threads.emplace_back(thread_hello, i);
    }
    for (auto &&thr : threads) {
      thr.join();
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
