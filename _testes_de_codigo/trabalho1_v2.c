/* Concurrent implementations of Matrix-Vector Multiplication */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

typedef struct {
  long int player1;
  long int player2;
} round_t;

typedef struct {
  long int player1_score;
  long int player2_score;
} result_t;

typedef struct {
  round_t *rounds;
  result_t *game_result;
  int winner;
} game_t;

game_t *games;

int number_of_threads;
long int number_of_games;
long int game_size;

game_t *create_games();
void populate_games(game_t *games);
void print_matrix(float *matrix);

void *task(void *arg) {
  int thread_id = *((int *) arg);
  long int i, j, number_of_terms, start, end;
  game_t *current_game;
  round_t *current_round;

  number_of_terms = number_of_games/number_of_threads; /* calcula a quantidade de jogos que a thread vai processar */
  start = thread_id * number_of_terms; /* verifica a partir de qual jogo a thread vai começar a calcular */

  /* verifica qual é o último jogo que a thread irá calcular */
  if (thread_id == number_of_threads - 1) end = number_of_games;
  else end = start + number_of_terms;

  /* percorre todos os jogos pelos quais a thread está responsável */
  for (i = start; i < end; i++) {
    current_game = games + i;

    /* percorre as partidas de cada jogo para identificar qual jogador ganhou */
    for (j = 0; j < game_size; j++) {
      current_round = current_game->rounds + j;

      if (current_round->player1 > current_round->player2) {
        current_game->game_result->player1_score += 1;
      } else if (current_round->player2 > current_round->player1) {
        current_game->game_result->player2_score += 1;
      }
    }

    if (current_game->game_result->player1_score > current_game->game_result->player2_score) {
      current_game->winner = 1;
    } else if (current_game->game_result->player2_score > current_game->game_result->player1_score) {
      current_game->winner = 2;
    }
  }

  pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
  pthread_t *threads_array;
  int *thread_ids;
  int i;
  double start, end, delta, total = 0.0;

  GET_TIME(start);

  /* testa se a quantidade de argumentos passados é suficiente */
  if (argc < 4) {
    printf("Entre com os dados: %s <número de threads> <quantidade de jogos> <tamanho dos jogos>\n", argv[0]);
    return 1;
  }

  /* converte os argumentos passados na chamada do programa */
  number_of_threads = atoi(argv[1]);
  number_of_games = atoi(argv[2]);
  game_size = atoi(argv[3]);

  if (number_of_threads > number_of_games) number_of_threads = number_of_games;

  GET_TIME(end);
  delta = end - start;
  total += delta;
  printf("Inicialização: %lf\n", delta);

  GET_TIME(start);

  /* aloca as estruturas de dados que são usadas */
  games = create_games();

  /* fornece valores para as matrizes */
  populate_games(games);

  GET_TIME(end);
  delta = end - start;
  total += delta;
  printf("Criação do jogo: %lf\n", delta);

  GET_TIME(start);

  /* aloca um array com todos os ponteiros para as threads criadas */
  threads_array = (pthread_t *) malloc(sizeof(pthread_t) * number_of_threads);
  if (threads_array == NULL) {
    printf("ERROR -- Malloc\n");
    return 2;
  }

  /* aloca um array que contem os identificadores das threads */
  thread_ids = (int *) malloc(sizeof(pthread_t) * number_of_threads);
  if (thread_ids == NULL) {
    printf("ERROR -- Malloc\n");
    return 2;
  }

  GET_TIME(end);
  delta = end - start;
  total += delta;
  printf("Alocação das threads: %lf\n", delta);

  GET_TIME(start);

  /* cria as threads que irão executar a multiplicação */
  for (i = 0; i < number_of_threads; i++) {
    thread_ids[i] = i;

    if (pthread_create(threads_array + i, NULL, task, (void *) &thread_ids[i])) {
      printf("ERROR -- pthread_create\n");
      return 2;
    }
  }

  /* espera as threads terminarem sua execução */
  for (i = 0; i < number_of_threads; i++) {
    pthread_join(*(threads_array + i), NULL);
  }

  // for (i = 0; i < number_of_games; i++) {
  //   printf("\n---------------\nJOGO %d: Ganhador é o Player %d\n", i, (games+i)->winner);
  // }

  GET_TIME(end);
  delta = end - start;
  total += delta;
  printf("Execução das threads: %lf\n", delta);

  GET_TIME(start);

  free(games->rounds);
  free(games->game_result);
  free(games);
  free(threads_array);
  free(thread_ids);

  GET_TIME(end);
  delta = end - start;
  total += delta;
  printf("Free na memória: %lf\n", delta);

  printf("Total time: %lf\n", total);

  return 0;
}

game_t *create_games() {
  game_t *games;
  long int i;

  games = (game_t *) malloc(sizeof(game_t) * number_of_games);
  if (games == NULL) {
    printf("ERROR -- Malloc\n");
    exit(2);
  }

  for (i = 0; i < number_of_games; i++) {
    (games + i)->rounds = (round_t *) malloc(sizeof(round_t) * game_size);
    (games + i)->game_result = (result_t *) malloc(sizeof(result_t));
  }

  return games;
}

void populate_games(game_t *games) {
  long int i, j;
  game_t *current_game;
  round_t *current_round;

  for (i = 0; i < number_of_games; i++) {
    current_game = games + i;

    current_game->game_result->player1_score = 0;
    current_game->game_result->player2_score = 0;

    for (j = 0; j < game_size; j++) {
      current_round = current_game->rounds + j;

      current_round->player1 = 5;
      current_round->player2 = 10;
    }
  }
}

// void print_matrix(float *matrix) {
//   int i, j;
//   for (i = 0; i < matrix_dimension; i++) {
//     for (j = 0; j < matrix_dimension; j++) {
//       printf("%.1f ", result[i * matrix_dimension + j]);
//     }
//     puts("");
//   }
//   puts("");
// }
