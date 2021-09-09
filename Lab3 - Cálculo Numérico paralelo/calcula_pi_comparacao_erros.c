#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

/* variáveis globais que serão acessadas pelas threads */
long int number_of_iterations;
int number_of_threads;

/* função que será executada pelas threads */
void *task1(void *arg);
void *task4(void *arg);
void *task_large_to_small(void *arg);
pthread_t *thread_alloc();
void error_test(char *description, void *function(void *arg));

int main(int argc, char const *argv[]) {
  /* avalia se a quantidade de argumentos fornecidos é suficiente */
  if (argc < 3) {
    fprintf(stderr, "enter: %s <number of iterations> <numeber of threads>\n", argv[0]);
    return  1;
  }

  /* converte os argumentos fornecidos para o programa */
  number_of_iterations = atoll(argv[1]);
  number_of_threads = atoi(argv[2]);

  error_test("Quando calcula todos os termos e só multiplica por quatro no final", task1);
  error_test("Quando multiplica cada termo por 4 individualmente", task4);
  error_test("Quando a função soma do maior valor pro menor valor", task_large_to_small);

  return 0;
}

void *task1(void *arg) {
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

  for (i = end; i >= start; i--) {
    if (i % 2 == 0) *local_sum += 1.0 / (2*i + 1);
    else *local_sum -= 1.0 / (2*i + 1);
  }

  *local_sum *= 4.0;

  pthread_exit((void *) local_sum);
}

void *task4(void *arg) {
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

  for (i = end; i >= start; i--) {
    if (i % 2 == 0) *local_sum += 4.0 / (2*i + 1);
    else *local_sum -= 4.0 / (2*i + 1);
  }

  pthread_exit((void *) local_sum);
}

void *task_large_to_small(void *arg) {
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
  if (thread_id == number_of_threads - 1) end = number_of_iterations;
  else end = start + number_of_terms;

  for (i = start; i < end; i++) {
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

void error_test(char *description, void *function(void *arg)) {
  pthread_t *threads_id; /* array com os ponteiros para as threads criadas */
  long int i; /* índice usado no for */
  double *thread_result, concurrent_sum = 0.0; /* resultado parcial e total da soma */
  double start, end, delta; /* variáveis de controle de tempo */
  double pi = 3.141592653589793238462643383279;

  GET_TIME(start);
  /* aloca o array de threads */
  threads_id = thread_alloc();

  /* cria todas as threads que serão executadas */
  for (i = 0; i < number_of_threads; i++) {
    if (pthread_create(threads_id+i, NULL, function, (void *) i)) {
      fprintf(stderr, "Error -- pthread_create\n");
      exit(3);
    }
  }

  /* espera o término da execução de cada thread */
  for (i = 0; i < number_of_threads; i++) {
    if (pthread_join(threads_id[i], (void **) &thread_result)) {
      fprintf(stderr, "Error -- pthread_join\n");
      exit(3);
    }

    concurrent_sum += *thread_result;

    free(thread_result);
  }

  printf("\nResultado: %.30f %s\nErro: %.30f\n", concurrent_sum, description, concurrent_sum - pi);

  free(threads_id);

  GET_TIME(end);
  delta = end - start;
  printf("Total time: %lf\n", delta);
}
