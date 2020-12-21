#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

int array_dimension;
double *array;
int number_of_threads;

void *task(void *arg);

int main(int argc, char const *argv[]) {
  pthread_t *threads_id;
  long int i;
  double *thread_result, concurrent_sum = 0;
  double start, end, delta;

  GET_TIME(start);

  if (argc < 3) {
    fprintf(stderr, "enter: %s <array dimension> <numeber of threads>\n", argv[0]);
    return  1;
  }

  array_dimension = atoi(argv[1]);
  number_of_threads = atoi(argv[2]);

  array = (double *) malloc(sizeof(double) * array_dimension);
  if (array == NULL) {
    fprintf(stderr, "Error -- malloc\n");
    return 2;
  }

  for (i = 0; i < array_dimension; i++) {
    array[i] = 1000.0 / (i + 1);
  }

  threads_id = (pthread_t *) malloc(sizeof(pthread_t) * number_of_threads);
  if (threads_id == NULL) {
    fprintf(stderr, "Error -- malloc\n");
    return 2;
  }

  for (i = 0; i < number_of_threads; i++) {
    if (pthread_create(threads_id+i, NULL, task, (void *) i)) {
      fprintf(stderr, "Error -- pthread_create\n");
      return 3;
    }
  }

  for (i = 0; i < number_of_threads; i++) {
    if (pthread_join(*(threads_id+i), (void **) &thread_result)) {
      fprintf(stderr, "Error -- pthread_create\n");
      return 3;
    }
    concurrent_sum += *thread_result;
    free(thread_result);
  }

  printf("Resultado: %lf\n", concurrent_sum);

  free(array);
  free(threads_id);

  GET_TIME(end);
  delta = end - start;
  printf("Total time: %lf\n", delta);

  return 0;
}

void *task(void *arg) {
  long int thread_id = (long int) arg;
  long int block_size = array_dimension/number_of_threads;
  long int start = thread_id * block_size, end;
  long int i;
  double *local_sum;

  local_sum = (double *) malloc(sizeof(double));
  if (local_sum == NULL) {
    fprintf(stderr, "Error -- malloc\n");
    exit(2);
  }

  if (thread_id == number_of_threads - 1) end = array_dimension;
  else end = start + block_size;

  for (i = start; i < end; i++) {
    *local_sum += array[i];
  }

  pthread_exit((void *) local_sum);
}
