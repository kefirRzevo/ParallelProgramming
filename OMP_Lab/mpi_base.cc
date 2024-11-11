#include <cmath>
#include <iostream>
#include <array>
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

  auto a = two_dim_arr<double>{ISIZE, JSIZE};

  const auto diff_I = int{ISIZE / commsize};
  auto start_I = int{diff_I * rank};
  auto end_I = int{diff_I * (rank + 1)};

  if (ISIZE % commsize) {
    if (rank < ISIZE % commsize) {
      start_I += rank;
      end_I += rank + 1;
    } else {
      start_I += (ISIZE % commsize);
      end_I += (ISIZE % commsize);
    }
  }

  for (auto i = start_I; i < end_I; ++i) {
    for (auto j = int{}; j < JSIZE; ++j) {
      a[i][j] = 10 * i + j;
    }
  }

  auto filename = "base_mpi_" + std::to_string(commsize) + ".dat";
  auto filepath = repo_path / "build" / "OMP_Lab" / filename;

  MPI_Barrier(MPI_COMM_WORLD);
  auto start = MPI_Wtime();
  if (commsize > 1) {

    for (auto i = start_I; i < end_I; ++i) {
      for (auto j = int{}; j < JSIZE; ++j) {
        a[i][j] = std::sin(2 * a[i][j]);
      }
    }

    if (rank) {
      MPI_Request myRequest;

      auto sendStart = start_I;
      auto sendSize = int{(end_I - start_I) * JSIZE};
      MPI_Isend(&sendStart, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &myRequest);
      MPI_Isend(&sendSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &myRequest);
      MPI_Isend(&a[start_I][0], sendSize, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,
                &myRequest);
    } else {
      for (auto k = int{1}; k < commsize; ++k) {
        auto recvStart = int{};
        auto recvSize = int{};

        MPI_Recv(&recvStart, 1, MPI_INT, k, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        MPI_Recv(&recvSize, 1, MPI_INT, k, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        MPI_Recv(&a[recvStart][0], recvSize, MPI_DOUBLE, k, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
      }
    }
  } else {
    for (auto i = int{}; i < ISIZE; ++i) {
      for (auto j = int{}; j < JSIZE; ++j) {
        a[i][j] = std::sin(2 * a[i][j]);
      }
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  auto end = MPI_Wtime();

  if (!rank) {
    report(commsize, end - start);
    to_file(filepath.c_str(), a);
  }
  MPI_Finalize();
  return 0;
}
