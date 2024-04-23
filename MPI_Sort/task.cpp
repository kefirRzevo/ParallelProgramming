#include <mpi.h>
#include <random>
#include <string>
#include <limits>
#include <fstream>
#include <numeric>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "sort.hpp"

//mpic++ -std=c++17 task.cpp -O3 -o a.out
//mpirun --map-by :oversubscribe -n 2 par 100

using buffer = std::vector<int>;

int main(int argc, char *argv[]) {
  try {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    /********** Create and populate the array **********/
    auto n = std::stoi(argv[1]);
    buffer original_array;
    original_array.resize(n);
    std::iota(original_array.begin(), original_array.end(), 1);
    std::shuffle(original_array.begin(), original_array.end(), std::mt19937{});
    /********** Initialize MPI **********/
    int world_rank;
    int world_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    //std::cout << original_array << std::endl;

    /********** Divide the array in equal-sized chunks **********/
    auto remain = n % world_size;
    int size;
    if (remain) {
      size = n / world_size + 1;
      for (auto i = 0; i < world_size - remain; ++i) {
        original_array.push_back(std::numeric_limits<int>::max());
      }
    } else {
      size = n / world_size;
    }
    /********** Send each subarray to each process **********/
    buffer sub_array;
    sub_array.resize(size);

    MPI_Scatter(original_array.data(), size, MPI_INT, sub_array.data(), size,
                MPI_INT, 0, MPI_COMM_WORLD);

    /********** Perform the mergesort on each process **********/
    buffer tmp_array;
    tmp_array.resize(size);

    detail::merge_sort(sub_array, tmp_array, 0, (size - 1));

    /********** Gather the sorted subarrays into one **********/
    if (world_rank != 0) {
      MPI_Gather(sub_array.data(), size, MPI_INT, nullptr, size, MPI_INT, 0,
                 MPI_COMM_WORLD);
    } else {
      buffer sorted;
      sorted.resize(original_array.size());
      MPI_Gather(sub_array.data(), size, MPI_INT, sorted.data(), size, MPI_INT,
                 0, MPI_COMM_WORLD);

      /********** Make the final mergeSort call **********/
      buffer other_array;
      other_array.resize(original_array.size());
      detail::merge_sort(sorted, other_array, 0, (n - 1));
      for (auto i = 0; i < world_size - remain; ++i) {
        sorted.pop_back();
      }
      //std::cout << sorted << std::endl;
    }
    /********** Finalize MPI **********/
    MPI_Barrier(MPI_COMM_WORLD);
    if (world_rank == 0) {
      auto end = high_resolution_clock::now();
      auto elapsed = duration_cast<microseconds>(end - start).count();
      std::cout << elapsed << std::endl;
    }
    MPI_Finalize();
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
