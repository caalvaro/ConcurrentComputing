/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 3 - Trabalho 3 */
/* Alunos: Álvaro de Carvalho Alves e Carla Moreno Barbosa */
/* Codigo: Implementação de leitura e processamento de arquivos */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "timer.h"

// indica se o programa está em modo debug, com printfs para indicar as etapas de execução
#define DEBUG_MODE 0

/* variáveis globais compartilhadas entre as threads */
char file_name[50]; // nome do arquivo a ser lido
long long int block_size; // tamanho do bloco em que o arquivo será lido (N)
long long int buffer_size; // quantidade de entradas de tamanho block_size (N) no buffer
int number_of_threads = 4; // quantidade de threads
long long int *buffer; // ponteiro para o buffer
int barrier_step = 0; // guarda o passo da barreira, usada apenas para debug

// resultados da função biggest_identical_sequence
long long int result_sequence_value, result_initial_sequence_position, result_biggest_identical_sequence_size;

// resultados da função size3_sequence_occurrences
long long int result_size3_sequence_occurrences;

// resultados da função sequence_012345_occurrences
long long int result_sequence_012345_occurrences;

/* variaveis de sincronização e exclusão mútua */
pthread_mutex_t barrier_mutex; // exclusão mútua para controlar a entrada e saída da barreira
pthread_mutex_t barrier_blocked_mutex; // exclusão mútua para controlar o acesso ao estado da barreira (se todas as threads estao bloqueadas)
pthread_cond_t can_read_buffer; // variável de condição para as threads se bloquearem caso não possam ler o buffer
pthread_cond_t can_read_file; // variável de condição para a thread leitora do arquivo ser sinalizada quando as threads estiverem bloqueadas na barreira
int threads_waiting = 0; // quantidade de threads bloqueadas na barreira
bool file_reading_ended = false; // guarda se a leitura do arquivo já finalizou
int number_of_file_reads = 0; // quantidade de vezes em que o buffer foi preenchido pela thread leitora

void *file_reader(void *arg); // função executada pela thread leitora do arquivos
void *biggest_identical_sequence(void *arg); // função executada pela thread que procura pela maior sequência de números idênticos
void *size3_sequence_occurrences(void *arg); // função executada pela thread que conta a quantidade de sequências de tamanho 3 de números idênticos
void *sequence_012345_occurrences(void *arg); // função executada pela thread que procura pela quantidade de sequências 012345
void barrier(long long int id); // função que implementa a barreira
void print_step(long long int id); // função que imprime o passo da barreira, usada apenas para debug

int main(int argc, char const *argv[]) {
  pthread_t *array_of_threads; // armazena um array com ponteiros para cada thread
  int i; // índice do for
  double start, end, initialization_time, thread_execution_time, free_memory_time, total = 0.0; // variáveis de controle de tempo

  GET_TIME(start);

  /* testa se a quantidade de argumentos passados é suficiente */
  if (argc < 4) {
    printf("Entre com os dados: %s <nome do arquivo> <tamanho do bloco (N)> <tamanho do buffer (M)>\n", argv[0]);
    return 1;
  }

  /* converte os argumentos passados na chamada do programa */
  strcpy(file_name, argv[1]);
  block_size = atoi(argv[2]);
  buffer_size = atoi(argv[3]);

  //inicializa as variaveis de sincronizacao
  pthread_mutex_init(&barrier_mutex, NULL);
  pthread_cond_init(&can_read_buffer, NULL);
  pthread_cond_init(&can_read_file, NULL);
  pthread_mutex_init(&barrier_blocked_mutex, NULL);

  /* aloca o buffer */
  /* o buffer será um vetor de tamanho buffer_size (M) de elementos de tamanho block_size (N) */
  /* essa estrutura será abstraída usando um array contíguo de tamanho block_size * buffer_size (N * M) */
  buffer = (long long int *) malloc(sizeof(long long int) * buffer_size * block_size);
  if (buffer == NULL) {
    printf("ERROR -- Malloc\n");
    return 2;
  }

  /* aloca um array com todos os ponteiros para as threads criadas */
  array_of_threads = (pthread_t *) malloc(sizeof(pthread_t) * number_of_threads);
  if (array_of_threads == NULL) {
    printf("ERROR -- Malloc\n");
    return 2;
  }

  GET_TIME(end);
  initialization_time = end - start;
  total += initialization_time;

  GET_TIME(start);

  /* cria as threads que irão executar o processamento do arquivo */
  if (pthread_create(array_of_threads, NULL, file_reader, (void *) 1)) {
    printf("ERROR -- pthread_create\n");
    return 2;
  }

  if (pthread_create(array_of_threads + 1, NULL, biggest_identical_sequence, (void *) 2)) {
    printf("ERROR -- pthread_create\n");
    return 2;
  }

  if (pthread_create(array_of_threads + 2, NULL, size3_sequence_occurrences, (void *) 3)) {
    printf("ERROR -- pthread_create\n");
    return 2;
  }

  if (pthread_create(array_of_threads + 3, NULL, sequence_012345_occurrences, (void *) 4)) {
    printf("ERROR -- pthread_create\n");
    return 2;
  }

  /* espera as threads terminarem sua execução */
  for (i = 0; i < number_of_threads; i++) {
    pthread_join(array_of_threads[i], NULL);
  }

  /* imprime a situação final do processamento */
  printf("\n------ RESULTADO DO PROCESSAMENTO -------\n");
  printf("Maior sequência: posição inicial %lld, tamanho %lld, valor %lld\n",
          result_initial_sequence_position,
          result_biggest_identical_sequence_size,
          result_sequence_value
        );
  printf("Sequências de tamanho 3: %lld\n", result_size3_sequence_occurrences);
  printf("Sequências 012345: %lld\n", result_sequence_012345_occurrences);

  GET_TIME(end);
  thread_execution_time = end - start;
  total += thread_execution_time;

  GET_TIME(start);

  /* libera a memória que foi alocada */
  free(array_of_threads);
  free(buffer);

  GET_TIME(end);
  free_memory_time = end - start;
  total += free_memory_time;

  /* imprime o tempo em cada etapa do código */
  printf("\n----------- TEMPO DE EXECUÇÃO -----------\n");
  printf("Inicialização das threads: %lf\n", initialization_time);
  printf("Tempo de execução das threads: %lf\n", thread_execution_time);
  printf("Tempo de free na memória: %lf\n", free_memory_time);
  printf("TEMPO TOTAL: %lf\n", total);

  return 0;
}

void *biggest_identical_sequence(void *arg) {
  long long int thread_id = (long long int) arg; // converte o argumento da thread
  long long int i, j; // variáveis do for
  long long int previous_number = -1, current_number, current_initial_sequence_position = -1, current_sequence_size = 1; // variáveis de controle da sequência sendo processada
  long long int biggest_identical_sequence_size = 1, initial_sequence_position = -1, sequence_value = -1; // armazena os dados da maior sequência encontrada


  if (DEBUG_MODE) {
    printf("Thread %lld: começou\n", thread_id);
  }

  while (!file_reading_ended) {
    barrier(thread_id); // entra na barreira para esperar o buffer ser preenchido

    // lógica de leitura do buffer
    // percorre cada entrada do buffer
    for (i = 0; i < buffer_size; i++) {
      // percorre os elementos da entrada i do buffer
      for (j = 0; j < block_size; j++) {
        // pega o elemento j da entrada i do buffer
        current_number = *(buffer + i * block_size + j);

        // se achou algum -1 no buffer é porque o arquivo terminou
        if (current_number == -1) {
          break;
        }

        if (previous_number == current_number) {
          // caso a posição inicial da sequência não tenha sido preenchida, preenche com a posição em que o elemento está no arquivo
          if (current_initial_sequence_position == -1) {
            current_initial_sequence_position = (number_of_file_reads - 1) * buffer_size * block_size + i * block_size + j;
          }

          current_sequence_size++;
        } else {
          if (current_sequence_size > biggest_identical_sequence_size) { // verifica se a sequência encontrada é a maior de todas
            // atualiza a sequência para a mais recente
            biggest_identical_sequence_size = current_sequence_size;
            sequence_value = previous_number;
            initial_sequence_position = current_initial_sequence_position;

            // reseta a sequência
            current_sequence_size = 1;
            current_initial_sequence_position = -1;
          }
        }

        previous_number = current_number;
      }
    }

    if (DEBUG_MODE) {
      print_step(thread_id);
    }
  }

  // ao final, atualiza as variáveis globais com o resultado do processamento
  result_sequence_value = sequence_value;
  result_initial_sequence_position = initial_sequence_position;
  result_biggest_identical_sequence_size = biggest_identical_sequence_size;

  if (DEBUG_MODE) {
    printf("Thread %lld: terminou\n", thread_id);
  }

  pthread_exit(NULL);
}

void *size3_sequence_occurrences(void *arg) {
  long long int thread_id = (long long int) arg; // converte o argumento da thread
  long long int i, j; // variáveis do for
  long long int previous_number = -1, current_number, current_sequence_size = 1; // variáveis de controle da sequência sendo processada
  long long int size3_sequence_occurrences = 0; // quantidade total de sequências iguais de tamanho 3

  if (DEBUG_MODE) {
    printf("Thread %lld: começou\n", thread_id);
  }

  while (!file_reading_ended) {
    barrier(thread_id); // entra na barreira para esperar o buffer ser preenchido

    //lógica de leitura do buffer
    // percorre as entradas do buffer
    for (i = 0; i < buffer_size; i++) {
      // pega o elemento j da entrada i do buffer
      for (j = 0; j < block_size; j++) {
        // pega o elemento j da entrada i do buffer
        current_number = *(buffer + i * block_size + j);

        // se achou algum -1 no buffer é porque o arquivo terminou
        if (current_number == -1) {
          break;
        }

        // se a sequência se mantiver idêntica, incrementa o tamanho até chegar numa sequência de tamanho 3
        if (previous_number == current_number) {
          current_sequence_size++;

          // se a sequência tiver tamanho 3, atualiza o número de sequências e reseta a sequência que está sendo processada
          if (current_sequence_size == 3) {
            size3_sequence_occurrences++;
            current_sequence_size = 1;
            previous_number = -1;
            continue;
          }
        }

        previous_number = current_number;
      }
    }

    if (DEBUG_MODE) {
      print_step(thread_id);
    }
  }

  // ao final, atualiza as variáveis globais com o resultado do processamento
  result_size3_sequence_occurrences = size3_sequence_occurrences;

  if (DEBUG_MODE) {
    printf("Thread %lld: terminou\n", thread_id);
  }

  pthread_exit(NULL);
}

void *sequence_012345_occurrences(void *arg) {
  long long int thread_id = (long long int) arg; // converte o argumento da thread
  long long int i, j; // variáveis do for
  long long int previous_number = -1, sequence_size = 0, current_number; // variáveis de controle da sequência sendo processada
  long long int sequence_occurrences = 0; // quantidade total de sequências 012345

  if (DEBUG_MODE) {
    printf("Thread %lld: começou\n", thread_id);
  }

  while (!file_reading_ended) {
    barrier(thread_id); // entra na barreira para esperar o buffer ser preenchido

    // lógica de leitura do buffer
    // percorre as entradas do buffer
    for (i = 0; i < buffer_size; i++) {
      // pega o elemento j da entrada i do buffer
      for (j = 0; j < block_size; j++) {
        // pega o elemento j da entrada i do buffer
        current_number = *(buffer + i * block_size + j);

        // se achou algum -1 no buffer é porque o arquivo terminou
        if (current_number == -1) {
          break;
        }

        // se estiver numa possível sequência, verifica se os números vão crescendo até chegar numa sequência de tamanho 6
        if (current_number == previous_number + 1) { // se a sequência se mantém
          sequence_size++;
          previous_number = current_number;

          // se chegou ao final de uma sequência 012345, incrementa a quantidade total e reseta a sequência
          if (sequence_size == 6) {
            sequence_occurrences++;
            sequence_size = 0;
            previous_number = -1;
          }
        } else { // se a sequência têm alguma inconsistência
          sequence_size = 0;
          previous_number = -1;
        }
      }
    }

    if (DEBUG_MODE) {
      print_step(thread_id);
    }
  }

  // ao final, atualiza as variáveis globais com o resultado do processamento
  result_sequence_012345_occurrences = sequence_occurrences;

  if (DEBUG_MODE) {
    printf("Thread %lld: terminou\n", thread_id);
  }

  pthread_exit(NULL);
}

void *file_reader(void *arg) {
  FILE *file = fopen(file_name, "r"); // ponteiro para o arquivo
  long long int file_size; // tamanho do arquivo
  long long int i, j; // variáveis do for
  long long int read_numbers; // quantidade de números do bloco que foram de fatos lidos pela função fread

  if (DEBUG_MODE) {
    printf("Thread leitora começou\n");
  }

  // lê o primeiro número do arquivo, que contém a quantidade de números a serem processados
  fread(&file_size, sizeof(long long int), 1, file);

  // percorre o arquivo até acabar
  for (i = 0; i <= file_size / (buffer_size * block_size); i++) {
    pthread_mutex_lock(&barrier_mutex); // exclusão mútua para acessar a variável global

    // se ainda há alguma thread processando, se bloqueia
    if (threads_waiting < (number_of_threads - 1)) {
      if (DEBUG_MODE) {
        printf("Thread Leitora vai se bloquear\n");
      }

      pthread_cond_wait(&can_read_file, &barrier_mutex);

      if (DEBUG_MODE) {
        printf("Thread Leitora foi sinalizada\n");
      }
    }

    // lógica de leitura do arquivo e atualização do buffer
    // lê cada entrada do buffer
    for (j = 0; j < buffer_size; j++) {
      read_numbers = fread(buffer + j * block_size, sizeof(long long int), block_size, file);

      // se chegou ao fim do arquivo
      if (read_numbers < block_size) {
        // preenche o resto do buffer com -1
        for (size_t k = read_numbers; k < block_size; k++) {
          *(buffer + j * block_size + k) = -1;
        }
      }
    }

    number_of_file_reads++;

    if (DEBUG_MODE) {
      printf("\n\n");
    }

    threads_waiting = 0;
    barrier_step++;

    // se for a última iteração, sinaliza que o arquivo acabou
    if (i == file_size / (buffer_size * block_size)) {
      file_reading_ended = true;
    }

    if (DEBUG_MODE) {
      printf("\nThread Leitora desbloqueou a barreira\n");
    }

    // exclusão mútua necessária para garantir que a thread leitora só desbloqueará as threads quando todas estiverem bloqueadas
    pthread_mutex_lock(&barrier_blocked_mutex);
    pthread_cond_broadcast(&can_read_buffer);
    pthread_mutex_unlock(&barrier_blocked_mutex);

    pthread_mutex_unlock(&barrier_mutex);
  }

  if (DEBUG_MODE) {
    printf("Thread Leitora: terminou\n");
  }

  pthread_exit(NULL);
}

void barrier(long long int id) {
    pthread_mutex_lock(&barrier_mutex); //inicio seção crítica
    threads_waiting++;

    if (DEBUG_MODE) {
      printf("Thread %lld entrou na barreira, threads waiting %d\n", id, threads_waiting);
    }

    // se ainda há outras threads para chegar aqui, se bloqueia
    if (threads_waiting < (number_of_threads-1)) {
      if (DEBUG_MODE) {
        printf("Thread %lld vai se bloquear\n", id);
      }

      pthread_cond_wait(&can_read_buffer, &barrier_mutex);

      if (DEBUG_MODE) {
        printf("Thread %lld se desbloqueou\n", id);
      }
    } else { // se for a última thread a chegar na barreira

      if (DEBUG_MODE) {
        printf("Thread %lld: barreira encontrada\n", id);
      }

      // sinaliza que a última thread está bloqueada e a barreira está esperando para ser desbloqueada
      pthread_mutex_lock(&barrier_blocked_mutex);
      // sinaliza a thread leitora do arquivo que ela pode ler e atualizar o buffer
      pthread_cond_signal(&can_read_file);

      if (DEBUG_MODE) {
        printf("Thread %lld vai se bloquear\n", id);
      }

      pthread_mutex_unlock(&barrier_mutex);
      pthread_cond_wait(&can_read_buffer, &barrier_blocked_mutex);

      if (DEBUG_MODE) {
        printf("Thread %lld se desbloqueou\n", id);
      }
      pthread_mutex_unlock(&barrier_blocked_mutex);

      pthread_mutex_lock(&barrier_mutex);
    }
    pthread_mutex_unlock(&barrier_mutex); //fim seção critica
}

void print_step(long long int id) {
  printf("Thread %lld: Passo = %d\n", id, barrier_step);
}
