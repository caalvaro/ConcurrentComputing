#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARRAY_SIZE 20

typedef struct {
   int *array, initial_array_position, final_array_position;
} thread_args;

void *task(void *arg);
int *create_array();
thread_args *create_args(int *array, int initial_array_position, int final_array_position);

int main(int argc, char const *argv[]) {
  int i, *array;
  pthread_t thread_ids[2];
  thread_args *thread1_args, *thread2_args; /*threads arguments*/

  array = create_array();

  /*A primeira thread pegará o array da posição zero até a parte inteira da metade*/
  thread1_args = create_args(array, 0, ARRAY_SIZE / 2);
  /*A segunda thread pegará o array da metade até o final*/
  thread2_args = create_args(array, ARRAY_SIZE / 2, ARRAY_SIZE);

  /*Imprime o array antes de ser modificado pelas threads*/
  for (i = 0; i < ARRAY_SIZE; i++) {
    printf("%d ", array[i]);
  }
  printf("\n");

  /*Cria todas as threads*/
  if (pthread_create(&thread_ids[0], (void *) NULL, task, (void *) thread1_args))
    printf("Error -- pthread_create 1\n");

  if (pthread_create(&thread_ids[1], (void *) NULL, task, (void *) thread2_args))
    printf("Error -- pthread_create 2\n");

  /* Espera cada thread terminar a execução e pega o valor de retorno delas */
  if (pthread_join(thread_ids[0], (void*) NULL))
    printf("Error -- pthread_join 1\n");

  if (pthread_join(thread_ids[1], (void*) NULL))
    printf("Error -- pthread_join 2\n");

  /* Imprime o array que foi modificado pelas threads */
  printf("\n---- Resultado Array ----\n");
  for (i = 0; i < ARRAY_SIZE; i++) {
    printf("%d ", array[i]);
  }

  printf("\n");

  free(array);

  return 0;
}

void* task(void* arg) {
  int i;
  thread_args *args = (thread_args *) arg;

  for (i = args->initial_array_position; i < args->final_array_position; i++) {
    args->array[i] += 1;
  }

  free(args);

  pthread_exit(NULL);
}

int* create_array() {
  int i, *array;

  array = calloc(ARRAY_SIZE, sizeof(int));
  if (array == NULL) {
    printf("--ERROR: calloc\n"); exit(-1);
  }

  for (i = 0; i < ARRAY_SIZE; i++) {
    array[i] = i;
  }

  return array;
}

thread_args *create_args(int *array, int initial_array_position, int final_array_position) {
  thread_args *thread_args;

  thread_args = malloc(sizeof(thread_args));
  if (thread_args == NULL) {
    printf("--ERROR: malloc\n"); exit(-1);
  }

  thread_args->array = array;
  thread_args->initial_array_position = initial_array_position;
  thread_args->final_array_position = final_array_position;

  return thread_args;
}
