#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

//variaveis globais/compartilhadas
//numero de threads e tamanho do vetor
int nthreads, tam;
//vetor de elementos
int *vet;
//vetor de resultados
int *resultados;
//funcao executada pelas threads
void *negativos (void *tid) {
int id = * (int *) tid;
//intervalo de elementos processados por cada thread
int inicio, fim, j;
int tam_bloco = tam/nthreads;
//variavel local para a qtde de negativos encontrados pela thread
int qtde=0;
//calcula o intervalo de elementos da thread
inicio = id * tam_bloco;
//o ultimo fluxo trata os elementos restantes
if (id<nthreads-1) fim = inicio + tam_bloco;
else fim = tam;
for(j=inicio; j<fim; j++) {
if(vet[j] < 0) qtde++;
}
//carrega o resultado parcial para o vetor de resultados
resultados[id] = qtde;
free(tid);
pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
pthread_t *tid_sistema; //vetor identificadores das threads no sistema
int *tid; //identificadores das threads no programa
int t; //variavel contadora
int qtde_negativos=0; //contabiliza a qtde total de numeros negativos encontrados
tam = atoi(argv[1]); //tamanho do vetor
nthreads = atoi(argv[2]); //numero de threads
//limita o numero de threads ao tamanho do vetor
if(nthreads>tam) nthreads = tam;
//aloca espaco para o vetor de identificadores das threads no sistema
tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
if(tid_sistema==NULL) {
printf("--ERRO: malloc()\n"); exit(-1);
}
//aloca espaco para o vetor de resultados
resultados = (int *) malloc(sizeof(int) * nthreads);
if(resultados==NULL) {
printf("--ERRO: malloc()\n"); exit(-1);
}

//Parte 1
vet = (int *) malloc(sizeof(int) * tam);
for (t = 0; t < tam; t++) {
  if (t % 2 == 0) {
    vet[t] = -1;
  } else {
    vet[t] = 1;
  }

}
//Parte 2: contabiliza a qtde de negativos no vetor
//cria as threads
for(t=0; t<nthreads; t++) {
tid = malloc(sizeof(int));
if(tid==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }
*tid = t;
if (pthread_create(&tid_sistema[t], NULL, negativos, (void*) tid)) {
printf("--ERRO: pthread_create()\n"); exit(-1);
}
}
//espera todas as threads terminarem e calcula o valor de saida
for(t=0; t<nthreads; t++) {
if (pthread_join(tid_sistema[t], NULL)) {
printf("--ERRO: pthread_join()\n"); exit(-1);
}
//atualiza o valor de saida
qtde_negativos += resultados[t];
}
//Parte 3
printf("\n\n RESULTADO %d negativos\n", qtde_negativos);
// for (t = 0; t < tam; t++) {
//   printf("%d  ", vet[t]);
// }
}
