#include <stdio.h>
#include <pthread.h>

#define NUMBER_OF_THREADS 10

// Variável global que as threads vão compartilhar e modificar
int array[NUMBER_OF_THREADS];

// Função que será executada pelas threads
void* task(void* arg) {
  int thread_number = * (int *) arg; // Tipa o argumento passado pra thread

  array[thread_number - 1] = thread_number;

  printf("Hello, I'm the thread %d!\n", thread_number);
  pthread_exit(NULL); // Indica que a thread pode terminar, passando um valor de retorno
}

int main(int argc, char const *argv[]) {
  int i;
  pthread_t thread_ids[NUMBER_OF_THREADS]; // Ponteiros para cada thread criada, para que se possa gerenciar cada uma durante o programa
  int threads_numbers[NUMBER_OF_THREADS];

  // Cria todas as threads
  for (i = 0; i < NUMBER_OF_THREADS; i++) {
    threads_numbers[i] = i + 1;

    if (pthread_create(&thread_ids[i], (void*) NULL, task, (void*) &threads_numbers[i]))
      printf("Error -- pthread_create\n");
  }

  // Espera cada thread terminar a execução e pega o valor de retorno delas
  for (i = 0; i < NUMBER_OF_THREADS; i++) {
    if (pthread_join(thread_ids[i], (void*) NULL))
      printf("Error -- pthread_join\n");
  }

  printf("Hello, I'm the Main Thread\n");

  // Imprime o array que foi modificado pelas threads
  for (i = 0; i < NUMBER_OF_THREADS; i++) {
    printf("%d ", array[i]);
  }

  printf("\n");

  return 0;
}
