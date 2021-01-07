/* Concurrent implementations of Matrix-Vector Multiplication */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

int *matrix1;
char *result;
int number_of_threads;
int matrix_dimension;

float *create_matrix();
void populate_matrix(float *matrix);
void print_matrix(float *matrix);

void *task(void *arg) {
  int thread_id = *((int *) arg), i, j, k;
  int temp;

  /*printf("Thread %d\n", thread_id);*/

  /* faz a multiplicação da matriz apenas nas linhas que ela é responsável */
  for (i = thread_id; i < matrix_dimension; i += number_of_threads) {
    for (j = 0; j < matrix_dimension; j++) {
      temp = 0;
      for (k = 0; k < matrix_dimension; k++) {
        temp += matrix1[i * matrix_dimension + k] * matrix2[k * matrix_dimension + j];
      }
      result[i * matrix_dimension + j] = temp;
    }
  }

  pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
  pthread_t *threads_array;
  int *thread_ids, i;
  double start, end, delta;

  GET_TIME(start);

  /* testa se a quantidade de argumentos passados é suficiente */
  if (argc < 3) {
    printf("Enter: <matrix dimension> <number o threads>\n");
    return 1;
  }

  /* converte os argumentos passados na chamada do programa */
  matrix_dimension = atoi(argv[1]);
  number_of_threads = atoi(argv[2]);

  if (number_of_threads > matrix_dimension) number_of_threads = matrix_dimension;

  /* aloca as estruturas de dados que são usadas */
  matrix1 = create_matrix();
  matrix2 = create_matrix();
  result = create_matrix();

  /* fornece valores para as matrizes */
  populate_matrix(matrix1);
  populate_matrix(matrix2);

  /* aloca um array com todos os ponteiros para as threads criadas */
  threads_array = (pthread_t *) malloc(sizeof(pthread_t) * number_of_threads);
  if (threads_array == NULL) {
    printf("ERROR -- Malloc\n");
    return 2;
  }

  /* aloca um array que contem os identificadores das threads */
  thread_ids = (int *) malloc(sizeof(pthread_t) * number_of_threads);
  if (thread_ids == NULL) {
    printf("ERROR -- Malloc\n");
    return 2;
  }

  /* cria as threads que irão executar a multiplicação */
  for (i = 0; i < number_of_threads; i++) {
    thread_ids[i] = i;

    if (pthread_create(threads_array + i, NULL, task, (void *) &thread_ids[i])) {
      printf("ERROR -- pthread_create\n");
      return 2;
    }
  }

  /* espera as threads terminarem sua execução */
  for (i = 0; i < number_of_threads; i++) {
    pthread_join(*(threads_array + i), NULL);
  }

  /* imprime o resultado */
  /* print_matrix(result); */

  free(matrix1);
  free(matrix2);
  free(result);
  free(threads_array);

  GET_TIME(end);
  delta = end - start;
  printf("Total time: %lf\n", delta);

  return 0;
}

float *create_matrix() {
  float *matrix;
  matrix = (float *) malloc(sizeof(float) * matrix_dimension * matrix_dimension);
  if (matrix == NULL) {
    printf("ERROR -- Malloc\n");
    exit(2);
  }

  return matrix;
}

void populate_matrix(float *matrix) {
  int i, j;

  for (i = 0; i < matrix_dimension; i++) {
    for (j = 0; j < matrix_dimension; j++) {
      matrix[i * matrix_dimension + j] = 1;
    }
  }
}

void print_matrix(float *matrix) {
  int i, j;
  for (i = 0; i < matrix_dimension; i++) {
    for (j = 0; j < matrix_dimension; j++) {
      printf("%.1f ", result[i * matrix_dimension + j]);
    }
    puts("");
  }
  puts("");
}
