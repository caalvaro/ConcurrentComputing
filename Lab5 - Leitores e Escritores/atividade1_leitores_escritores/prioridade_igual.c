/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Álvaro de Carvalho */
/* Descricao: implementa  o problema dos leitores/escritores usando variaveis de condicao da biblioteca Pthread */
/*            Essa versão é programada para que a prioridade entre leitores e escritores seja igual */

#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>

#define ARRAY_SIZE 5

// macros para controlar os turnos
#define READER 1
#define WRITER 0

int number_of_readers, number_of_writers; // recebem na entrada do programa a quantidade de threads

//variaveis do problema
int readers = 0; //contador de threads lendo
int writers = 0; //contador de threads escrevendo
bool writer_is_waiting = false; // armazena se tem algum escritor esperando para escrever
bool reader_is_waiting = false; // armazena se tem algum leitor esperando para ler
int turn = READER; // armazena de quem é a vez de executar

//variaveis para sincronizacao
pthread_mutex_t mutex;
pthread_cond_t cond_reader, cond_writer;

int shared_array[ARRAY_SIZE];

//entrada leitura
void begin_reading(int id) {
  pthread_mutex_lock(&mutex);
  printf("L[%d] quer ler\n", id);

  // a thread se bloqueará caso haja algum escritor escrevendo ou algum escritor esperando para escrever
  // caso haja muitos escritores esperando para escrever, eles irão passar o controle pros leitores
  // dessa forma, os leitores e escritores farão suas operações de forma intercalada
  // ou seja, se tiver vários escritores esperando, um irá escrever e deixar o pŕoximo leitor ler
  // depois que o leitor ler, necessariamente passará o controle para um escritor e assim por diante
  while((writers > 0) || (writer_is_waiting == true && turn == WRITER)) {
    reader_is_waiting = true;

    if (writer_is_waiting == true) {
      printf("L[%d] bloqueou pois tem um escritor esperando para escrever\n", id);
      if (turn == WRITER && writers == 0) {
        printf("É a vez de um escritor escrever agora...\n");
      }
    } else {
      printf("L[%d] bloqueou pois já tem escritor escrevendo\n", id);
    }

    pthread_cond_wait(&cond_reader, &mutex);
    printf("L[%d] desbloqueou\n", id);

    reader_is_waiting = false;
  }

  readers++;
  pthread_mutex_unlock(&mutex);
}

//saida leitura
void finish_reading(int id) {
  pthread_mutex_lock(&mutex);
  printf("L[%d] terminou de ler\n", id);
  readers--;
  if(readers == 0) {
    turn = WRITER;
    pthread_cond_signal(&cond_writer);
  }
  pthread_mutex_unlock(&mutex);
}

//entrada escrita
void begin_writing(int id) {
  pthread_mutex_lock(&mutex);
  printf("E[%d] quer escrever\n", id);

  // a thread se bloqueará caso haja algum escritor escrevendo ou algum leitor lendo ou algum leitor esperando para ler
  // caso haja algum leitor esperando para ler, o escritor passará o controle pros leitores
  // dessa forma, os leitores e escritores farão suas operações de forma intercalada
  // ou seja, se tiver vários escritores esperando, um irá escrever e deixar o pŕoximo leitor ler
  // depois que o leitor ler, necessariamente passará o controle para um escritor e assim por diante
  while((readers > 0) || (writers > 0) || (reader_is_waiting == true && turn == READER)) {
    writer_is_waiting = true;

    if (readers>0) {
      printf("E[%d] bloqueou pois tem leitor lendo\n", id);
    } else if (reader_is_waiting == true) {
      printf("E[%d] bloqueou pois tem leitor esperando para ler\n", id);
      if (turn == READER && readers == 0) {
        printf("É a vez dos leitores agora...\n");
      }
    } else {
      printf("E[%d] bloqueou pois já tem escritor escrevendo\n", id);
    }

    pthread_cond_wait(&cond_writer, &mutex);
    printf("E[%d] desbloqueou\n", id);
    writer_is_waiting = false;
  }

  writers++;
  pthread_mutex_unlock(&mutex);
}

//saida escrita
void finish_writing(int id) {
  pthread_mutex_lock(&mutex);
  printf("E[%d] terminou de escrever\n", id);
  writers--;

  if (reader_is_waiting == true && readers == 0) {
    turn = READER;
    pthread_cond_broadcast(&cond_reader);
  } else {
    pthread_cond_signal(&cond_writer);
  }

  pthread_mutex_unlock(&mutex);
}

//thread leitora
void *reader(void *arg) {
  int *id = (int *) arg;
  while(1) {
    begin_reading(*id);

    printf("Leitora %d esta lendo\n", *id);

    printf(">>> L[%d]: %d %d %d %d %d\n", *id, shared_array[0], shared_array[1], shared_array[2], shared_array[3], shared_array[4]);

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

    printf(">>> Escritora %d esta escrevendo\n", *id);

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
  for(i = 0; i < number_of_readers; i++) {
    thread_ids[i] = i + 1;
    if(pthread_create(&threads[i], NULL, reader, (void *) &thread_ids[i])) exit(-1);
  }

  //cria as threads escritoras
  for(i = 0; i < number_of_writers; i++) {
    thread_ids[i + number_of_readers] = i + 1;
    if(pthread_create(&threads[i + number_of_readers], NULL, writer, (void *) &thread_ids[i + number_of_readers])) exit(-1);
  }

  pthread_exit(NULL);
  return 0;
}
