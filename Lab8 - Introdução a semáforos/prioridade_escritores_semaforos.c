/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Álvaro de Carvalho */
/* Descricao: implementa  o problema dos leitores/escritores usando variaveis de condicao da biblioteca Pthread */
/*            Essa versão é programada para que os escritores tenham prioridade sobre os leitores */

#include<pthread.h>
#include<semaphore.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>

int number_of_readers, number_of_writers; // recebem na entrada do programa a quantidade de threads

//variaveis para sincronizacao
sem_t em_e, em_l, escr, leit;
int e = 0, l = 0;

//thread leitora
void *reader(void *arg) {
  int *id = (int *) arg;
  while(1) {
    //sem_wait(&leit); // redundante aqui
    sem_wait(&em_l); // exlusão mútua para acesso a variável global l
    printf("Leitora %d quer ler\n", *id);

    l++;

    if (l == 1) // se é a primeira leitora
      sem_wait(&escr); // bloqueia a entrada de escritores

    sem_post(&em_l);
    //sem_post(&leit); // redundante

    // faz leitura
    printf("Leitora %d leu\n", *id);

    sem_wait(&em_l); // exlusão mútua para acesso a variável global l

    l--;

    if (l == 0) // se é a última leitora
      sem_post(&escr); // permite a entrada de escritores

    sem_post(&em_l);
    sleep(1);
  }
  free(arg);
  pthread_exit(NULL);
}

//thread escritora
void *writer(void *arg) {
  int *id = (int *) arg;
  while(1) {
    sem_wait(&em_e); // exlusão mútua para acesso a variável global e
    printf("Escritora %d quer escrever\n", *id);

    e++;

    if (e == 1) sem_wait(&leit);

    //sem_post(&em_e); // podemos remover

    //sem_wait(&escr); // podemos remover
    // faz escrita
    printf("Escritora %d escreveu\n", *id);
    //sem_post(&escr); // podemos remover

    //sem_wait(&em_e); // podemos remover

    e--;

    if (e == 0) sem_post(&leit);

    sem_post(&em_e); // fim da seção crítica
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
  sem_init(&em_e, 0, 1);
  sem_init(&em_l, 0, 1);
  sem_init(&escr, 0, 1);
  sem_init(&leit, 0, 1);

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
