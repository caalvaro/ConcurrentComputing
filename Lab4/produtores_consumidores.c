#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int shared_area_size, in = 0, out = 0; // váriavel responsavel por controlar quantas das primeiras mensagens já foram imprimidas
pthread_mutex_t shared_area_size_mutex; // variável de controle para evitar acesso mútuo à seção crítica do código
pthread_cond_t shared_area_size_cond; // variável de controle para desbloquear determinadas threads baseadas em uma condição
int *shared_area;

void insert(long int number) {
  shared_area[in] = number;
}

void *producer(void *arg) {
  long int id = (long int) arg;

  printf("Produtor %ld: Comecei\n", id);

  pthread_mutex_lock(&shared_area_size_mutex); // evita que mais de uma thread execute a seção crítica
  shared_area_size++;

  while (out == shared_area_size) {
      printf("Produtor %ld:  out = %d, vai sinalizar a condicao \n", id, out);
      pthread_cond_broadcast(&shared_area_size_cond);
  }
  pthread_mutex_unlock(&shared_area_size_mutex); // sinaliza que a seção crítica já finalizou

  pthread_exit(NULL);
}

void *consumer(void *arg) {
  long int id = (long int) arg;

  printf("Produtor %ld: Comecei\n", id);

  pthread_mutex_lock(&shared_area_size_mutex); // evita que mais de uma thread execute a seção crítica
  shared_area_size++;

  while (out == shared_area_size) {
    printf("Consumidor %ld:  out = %d, vai se bloquear... \n", id, out);
    pthread_cond_wait(&shared_area_size_cond, &shared_area_size_mutex);
    printf("Consumidor %ld: sinal recebido e mutex realocado, out = %d\n", id, out);
  }
  pthread_mutex_unlock(&shared_area_size_mutex); // sinaliza que a seção crítica já finalizou

  pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
  pthread_t *threads;
  int number_of_producers, number_of_consumers;
  long int i;

  /* testa se a quantidade de argumentos passados é suficiente */
  if (argc < 4) {
    printf("Entre com os dados: %s <número de produtores> <número de consumidores> <tamanho da área compartilhada>\n", argv[0]);
    return 1;
  }

  /* converte os argumentos passados na chamada do programa */
  number_of_producers = atoi(argv[1]);
  number_of_consumers = atoi(argv[2]);
  shared_area_size = atoi(argv[3]);

  pthread_mutex_init(&shared_area_size_mutex, NULL); // inicializa a variável que controla a exclusão mútua
  pthread_cond_init (&shared_area_size_cond, NULL); // inicializa a variável que controla a sincronização lógica

  // aloca espaço para as threads
  threads = (pthread_t *) malloc(sizeof(pthread_t) * (number_of_consumers + number_of_producers));
  if (threads == NULL) {
    printf("ERROR -- malloc\n"); exit(2);
  }

  shared_area = (int *) malloc(sizeof(int) * shared_area_size);
  if (shared_area == NULL) {
    printf("ERROR -- malloc\n"); exit(2);
  }

  for (i = 0; i < number_of_producers; i++) {
    if (pthread_create(threads, NULL, producer, (void *) i)) {
      printf("ERROR -- pthread_create\n"); exit(3);
    }
  }

  for (i = 0; i < number_of_consumers; i++) {
    if (pthread_create(threads, NULL, consumer, (void *) i)) {
      printf("ERROR -- pthread_create\n"); exit(3);
    }
  }

  pthread_exit(NULL);

  return 0;
}
