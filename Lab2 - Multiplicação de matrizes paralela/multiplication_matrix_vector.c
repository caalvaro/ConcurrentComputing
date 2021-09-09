/* Concurrent implementations of Matrix-Vector Multiplication */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

float *matrix;
float *vector;
float *result;
int number_of_threads;
int matrix_dimension;

void *task(void *arg) {
  int thread_id = *((int *) arg), i, j;

  printf("Thread %d\n", thread_id);

  for (i = thread_id; i < matrix_dimension; i += number_of_threads) {
    for (j = 0; j < matrix_dimension; j++) {
      result[i] += matrix[i * matrix_dimension + j] * vector[j];
    }
  }

  pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
  pthread_t *threads_array;
  int *thread_ids;
  int i, j;

  if (argc < 3) {
    printf("Enter: <matrix dimension> <number o threads>\n");
    return 1;
  }

  matrix_dimension = atoi(argv[1]);
  number_of_threads = atoi(argv[2]);

  if (number_of_threads > matrix_dimension) number_of_threads = matrix_dimension;

  matrix = (float *) malloc(sizeof(float) * matrix_dimension * matrix_dimension);
  if (matrix == NULL) {
    printf("ERROR -- Malloc\n");
    return 2;
  }

  vector = (float *) malloc(sizeof(float) * matrix_dimension);
  if (vector == NULL) {
    printf("ERROR -- Malloc\n");
    return 2;
  }

  result = (float *) malloc(sizeof(float) * matrix_dimension);
  if (result == NULL) {
    printf("ERROR -- Malloc\n");
    return 2;
  }

  for (i = 0; i < matrix_dimension; i++) {
    for (j = 0; j < matrix_dimension; j++) {
      matrix[i * matrix_dimension + j] = 1;
    }
    vector[i] = 1;
    result[i] = 0;
  }

  threads_array = (pthread_t *) malloc(sizeof(pthread_t) * number_of_threads);
  if (threads_array == NULL) {
    printf("ERROR -- Malloc\n");
    return 2;
  }

  thread_ids = (int *) malloc(sizeof(pthread_t) * number_of_threads);
  if (thread_ids == NULL) {
    printf("ERROR -- Malloc\n");
    return 2;
  }

  for (i = 0; i < number_of_threads; i++) {
    thread_ids[i] = i;

    if (pthread_create(threads_array + i, NULL, task, (void *) &thread_ids[i])) {
      printf("ERROR -- pthread_create\n");
      return 2;
    }
  }

  for (i = 0; i < number_of_threads; i++) {
    pthread_join(*(threads_array + i), NULL);
  }

  for (j = 0; j < matrix_dimension; j++) {
    printf("%.1f ", result[j]);
  }
  puts("");

  free(matrix);
  free(vector);
  free(result);
  free(threads_array);

  return 0;
}
