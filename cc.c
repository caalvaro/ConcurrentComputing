int x = 10;
int queroEntrar_0 = 0, queroEntrar_1 = 0, turn;

int main() {
  queroEntrar_1 = 1;
  turn = 0;

  while (queroEntrar_0 && turn == 0) {}

  x--;
  x++;

  if (x != 10) {
    printf("x = %d\n", x);
  }

  queroEntrar_1 = 0;
  turn = 1;

  return 0;
}
