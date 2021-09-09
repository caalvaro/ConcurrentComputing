/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Álvaro de Carvalho */
/* Descricao: implementa  o problema dos leitores/escritores usando variaveis de condicao da biblioteca Pthread */
/*            Essa versão é programada para não haver prioridade entre leitores e escritores */

#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define ARRAY_SIZE 5

int number_of_readers, number_of_writers;

//variaveis do problema
int readers = 0; //contador de threads lendo
int writers = 0; //contador de threads escrevendo

//variaveis para sincronizacao
pthread_mutex_t mutex;
pthread_cond_t cond_reader, cond_writer;

int shared_array[ARRAY_SIZE];

//entrada leitura
void begin_reading(int id) {
   pthread_mutex_lock(&mutex);
   printf("L[%d] quer ler\n", id);

   while(writers > 0) {
     printf("L[%d] bloqueou\n", id);
     pthread_cond_wait(&cond_reader, &mutex);
     printf("L[%d] desbloqueou\n", id);
   }

   readers++;
   pthread_mutex_unlock(&mutex);
}

//saida leitura
void finish_reading(int id) {
   pthread_mutex_lock(&mutex);
   printf("L[%d] terminou de ler\n", id);
   readers--;
   if(readers==0) pthread_cond_signal(&cond_writer);
   pthread_mutex_unlock(&mutex);
}

//entrada escrita
void begin_writing(int id) {
   pthread_mutex_lock(&mutex);
   printf("E[%d] quer escrever\n", id);

   while((readers>0) || (writers>0)) {
     printf("E[%d] bloqueou\n", id);
     pthread_cond_wait(&cond_writer, &mutex);
     printf("E[%d] desbloqueou\n", id);
   }

   writers++;
   pthread_mutex_unlock(&mutex);
}

//saida escrita
void finish_writing(int id) {
   pthread_mutex_lock(&mutex);
   printf("E[%d] terminou de escrever\n", id);
   writers--;
   pthread_cond_signal(&cond_writer);
   pthread_cond_broadcast(&cond_reader);
   pthread_mutex_unlock(&mutex);
}

//thread leitora
void *reader(void *arg) {
  int *id = (int *) arg, i;
  while(1) {
    begin_reading(*id);

    printf("Leitora %d esta lendo\n", *id);

    for (i = 0; i < ARRAY_SIZE; i++) {
      printf("%d ", shared_array[i]);
    }
    printf("\n");

    finish_reading(*id);
    sleep(1);
  }
  free(arg);
  pthread_exit(NULL);
}

//thread escritora
void *writer(void *arg) {
  int *id = (int *) arg, i;
  while(1) {
    begin_writing(*id);

    printf("Escritora %d esta escrevendo\n", *id);

    shared_array[0] = *id;
    shared_array[ARRAY_SIZE - 1] = *id;

    for (i = 1; i < ARRAY_SIZE - 1; i++) {
      shared_array[i] = 2 * *id;
    }

    finish_writing(*id);
    sleep(1);
  }
  free(arg);
  pthread_exit(NULL);
}

//funcao principal
int main(int argc, char const *argv[]) {
  //identificadores das threads
  pthread_t *threads;
  int *thread_ids, i;

  /* testa se a quantidade de argumentos passados é suficiente */
  if (argc < 3) {
    printf("Entre com os dados: %s <número de leitores> <número de escritores>\n", argv[0]);
    return 1;
  }

  /* converte os argumentos passados na chamada do programa */
  number_of_readers = atoi(argv[1]);
  number_of_writers = atoi(argv[2]);

  threads = (pthread_t *) malloc(sizeof(pthread_t) * (number_of_readers + number_of_writers));
  thread_ids = (int *) malloc(sizeof(pthread_t) * (number_of_readers + number_of_writers));

  //inicializa as variaveis de sincronizacao
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_reader, NULL);
  pthread_cond_init(&cond_writer, NULL);

  // zera os elementos do array
  for (i = 0; i < ARRAY_SIZE; i++) {
    shared_array[i] = 0;
  }

  //cria as threads leitoras
  for(i=0; i<number_of_readers; i++) {
    thread_ids[i] = i+1;
    if(pthread_create(&threads[i], NULL, reader, (void *) &thread_ids[i])) exit(-1);
  }

  //cria as threads escritoras
  for(i=0; i<number_of_writers; i++) {
    thread_ids[i+number_of_readers] = i+1;
    if(pthread_create(&threads[i+number_of_readers], NULL, writer, (void *) &thread_ids[i+number_of_readers])) exit(-1);
  }

  pthread_exit(NULL);
  return 0;
}
