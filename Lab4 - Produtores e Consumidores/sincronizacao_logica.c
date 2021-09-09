#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUMBER_OF_THREADS 4

int control = 0; // váriavel responsavel por controlar quantas das primeiras mensagens já foram imprimidas
pthread_mutex_t control_mutex; // variável de controle para evitar acesso mútuo à seção crítica do código
pthread_cond_t control_cond; // variável de controle para desbloquear determinadas threads baseadas em uma condição

void *print_first(void *arg);
void *print_second(void *arg);

int main(int argc, char const *argv[]) {
  pthread_t *threads;
  char *messages[4] = {"Bom dia!", "Boa tarde", "Tudo bem?", "Até mais!"};

  pthread_mutex_init(&control_mutex, NULL); // inicializa a variável que controla a exclusão mútua
  pthread_cond_init (&control_cond, NULL); // inicializa a variável que controla a sincronização lógica

  // aloca espaço para as threads
  threads = (pthread_t *) malloc(sizeof(pthread_t) * NUMBER_OF_THREADS);
  if (threads == NULL) {
    printf("ERROR -- malloc\n"); exit(2);
  }

  // cria a thread que imprime "Bom dia!"
  if (pthread_create(threads, NULL, print_first, (void *) messages[0])) {
    printf("ERROR -- pthread_create\n"); exit(3);
  }

  // cria a thread que imprime "Boa tarde!"
  if (pthread_create(threads, NULL, print_second, (void *) messages[1])) {
    printf("ERROR -- pthread_create\n"); exit(3);
  }

  // cria a thread que imprime "Tudo bem?"
  if (pthread_create(threads, NULL, print_first, (void *) messages[2])) {
    printf("ERROR -- pthread_create\n"); exit(3);
  }

  // cria a thread que imprime "Até mais!"
  if (pthread_create(threads, NULL, print_second, (void *) messages[3])) {
    printf("ERROR -- pthread_create\n"); exit(3);
  }

  pthread_exit(NULL);

  return 0;
}

void *print_first(void *arg) {
  char *message = (char *) arg; // recebe qual mensagem será imprimida pela thread

  printf("%s: Comecei\n", message);

  printf("%s\n", message);

  pthread_mutex_lock(&control_mutex); // evita que mais de uma thread execute a seção crítica
  control++;

  // se control é igual a 2 significa que uma outra thread já printou a mensagem que deveria
  // então essa sinaliza que já pode liberar as outras que estã esperando
  if (control==2) {
      printf("%s:  control = %d, vai sinalizar a condicao \n", message, control);
      pthread_cond_broadcast(&control_cond);
  }
  pthread_mutex_unlock(&control_mutex); // sinaliza que a seção crítica já finalizou

  pthread_exit(NULL);
}

void *print_second(void *arg) {
  char *message = (char *) arg; // recebe qual mensagem será imprimida pela thread

  printf("%s: Comecei\n", message);

  pthread_mutex_lock(&control_mutex);  // evita que mais de uma thread execute a seção crítica

  // se control é menor que 2 significa que as mensagens que deveriam ser impressas antes dessa thread imprimir ainda não foram impressas
  // então essa thread se bloqueia e aguarda que as outras a desbloqueie
  if (control < 2) {
      printf("%s:  control = %d, vai se bloquear... \n", message, control);
      pthread_cond_wait(&control_cond, &control_mutex);
      printf("%s: sinal recebido e mutex realocado, control = %d\n", message, control);
  }

  printf("%s\n", message);
  pthread_mutex_unlock(&control_mutex); // sinaliza que a seção crítica já finalizou

  pthread_exit(NULL);
}
