#include <stdio.h>
#include <omp.h>

#define NUM_THREADS 4


int main()
{
  omp_set_dynamic(0);
  omp_set_num_threads(NUM_THREADS);

  int data = 0;
  int flag = 0;

  #pragma omp parallel
  {
    int tid = omp_get_thread_num();
    while (flag < tid) {
      #pragma omp flush(flag)
    }
    #pragma omp flush(data)
    printf("Thread %d before increment: var = %d\n", tid, data);
    data++;
    printf("Thread %d after increment:  var = %d\n\n", tid, data);
    flag++;
    #pragma omp flush(data)
    #pragma omp flush(flag)
  }
  return 0;
}
