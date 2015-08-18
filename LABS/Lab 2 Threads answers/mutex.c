//
// mutex.c - sum a vector of values using multi-threading and a mutex
// compile with: gcc mutex.c -o mutex --std=c99 -lpthread
//
//


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 16
#define N 10000


typedef struct  {
 float *total;

 float *values;
 int n;
 
 pthread_mutex_t *lock;

 
} Worker;

 
void *run_summation( void *ptr )
{
  Worker *worker = (Worker*)ptr;
  float total = 0;
  
  for(int i = 0; i < worker->n; ++i) {
      pthread_mutex_lock(worker->lock);
      (*worker->total) +=  worker->values[i];
      pthread_mutex_unlock(worker->lock);
  }
  
}



int main()
{
  int i = 0;
  pthread_t thread[NUM_THREADS];
 
  float values[N];
  
  // Initialize the values to something
  for(int i = 0; i < N; ++i) {
      values[i] = i;
  }

  // Global variable for total summation so far
  float total = 0;
 
  // How many values per thread
  int per_batch = N/NUM_THREADS;
  int n = 0; // How many values assigned so far 
  
  pthread_mutex_t lock;
  
  Worker worker[NUM_THREADS];
  for(int i = 0; i < NUM_THREADS; ++i) {
    // What would be the problem declaring Worker w here?
    
    Worker *w = &worker[i];
    w->total = &total; // Pass the global total into each thread
    w->values = &values[n]; // Assign each worker a section of values to sum
    
    w->lock = &lock;
    
    // Assign some threads to this worker
    w->n = i == NUM_THREADS - 1 ? (N - n) : per_batch;
    n += per_batch;

  }
    
  // Create a bunch of threads to perform some withdrawals
  for (i = 0; i < NUM_THREADS; ++i) {
        
    //TODO: Make this run in a thread!
    //run_summation((void*)w);
    
    pthread_create( &thread[i], NULL, run_summation, &worker[i]);
  }

  ////////////////////////////////
  // Wait for all the threads we created
  for (i = 0; i < NUM_THREADS; ++i) {
    // Wait for ith thread to finish
    pthread_join( thread[i], NULL);
  }
  ////////////////////////////////
  
  printf("Final total: %f \n", total);

	return 0;
}

