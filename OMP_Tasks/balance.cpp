#include <cassert>
#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <stdio.h>

enum Mode {
  STATIC = 1,
  DYNAMIC = 2,
  GUIDED = 3,
};

long long int fact(long long int n) { return n <= 1 ? 1 : n * fact(n - 1); }

auto main(int argc, const char *argv[]) -> int {
  const int itersCount = 65;
  const int threadsCount = 4;
  const int scheduleChunkSize = 4;
  if (argc != 2) {
    std::cout << "usage: mode (1 - static, 2 - dynamic, 3 - guided)"
              << std::endl;
    return -1;
  }
  const int modeType = std::atoi(argv[1]);
  omp_set_dynamic(0);
  omp_set_num_threads(threadsCount);

  printf("Thread Iteration\n");
  switch (modeType) {
  case Mode::STATIC: {
#pragma omp parallel for schedule(static, scheduleChunkSize)
    for (int i = 0; i < itersCount; i++) {
      printf("%d %d %lld\n", omp_get_thread_num(), i, fact(i));
    }
    break;
  }
  case Mode::DYNAMIC: {
#pragma omp parallel for schedule(dynamic, scheduleChunkSize)
    for (int i = 0; i < itersCount; i++) {
      printf("%d %d %lld\n", omp_get_thread_num(), i, fact(i));
    }
    break;
  }
  case Mode::GUIDED: {

#pragma omp parallel for schedule(guided, scheduleChunkSize)
    for (int i = 0; i < itersCount; i++) {
      printf("%d %d %lld\n", omp_get_thread_num(), i, fact(i));
    }
    break;
  }
  default:
    assert(0 && "Unreachable");
  }

  return 0;
}
