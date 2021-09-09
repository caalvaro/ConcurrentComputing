#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[]) {
  char file_name[50];

  // entrada do programa é o tamanho e o nome do arquivo a ser criado
  long long int resto = atoll(argv[1]);
  strcpy(file_name, argv[2]);

  FILE *file = fopen(file_name, "w");

  long long int file_size = resto * 11 + 5 * 3 + 15 + 50, i;

  long long int begin[] = {file_size};
  long long int elements[] = {1,2,3,4,5};
  long long int sequence[] = {0,1,2,3,4,5};
  long long int big_identical_sequence[] = {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3}; //15 tamanho
  long long int biggest_identical_sequence[] = {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3}; //50 tamanho

  // escreve a quantidade de números no arquivo
  fwrite(begin, sizeof(long long int), 1, file); //0

  // intercala duas sequências contíguas de números iguais com números quaisquer
  fwrite(elements, sizeof(long long int), 5, file); //1
  fwrite(big_identical_sequence, sizeof(long long int), 15, file); //6
  fwrite(elements, sizeof(long long int), 5, file); //21
  fwrite(elements, sizeof(long long int), 5, file); //26
  fwrite(biggest_identical_sequence, sizeof(long long int), 50, file); //31

  // insere uma sequência qualquer e uma sequência 012345 de acordo com a entrada do programa
  for (i = 0; i < resto; i++) {
    fwrite(elements, sizeof(long long int), 5, file);
    fwrite(sequence, sizeof(long long int), 6, file);
  }

  return 0;
}
