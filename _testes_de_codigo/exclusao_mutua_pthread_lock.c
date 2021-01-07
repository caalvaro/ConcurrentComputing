#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define MIN 10
#define MAX 10

int x = 10;
int queroEntrar_0 = 0, queroEntrar_1 = 0, turn;
pthread_mutex_t mutex;

void *t0(void *arg) {

  while (1) {
    pthread_mutex_lock(&mutex);

    x--;
    x++;

    if (x <MIN || x > MAX) {
      printf("x = %d\n", x);
    }

    pthread_mutex_unlock(&mutex);
  }

  pthread_exit(NULL);
}

void *t1(void *arg) {

  while (1) {
    pthread_mutex_lock(&mutex);
    x--;
    x++;

    if (x <MIN || x > MAX) {
      printf("\n\n------------ x = %d\n", x);
    }
    pthread_mutex_unlock(&mutex);
  }

  pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
  pthread_t thread0, thread1;

  pthread_mutex_init(&mutex, NULL);

  if (pthread_create(&thread0, NULL, t0, (void*) NULL)) {
    printf("--ERRO: pthread_create()\n"); exit(-1);
  }

  if (pthread_create(&thread1, NULL, t1, (void*) NULL)) {
    printf("--ERRO: pthread_create()\n"); exit(-1);
  }


  pthread_exit(NULL);
  pthread_mutex_destroy(&mutex);
  return 0;
}
