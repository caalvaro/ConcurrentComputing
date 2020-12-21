#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

/* variáveis globais que serão acessadas pelas threads */
long int number_of_iterations;
int number_of_threads;

/* função que será executada pelas threads */
void *task(void *arg);
double task_sequential();
void *task_stl(void *arg);
double task_sequential_stl();
pthread_t *thread_alloc();

int main(int argc, char const *argv[]) {
  pthread_t *threads_id; /* array com os ponteiros para as threads criadas */
  long int i; /* índice usado no for */
  double *thread_result, concurrent_sum = 0.0, sequential_sum = 0.0; /* resultado parcial e total da soma */
  double start, end, delta; /* variáveis de controle de tempo */

  /* avalia se a quantidade de argumentos fornecidos é suficiente */
  if (argc < 3) {
    fprintf(stderr, "enter: %s <number of iterations> <numeber of threads>\n", argv[0]);
    return  1;
  }

  /* converte os argumentos fornecidos para o programa */
  number_of_iterations = atoll(argv[1]);
  number_of_threads = atoi(argv[2]);

  /* calcula Pi de forma sequencial */
  GET_TIME(start);

  sequential_sum = task_sequential();
  printf("\nResultado: %.15f (sequencial)\nErro: %.15f\n", sequential_sum, sequential_sum - M_PI);

  GET_TIME(end);
  delta = end - start;
  printf("Tempo total: %lf (sequencial)\n", delta);

  /* aloca o array de threads */
  threads_id = thread_alloc();

  /* cria todas as threads que serão executadas */
  for (i = 0; i < number_of_threads; i++) {
    if (pthread_create(threads_id+i, NULL, task, (void *) i)) {
      fprintf(stderr, "Error -- pthread_create\n");
      exit(3);
    }
  }

  /* espera o término da execução de cada thread */
  for (i = number_of_threads - 1; i >= 0; i--) {
    if (pthread_join(threads_id[i], (void **) &thread_result)) {
      fprintf(stderr, "Error -- pthread_join\n");
      exit(3);
    }

    concurrent_sum += *thread_result;

    free(thread_result);
  }

  printf("\nResultado: %.15f (concorrente)\nErro: %.15f\n", concurrent_sum, concurrent_sum - M_PI);

  free(threads_id);

  GET_TIME(end);
  delta = end - start;
  printf("Tempo total: %lf (concorrente)\n", delta);

  return 0;
}

double task_sequential() {
  double result = 0.0;
  long int i;

  for (i = number_of_iterations - 1; i >= 0; i--) {
    if (i % 2 == 0) result += 4.0 / (2*i + 1);
    else result -= 4.0 / (2*i + 1);
  }

  return result;
}

void *task(void *arg) {
  long int thread_id, number_of_terms, start, end, i;
  double *local_sum;

  thread_id = (long int) arg; /* faz cast do argumento recebido */

  number_of_terms = number_of_iterations/number_of_threads; /* calcula a quantidade de termos da sequência que a thread vai calcular */
  start = thread_id * number_of_terms; /* verifica a partir de qual termo a thread vai começar a calcular */

  local_sum = (double *) malloc(sizeof(double)); /* aloca um double para retornar na saída da thread */
  if (local_sum == NULL) {
    fprintf(stderr, "Error -- malloc\n");
    exit(2);
  }

  /* verifica qual é o último termo que a thread irá calcular */
  if (thread_id == number_of_threads - 1) end = number_of_iterations - 1;
  else end = start + number_of_terms - 1;

  /* faz a soma parcial dos termos */
  for (i = end; i >= start; i--) {
    if (i % 2 == 0) *local_sum += 4.0 / (2*i + 1);
    else *local_sum -= 4.0 / (2*i + 1);
  }

  pthread_exit((void *) local_sum);
}

pthread_t *thread_alloc() {
  pthread_t *threads_id = (pthread_t *) malloc(sizeof(pthread_t) * number_of_threads);
  if (threads_id == NULL) {
    fprintf(stderr, "Error -- malloc\n");
    exit(2);
  }

  return threads_id;
}
