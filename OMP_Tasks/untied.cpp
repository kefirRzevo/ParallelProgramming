#include <omp.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <time.h>

#define NUM_THREADS 100000

#define UNTIED_DANGER
#define UNTIED_SAFE

int g_array[NUM_THREADS];
int S = 0;

void task_safe() {
  // Safe
  int localSum = 0;

  // Safe
  int start_1 = omp_get_thread_num() * 100;
  int end_1 = (omp_get_thread_num() + 1) * 100;

  // Safe
  for (int i = start_1; i < end_1; i++) {
    localSum++;
  }

// Force scheduling point, safe
#pragma omp taskyield

  // Even if thread num is new, it doesn't matter
  int start_2 = omp_get_thread_num() * 10;
  int end_2 = (omp_get_thread_num() + 1) * 10;

  // Safe
  for (int i = start_2; i < end_2; i++) {
    localSum++;
  }

// Safe, S should be (100 + 10) * NUM_THREADS
#pragma omp atomic
  S += localSum;
}

void task_danger() {
  // Safe
  g_array[omp_get_thread_num()] = omp_get_thread_num();

  // Safe
  g_array[omp_get_thread_num()] += omp_get_thread_num();

// Force scheduling point, safe
#pragma omp taskyield

  // Line itself is safe but can possibly make g_array[omp_get_thread_num()] < 0
  // or >= NUM_THREADS
  g_array[omp_get_thread_num()] -= omp_get_thread_num();

  // Line itself is safe but at this stage idx can possibly be < 0 or >=
  // NUM_THREADS
  int idx = g_array[omp_get_thread_num()];

  // DANGER!!! idx can be < 0 or >= NUM_THREADS by this moment
  printf("g_array[%d] = %d\n", idx, g_array[idx]);
}

int main() {
  srand(time(NULL));
#ifdef UNTIED_SAFE

  printf("SAFE ZONE:\n");

#pragma omp parallel num_threads(NUM_THREADS)
#pragma omp single
  {
    for (int i = 0; i < NUM_THREADS; i++) {

#pragma omp task untied
      task_safe();
    }
  }

  printf("S = %d\n", S);

#endif

#ifdef UNTIED_DANGER

  printf("\nDANGER ZONE:\n");

#pragma omp parallel num_threads(NUM_THREADS)
#pragma omp single
  {
    for (int i = 0; i < NUM_THREADS; i++) {

#pragma omp task untied
      task_danger();
    }
  }

#endif

  return 0;
}
