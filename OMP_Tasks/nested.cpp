#include <omp.h>
#include <stdio.h>

#define NEST_1_NUM_THREADS 8
#define NEST_2_NUM_THREADS 2
#define NEST_3_NUM_THREADS 4

auto main() -> int {
  omp_set_dynamic(0);
  omp_set_nested(1);

  printf("<current level thread number>/<current level number of "
         "threads>/[<previous level number of threads>/...]\n\n");
  int sync_var = 0;

  printf("\nLevel: 1\n");
#pragma omp parallel num_threads(NEST_1_NUM_THREADS) shared(sync_var)
  {
    int level_1_thread_num = omp_get_thread_num();
    int level_1_num_threads = omp_get_num_threads();
    printf("%d/%d\n", level_1_thread_num, level_1_num_threads);
#pragma omp barrier

    if (level_1_thread_num == 0)
      printf("\nLevel: 2\n");
#pragma omp barrier

#pragma omp parallel num_threads(NEST_2_NUM_THREADS) \
    shared(level_1_thread_num, level_1_num_threads)
    {
      int level_2_thread_num =
          omp_get_num_threads() * level_1_thread_num + omp_get_thread_num();
      int level_2_num_threads = omp_get_num_threads() * level_1_num_threads;
      printf("%2d/%d/%d\n", level_2_thread_num, level_2_num_threads,
             level_1_num_threads);
#pragma omp barrier

#pragma omp atomic
      sync_var++;
#pragma omp flush(sync_var)
      while (sync_var < level_2_num_threads) {
#pragma omp flush(sync_var)
      }

      if (level_2_thread_num == 0) {
        printf("\nLevel: 3\n");
      }
#pragma omp barrier

#pragma omp parallel num_threads(NEST_3_NUM_THREADS)                           \
    shared(level_2_thread_num, level_2_num_threads)
      {
        int level_3_thread_num =
            omp_get_num_threads() * level_2_thread_num + omp_get_thread_num();
        int level_3_num_threads = omp_get_num_threads() * level_2_num_threads;
        printf("%2d/%d/%d/%d\n", level_3_thread_num, level_3_num_threads,
               level_2_num_threads, level_1_num_threads);
      }
    }
  }
  return 0;
}
