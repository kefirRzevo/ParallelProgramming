#include <stdio.h>
#include <omp.h>

//#define SCHEDULE_STATIC
//#define SCHEDULE_DYNAMIC
//#define SCHEDULE_GUIDED
//#define SCHEDULE_DEFAULT

#ifdef SCHEDULE_STATIC
    #define PARALLEL_STATEMENT schedule(static, scheduleChunkSize)
#elif defined(SCHEDULE_DYNAMIC)
    #define PARALLEL_STATEMENT schedule(dynamic, scheduleChunkSize)
#elif defined(SCHEDULE_GUIDED)
    #define PARALLEL_STATEMENT schedule(guided, scheduleChunkSize)
#elif defined(SCHEDULE_DEFAULT)
    #define PARALLEL_STATEMENT
#endif

int main()
{
  const int itersCount = 65;
  const int threadsCount = 4;
  const int scheduleChunkSize = 4;

  omp_set_dynamic(0);
  omp_set_num_threads(threadsCount);

  printf("Thread Iteration\n");

  #pragma omp parallel for PARALLEL_STATEMENT
  for(int i = 0; i < itersCount; i++) {
      printf("%d %d\n", omp_get_thread_num(), i);
  }

  return 0;
}
