/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Trabalho 1 */
/* Alunos: Álvaro de Carvalho Alves e Carla Moreno Barbosa */
/* Codigo: Implementação concorrente do Jogo do maior */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

/* struct para armazenar a entrada dos jogadores em cada rodada */
typedef struct {
  long int player1;
  long int player2;
} Round;

/* struct para armazerar o resultado de cada jogo */
typedef struct {
  long int player1_score;
  long int player2_score;
} Result;

/* struct para armazenar cada jogo */
typedef struct {
  Round *rounds;
  Result *game_result;
  int winner;
} Game;

Game *games; /* variável global para armazenar a estrutura de dados que controla todo jogo */

int number_of_threads;
long int number_of_games;
long int game_size;

void create_games_concurrent(); /* controla a alocação e preenchimento da estrutura de dados do jogo */
void *result_round_alloc(void *arg); /* função executada pelas threads que alocam e preenchem a estrutura de dados do jogo */
void *game_process(void *arg); /* função executada pelas threads que processam a estrutura de dados do jogo e identificam os vencedores */

int main(int argc, char const *argv[]) {
  pthread_t *array_of_threads; /* armazena um array com ponteiros para cada thread */
  int *thread_ids; /* armazena o identificador de cada thread */
  int i; /* índice do for */
  double start, end, game_creating_time, thread_execution_time, free_memory_time, total = 0.0; /* variáveis de controle de tempo */

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

  /* aloca e preeche de maneira concorrente as estruturas de dados que são usadas */
  create_games_concurrent();

  GET_TIME(end);
  game_creating_time = end - start;
  total += game_creating_time;

  GET_TIME(start);

  /* aloca um array com todos os ponteiros para as threads criadas */
  array_of_threads = (pthread_t *) malloc(sizeof(pthread_t) * number_of_threads);
  if (array_of_threads == NULL) {
    printf("ERROR -- Malloc\n");
    return 2;
  }

  /* aloca um array que contem os identificadores das threads */
  thread_ids = (int *) malloc(sizeof(pthread_t) * number_of_threads);
  if (thread_ids == NULL) {
    printf("ERROR -- Malloc\n");
    return 2;
  }

  /* cria as threads que irão executar a multiplicação */
  for (i = 0; i < number_of_threads; i++) {
    thread_ids[i] = i;

    if (pthread_create(array_of_threads + i, NULL, game_process, (void *) &thread_ids[i])) {
      printf("ERROR -- pthread_create\n");
      return 2;
    }
  }

  /* espera as threads terminarem sua execução */
  for (i = 0; i < number_of_threads; i++) {
    pthread_join(*(array_of_threads + i), NULL);
  }

  /* imprime a situação final de cada jogo */
  // printf("\n----- RESULTADO DOS JOGOS -----\n");
  // for (i = 0; i < number_of_games; i++) {
  //   if ((games+i)->game_result->player1_score == (games+i)->game_result->player2_score) {
  //     printf("JOGO %d: EMPATE!\n", i);
  //   } else {
  //     printf("\nJOGO %d: Ganhador é o Player %d\n", i, (games+i)->winner);
  //   }
  // }

  GET_TIME(end);
  thread_execution_time = end - start;
  total += thread_execution_time;

  GET_TIME(start);

  /* libera a memória que foi alocada */
  free(games->rounds);
  free(games->game_result);
  free(games);
  free(array_of_threads);
  free(thread_ids);

  GET_TIME(end);
  free_memory_time = end - start;
  total += free_memory_time;

  /* imprime o tempo em cada etapa do código */
  printf("\n----------- TEMPO DE EXECUÇÃO -----------\nCriação do jogo: %lf\n", game_creating_time);
  printf("Tempo de execução das threads: %lf\n", thread_execution_time);
  printf("Tempo de free na memória: %lf\n", free_memory_time);
  printf("TEMPO TOTAL: %lf\n", total);

  return 0;
}

void *game_process(void *arg) {
  int thread_id = *((int *) arg); /* converte o argumento da thread */
  int i, j; /* índices do for */
  int number_of_terms, first_game, last_game; /* variáveis de controle para o for */
  Game *current_game; /* armazena o jogo que a thread está processando no momento */
  Round *current_round; /* armazena a rodada que a thread está processando no momento */

  number_of_terms = number_of_games/number_of_threads; /* calcula a quantidade de jogos que a thread vai processar */

  /* faz o balanceamento de carga, evitando que a última thread fique com muito mais elementos para processar */
  if (thread_id < (number_of_games % number_of_threads)) {
    number_of_terms++;
  }

  first_game = thread_id * number_of_terms; /* verifica a partir de qual jogo a thread vai começar a calcular */

  /* verifica qual é o último jogo que a thread irá calcular */
  if (thread_id == number_of_threads - 1) last_game = number_of_games;
  else last_game = first_game + number_of_terms;

  /* percorre todos os jogos pelos quais a thread está responsável */
  for (i = first_game; i < last_game; i++) {
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

void *result_round_alloc(void *arg) {
  int thread_id = *((int *) arg); /* converte o argumento da thread */
  int i, j; /* índices do for */
  int number_of_terms, first_game, last_game; /* variáveis de controle para o for */
  Game *current_game; /* armazena o jogo que a thread está processando no momento */
  Round *current_round; /* armazena a rodada que a thread está processando no momento */

  number_of_terms = number_of_games/number_of_threads; /* calcula a quantidade de jogos que a thread vai processar */

  /* faz o balanceamento de carga, evitando que a última thread fique com muito mais elementos para processar */
  if (thread_id < (number_of_games % number_of_threads)) {
    number_of_terms++;
  }

  first_game = thread_id * number_of_terms; /* verifica a partir de qual jogo a thread vai começar a calcular */

  /* verifica qual é o último jogo que a thread irá calcular */
  if (thread_id == number_of_threads - 1) last_game = number_of_games;
  else last_game = first_game + number_of_terms;

  /* percorre todos os jogos pelos quais a thread está responsável */
  for (i = first_game; i < last_game; i++) {
    current_game = games + i;

    /* aloca espaço pros rounds e para o resultado do jogo */
    current_game->rounds = (Round *) malloc(sizeof(Round) * game_size);
    current_game->game_result = (Result *) malloc(sizeof(Result));

    /* inicializa os resultados de cada jogador com zero */
    current_game->game_result->player1_score = 0;
    current_game->game_result->player2_score = 0;

    /* percorre cada rodada e preeche quais foram as entradas que os jogadores forneceram */
    for (j = 0; j < game_size; j++) {
      current_round = current_game->rounds + j;

      /* utilizamos uma entrada padrão para fazer os testes, altere aqui para mudar qual jogador deve ganhar */
      current_round->player1 = 5;
      current_round->player2 = 3;
    }
  }

  pthread_exit(NULL);
}

void create_games_concurrent() {
  pthread_t *array_of_threads; /* armazena um array com ponteiros para cada thread */
  int *thread_ids; /* armazena o identificador de cada thread */
  int i; /* índice do for */

  /* aloca a estrutura de dados para os jogos */
  games = (Game *) malloc(sizeof(Game) * number_of_games);
  if (games == NULL) {
    printf("ERROR -- Malloc\n");
    exit(2);
  }

  /* fizemos o preenchimento do jogo de maneira concorrente pois era a parte que mais levava tempo no programa */
  /* aloca um array com todos os ponteiros para as threads criadas */
  array_of_threads = (pthread_t *) malloc(sizeof(pthread_t) * number_of_threads);
  if (array_of_threads == NULL) {
    printf("ERROR -- Malloc\n");
    exit(2);
  }

  /* aloca um array que contem os identificadores das threads */
  thread_ids = (int *) malloc(sizeof(pthread_t) * number_of_threads);
  if (thread_ids == NULL) {
    printf("ERROR -- Malloc\n");
    exit(2);
  }

  /* cria as threads que irão executar a multiplicação */
  for (i = 0; i < number_of_threads; i++) {
    thread_ids[i] = i;

    if (pthread_create(array_of_threads + i, NULL, result_round_alloc, (void *) &thread_ids[i])) {
      printf("ERROR -- pthread_create\n");
      exit(3);
    }
  }

  /* espera as threads terminarem sua execução */
  for (i = 0; i < number_of_threads; i++) {
    pthread_join(*(array_of_threads + i), NULL);
  }

  free(array_of_threads);
  free(thread_ids);
}
