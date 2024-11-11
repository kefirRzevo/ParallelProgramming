#include <cmath>
#include <filesystem>
#include <iostream>
#include <omp.h>

#include "common.h"

namespace fs = std::filesystem;

auto repo_path = fs::path{__FILE__}.parent_path().parent_path();

int main(int argc, char **argv) {

  auto a = two_dim_arr<double>{ISIZE, JSIZE};

  for (auto i = int{}; i < ISIZE; ++i) {
    for (auto j = int{}; j < JSIZE; ++j) {
      a[i][j] = 10 * i + j;
    }
  }

  auto num_threads = int{1};
  if (argc >= 2) {
    num_threads = std::atoi(argv[1]);
    if (num_threads <= 0) {
      std::cerr << "Number of threads must be > 0!" << std::endl;
      return -1;
    }
  }

  omp_set_dynamic(0);
  omp_set_num_threads(num_threads);

  auto filename = "3b_omp_" + std::to_string(num_threads) + ".dat";
  auto filepath = repo_path / "build" / "OMP_Lab" / filename;

  auto start = omp_get_wtime();

  for (auto i = int{}; i < ISIZE - 1; ++i) {

#pragma omp parallel for schedule(static, 8)
    for (auto j = int{6}; j < JSIZE; ++j) {
      a[i][j] = std::sin(0.2 * a[i + 1][j - 6]);
    }
  }

  auto end = omp_get_wtime();
  report(num_threads, end - start);
  to_file(filepath.c_str(), a);
  return 0;
}