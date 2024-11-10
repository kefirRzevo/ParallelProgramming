#include <fstream>

#include "Sudoku.h"

int main(int argc, const char *argv[]) {
  try {
    if (argc < 3) {
      auto msg = "Usage: " + std::string(argv[0]) + " <threads_count> <file>\n";
      throw std::runtime_error(msg);
    }
    auto threads_count = std::atoi(argv[1]);
    std::ifstream is{argv[2]};
    sudoku::sudoku_solver solver;
    solver.read(is);
    solver.write_original(std::cout);
    omp_set_num_threads(threads_count);
    auto start = omp_get_wtime();
    if (solver.solve()) {
      auto end = omp_get_wtime();
      solver.write_solved(std::cout);
      std::cout << "Passed: " << end - start << "\n";
    } else {
      std::cout << "No solutions!\n";
      return -1;
    }
  } catch (const std::exception &Ex) {
    std::cerr << Ex.what() << "\n";
    return 1;
  }
  return 0;
}
