#include <algorithm>
#include <iostream>
#include <mpi.h>
#include <stdexcept>

// mpic++ -std=c++17 task2.cpp -O3 -o a2.out
// mpirun --map-by :oversubscribe -n 2 a2.out

auto main(int argc, char *argv[]) -> int {
  try {
    if (argc < 2) {
      MPI_Finalize();
      throw std::runtime_error("Incorrect num of argument");
    }

    auto accumulate = [](int start, int end) -> double {
      auto sum = 0.0;
      for (auto it = start; it != end; ++it) {
        sum += 1 / static_cast<decltype(sum)>(it);
      }
      return sum;
    };

    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    auto N = std::stoi(argv[1]);
    auto not_first = (size == 1 ? 0 : N + size - N % size) / (size + 1);
    auto first = N - not_first * (size - 1);
    auto end = first + rank * not_first;
    auto start = (rank == 0 ? 1 : end - not_first + 1);

    auto partSum = accumulate(start, end);
    std::cout << "Process " << rank << ": summing interval = [" << start << ", "
              << end << ")  =>  Sum = " << partSum << std::endl;

    MPI_Win win;
    MPI_Win_create(&partSum, (MPI_Aint)sizeof(double), sizeof(double),
                   MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    MPI_Win_fence(0, win);
    if (rank) {
      MPI_Accumulate(&partSum, 1, MPI_DOUBLE, 0, (MPI_Aint)0, 1, MPI_DOUBLE,
                     MPI_SUM, win);
    }
    MPI_Win_fence(0, win);
    if (rank == 0) {
      std::cout << "Total sum S = " << partSum << std::endl;
    }
    MPI_Win_free(&win);
    MPI_Finalize();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}
