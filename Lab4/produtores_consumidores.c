#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include<unistd.h>

int shared_area_size, in = 0, out = 0, count = 0; // váriavel responsavel por controlar quantas das primeiras mensagens já foram imprimidas
pthread_mutex_t shared_area_size_mutex; // variável de controle para evitar acesso mútuo à seção crítica do código
pthread_cond_t cond_producer, cond_consumer; // variável de controle para desbloquear determinadas threads baseadas em uma condição
int *shared_area;

void print_area() {
  int i;
  for (i = 0; i < shared_area_size; i++) {
    printf("%d ", shared_area[i]);
  }
  printf("\n");
}

void insert(long int number) {
  shared_area[in] = number;
  count++;
}

void consume(long int number) {
  shared_area[out] = 0;
  count--;
}

void *producer(void *arg) {
  long int id = (long int) arg;

  printf("Produtor %ld: Comecei\n", id);

  while(1) {
    pthread_mutex_lock(&shared_area_size_mutex); // evita que mais de uma thread execute a seção crítica

    while (count == shared_area_size) {
        printf("Produtor %ld: produção cheia in = %d, vai sinalizar a condicao \n", id, out);
        pthread_cond_wait(&cond_producer, &shared_area_size_mutex);
    }

    insert(id);
    in = (in + 1) % shared_area_size;

    printf("Produtor %ld produziu: ", id);
    print_area();

    pthread_mutex_unlock(&shared_area_size_mutex); // sinaliza que a seção crítica já finalizou
    pthread_cond_signal(&cond_consumer);
    sleep(1);
  }

  pthread_exit(NULL);
}

void *consumer(void *arg) {
  long int id = (long int) arg;

  printf("Consumidor %ld: Comecei\n", id);

  while (1) {
    pthread_mutex_lock(&shared_area_size_mutex); // evita que mais de uma thread execute a seção crítica

    while (count == 0) {
      printf("Consumidor %ld: produção vazia out = %d, vai se bloquear... \n", id, out);
      pthread_cond_wait(&cond_consumer, &shared_area_size_mutex);
      printf("Consumidor %ld: sinal recebido e mutex realocado, out = %d\n", id, out);
    }

    consume(id);
    out = (out + 1) % shared_area_size;

    printf("Consumidor %ld consumiu: ", id);
    print_area();

    pthread_mutex_unlock(&shared_area_size_mutex); // sinaliza que a seção crítica já finalizou
    pthread_cond_signal(&cond_producer);
  }

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
  pthread_cond_init(&cond_producer, NULL); // inicializa a variável que controla a sincronização lógica
  pthread_cond_init(&cond_consumer, NULL); // inicializa a variável que controla a sincronização lógica

  // aloca espaço para as threads
  threads = (pthread_t *) malloc(sizeof(pthread_t) * (number_of_consumers + number_of_producers));
  if (threads == NULL) {
    printf("ERROR -- malloc\n"); exit(2);
  }

  shared_area = (int *) malloc(sizeof(int) * shared_area_size);
  if (shared_area == NULL) {
    printf("ERROR -- malloc\n"); exit(2);
  }

  for (i = 1; i <= number_of_producers; i++) {
    if (pthread_create(threads, NULL, producer, (void *) i)) {
      printf("ERROR -- pthread_create\n"); exit(3);
    }
  }

  for (i = 1; i <= number_of_consumers; i++) {
    if (pthread_create(threads, NULL, consumer, (void *) i)) {
      printf("ERROR -- pthread_create\n"); exit(3);
    }
  }

  pthread_exit(NULL);

  return 0;
}
