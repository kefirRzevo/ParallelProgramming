#include <iostream>
#include <mpi.h>
#include <stdexcept>
#include <string>

// mpic++ -std=c++17 task3.cpp -O3 -o a3.out
// mpirun --map-by :oversubscribe -n 2 a3.out

int main(int argc, char *argv[]) {
  try {
    auto offset = [](int rank, int size) {
      int offset = 0;
      for (auto it = size - 1; it > rank; --it) {
        offset += std::to_string(it).size() + 1;
      }
      return offset;
    };

    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::string out = std::to_string(rank) + "\n";

    MPI_File file;
    MPI_File_open(MPI_COMM_WORLD, "res/output3.txt",
                  MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &file);
    // Just for truncation
    MPI_File_set_size(file, 0);

    MPI_File_write_at(file, offset(rank, size), out.c_str(),
                      static_cast<int>(out.size()), MPI_CHAR,
                      MPI_STATUS_IGNORE);
    MPI_File_close(&file);
    MPI_Finalize();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}
