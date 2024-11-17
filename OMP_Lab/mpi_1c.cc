#include <array>
#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>
#include <filesystem>

#include <mpi/mpi.h>

#include "common.h"

namespace fs = std::filesystem;

auto repo_path = fs::path{__FILE__}.parent_path().parent_path();

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  auto commsize = int{};
  auto rank = int{};
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  auto a = two_dim_arr_mpi<double>{ISIZE, JSIZE};

  const auto diff_J = int{JSIZE / commsize};
  auto start_J = int{diff_J * rank};
  auto end_J = int{diff_J * (rank + 1)};

  if (JSIZE % commsize) {
    if (rank < JSIZE % commsize) {
      start_J += rank;
      end_J += rank + 1;
    } else {
      start_J += (JSIZE % commsize);
      end_J += (JSIZE % commsize);
    }
  }

  if (rank == commsize - 1) {
    end_J = JSIZE - 2;
  }
  for (auto i = int{}; i < ISIZE; ++i) {
    for (auto j = int{}; j < JSIZE; ++j) {
      a[i][j] = 10 * i + j;
    }
  }
  auto filename = "1c_mpi_" + std::to_string(commsize) + ".dat";
  auto filepath = repo_path / "build" / "OMP_Lab" / filename;
  auto localArraySize = int{end_J - start_J};
  auto localArray = std::array<double, JSIZE>{};
  auto recvcnts = new int[commsize];
  auto displs = new int[commsize];

  for (auto k = int{}; k < commsize; ++k) {
    MPI_Gather(&localArraySize, 1, MPI_INT, &recvcnts[0], 1, MPI_INT, k,
               MPI_COMM_WORLD);
  }

  displs[0] = 0;
  for (auto i = int{1}; i < commsize; ++i) {
    displs[i] = displs[i - 1] + recvcnts[i - 1];
  }

  MPI_Barrier(MPI_COMM_WORLD);
  auto start = MPI_Wtime();
  if (commsize > 1) {
    for (auto i = int{2}; i < ISIZE; ++i) {
      for (auto j = start_J; j < end_J; ++j) {
        localArray[j] = std::sin(5 * a[i - 2][j + 3]);
      }

      MPI_Allgatherv(&localArray[start_J], localArraySize, MPI_DOUBLE, &a[i][0],
                     &recvcnts[0], &displs[0], MPI_DOUBLE, MPI_COMM_WORLD);
    }
  } else {
    for (auto i = int{2}; i < ISIZE; ++i) {
      for (auto j = int{}; j < JSIZE - 3; ++j) {
        a[i][j] = std::sin(5 * a[i - 2][j + 3]);
      }
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  auto end = MPI_Wtime();

  delete[] recvcnts;
  delete[] displs;

  if (!rank) {
    report(commsize, end - start);
    to_file(filepath.c_str(), a);
  }
  MPI_Finalize();
  return 0;
}
