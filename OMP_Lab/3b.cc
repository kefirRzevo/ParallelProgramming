#include "common.h"

double a[ISIZE][JSIZE], b[ISIZE][JSIZE];
int main(int argc, char **argv) {
  int i, j;
  FILE *ff;
  for (i = 0; i < ISIZE; i++) {
    for (j = 0; j < JSIZE; j++) {
      a[i][j] = 10 * i + j;
      b[i][j] = 0;
    }
  }
  /// начало измерения времени
  for (i = 0; i < ISIZE; i++) {
    for (j = 0; j < JSIZE; j++) {
      a[i][j] = sin(0.01 * a[i][j]);
    }
  }
  for (i = 0; i < ISIZE - 1; i++) {
    for (j = 3; j < JSIZE; j++) {
      b[i][j] = a[i + 1][j - 3] * 2;
    }
  }
  /// окончание измерения времени
  ff = fopen("result.txt", "w");
  for (i = 0; i < ISIZE; i++) {
    for (j = 0; j < JSIZE; j++) {
      fprintf(ff, "%f ", b[i][j]);
    }
    fprintf(ff, "\n");
  }
  fclose(ff);
}
