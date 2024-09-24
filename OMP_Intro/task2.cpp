#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>
#include <string>

#define NUM_THREADS 4

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
      throw std::runtime_error("Incorrect number of arguments");

    auto N = std::atoll(argv[1]);
    double sum = 0.;

    omp_set_dynamic(0);
    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel for reduction(+:sum)
    for(long long i = 1; i < N + 1; i++) {
      sum += 1.0 / (double)i;
    }
    printf("\nTotal sum S = %.16lg\n", sum);
    return 0;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
