#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "matrix.h"


void benchmark_mul(int n, int repeats) {


  double *a = random_matrix(n);
  double *b = random_matrix(n);

  double *res = alloc_matrix(n);

  clock_t start, end;
  int i;

  double basic = 0.0;

  if(n < 1000) {
    start = clock();
    for(i = 0; i < repeats; ++i) {
      matrix_mul_basic(res, a, b, n);
    }

    end = clock();
    basic = (double)(end - start) / CLOCKS_PER_SEC / (double)repeats;
  }



  start = clock();
  for(i = 0; i < repeats; ++i) {
    matrix_mul_transposed(res, a, b, n);
  }

  end = clock();
  double transposed = (double)(end - start) / CLOCKS_PER_SEC / (double)repeats;



  start = clock();
  for(i = 0; i < repeats; ++i) {
    matrix_mul_blocked(res, a, b, n, 128);
  }

  end = clock();
  double blocked = (double)(end - start) / CLOCKS_PER_SEC / (double)repeats;


  printf("%d basic: %.5lf, transposed: %.5lf, blocked: %.5lf\n", n, basic, transposed, blocked);

  free(a);
  free(b);
  free(res);
}

int main(int argc, char**argv) {


  int size_inc = 100;
  int reps = 10;
  int i;

  for(i = 1; i < 21; ++i) {
    benchmark_mul(i * size_inc, reps);
  }

  return 0;
}
