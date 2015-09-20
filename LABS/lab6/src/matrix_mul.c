
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include "matrix.h"
#include "queue.h"

// Naive matrix multiplication
// Multiply two square matrices of size n
// return output in res pointer
void matrix_mul_basic(double *res, double *a, double *b, int n) {

  int i, j, k;

  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {

      double r = 0.0;

      // Compute res(i, j) as dot product of ith row of a and jth column of b
      // Sum entries
      for (k = 0; k < n; ++k) {
        r += a[i * n + k] * b[k * n + j];
      }

      res[i * n + j] = r;
    }
  }

}




// Improve upon the above matrix multiplication by first transposing
// the second matrix.
//
// Run the test provided ./test_mul  to test correctness
// then run the benchmark with ./benchmark_mul to compare the differences
//
// What impact does the transposition have on performance? Why?
//

void matrix_mul_transposed(double *res, double *a, double *b, int n) {
  double *bt = alloc_matrix(n);
  matrix_transpose(bt, b, n);

  // TODO: implement matrix multiplication between a and transposed matrix bt
  int i, j, k;

  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {

      double r = 0.0;

      // Compute res(i, j) as dot product of ith row of a and jth column of b
      // Sum entries
      for (k = 0; k < n; ++k) {
        r += a[i * n + k] * bt[j * n + k];
      }

      res[i * n + j] = r;
    }
  }

  free(bt);

}

int min(int a, int b) {
 return a > b ? b : a;
}


typedef struct
{

    int i, j, k;
    int block;
    int n;
    double *result, *a, *b;

} block_matrix;

pthread_mutex_t s_lock = PTHREAD_MUTEX_INITIALIZER;

void *matrix_worker(void *args)
{
    Queue *queue = (Queue *) args;
    block_matrix *task;

    int i_end, j_end, k_end;
    int ii, jj, kk;
    double result;

    task = (block_matrix *) queue_get(queue);
    while (task)
    {
        i_end = min(task->i + task->block, task->n);
        j_end = min(task->j + task->block, task->n);
        k_end = min(task->k + task->block, task->n);



        for (ii = task->i; ii < i_end; ii++)
        {
          for (jj = task->j; jj < j_end; jj++)
          {
            result = 0.0;
            for (kk = task->k; kk < k_end; kk++)
            {
              result += task->a[ii * task->n + kk] * task->b[kk * task->n + jj];
            }
            pthread_mutex_lock(&s_lock);
            task->result[ii * task->n + jj] += result;
            pthread_mutex_unlock(&s_lock);
          }
        }

        free(task);

        task = (block_matrix *) queue_get(queue);
    }


    return NULL;
}
//
// Improve upon the naive matrix multiplication using loop blocking
// the parameter for the inner loop size is passed as the last parameter
//
// As above test your implementation before benchmarking performance
// Can you beat matrix_mul_transposed?
//
// What is the best block size? Why? Experiment with ./benchmark_block
//
// Note: helping gcc with array indexing by pre-computing common values helps a great deal to improve performance here
//
//

#define NUM_WORKERS 8

void matrix_mul_blocked(double *res, double *a, double *b, int n, int block) {


  int i, j, k;
  pthread_t threads[NUM_WORKERS];

  zero_matrix(res, n);

  // create queue
  Queue *queue = queue_alloc(500000);

  // create workers
  for (i = 0; i < NUM_WORKERS; i++)
  {
      pthread_create(&threads[i], NULL, matrix_worker, queue);
  }




  // loop over outer blocks
  for (i = 0; i < n; i += block) {
    for (j = 0; j < n; j += block) {
      for (k = 0; k < n; k += block) {
        // distribute those tasks to child processes
        block_matrix *task = (block_matrix *) malloc(sizeof(block_matrix));
        task->i = i;
        task->j = j;
        task->k = k;
        task->block = block;
        task->n = n;
        task->result = res;
        task->a = a;
        task->b = b;

        queue_put(queue, task);
      }
    }
  }

  for (i = 0; i < NUM_WORKERS; ++i) {
    queue_put(queue, NULL);
  }


  for (i = 0; i < NUM_WORKERS; i++)
  {
      pthread_join(threads[i], NULL);
  }


}
