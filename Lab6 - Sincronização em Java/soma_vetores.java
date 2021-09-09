/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Álvaro de Carvalho */
/* Laboratório: 6 */
/* Codigo: Somando dois vetores em um programa multithreading Java */
/* -------------------------------------------------------------------*/

//classe da estrutura de dados (recurso) compartilhado entre as threads
class Vector {
  private int vector[];

  //construtor
  public Vector(int initialValue) {
    this.vector = new int[Main.vectorSize];

    for (int i = 0; i < vector.length; i++) {
      this.vector[i] = initialValue;
    }
  }

  //operacao de leitura sobre o recurso compartilhado
  public synchronized void printVector() {
    for (int i = 0; i < vector.length; i++) {
      System.out.print(vector[i] + " ");
    }
    System.out.println("");
  }

  //operacao de leitura sobre o recurso compartilhado
  public synchronized int[] getVector() {
    return this.vector;
  }

  //operacao de leitura sobre o recurso compartilhado
  public int getVectorSize() {
    return this.vector.length;
  }

  //operacao de leitura sobre o recurso compartilhado
  public synchronized int getElement(int index) {
    return this.vector[index];
  }

  //operacao de escrita sobre o recurso compartilhado
  public synchronized int setElement(int index, int value) {
    this.vector[index] = value;
    return this.vector[index];
  }
}

//classe que estende Thread e implementa a tarefa de cada thread do programa
class VectorSumThread extends Thread {
  //identificador da thread
  private int id;

  private int initialVectorIndex;
  private int finalVectorIndex;

  //objeto compartilhado com outras threads
  private static Vector vectorA;
  private static Vector vectorB;
  private static Vector vectorSum = new Vector(0);

  //construtor
  public VectorSumThread(int tid, Vector vectorA, Vector vectorB) {
    this.id = tid;

    this.vectorA = vectorA;
    this.vectorB = vectorB;

    // calcula a quantidade de elementos do vetor que a thread irá acessar para somar
    int numberOfTerms = Main.vectorSize / Main.numberOfThreads;

    // faz o balanceamento de carga, evitando que a última thread fique com muito mais elementos que as outras
    if (this.id < (Main.vectorSize % Main.numberOfThreads)) {
      numberOfTerms++;
    }

    // calcula o primeiro indíce que a thread irá calcular
    this.initialVectorIndex = this.id * numberOfTerms;
    /* verifica qual é o último indice que a thread irá calcular */
    if (this.id == Main.numberOfThreads - 1) this.finalVectorIndex = Main.vectorSize;
    else this.finalVectorIndex = this.initialVectorIndex + numberOfTerms;
  }

  //metodo main da thread
  public void run() {
    System.out.println("Thread " + this.id + " iniciou!");

    // percorre os vetores somente no trecho pelo qual a thread é responsável
    for (int i = this.initialVectorIndex; i < this.finalVectorIndex; i++) {
      int sumAB = this.vectorA.getElement(i) + this.vectorB.getElement(i);
      this.vectorSum.setElement(i, sumAB);
    }

    System.out.println("Thread " + this.id + " terminou!");
  }

  // retorna a soma dos vetores
  public static Vector getVectorSum() {
    return vectorSum;
  }
}

//classe da aplicacao
class Main {
  // atributos estáticos para definir número de threads e o tamanho dos vetores que serão somados
  static final int numberOfThreads = 4;
  static final int vectorSize = 23;

  public static void main (String[] args) {
    //reserva espaço para um vetor de threads
    Thread[] threads = new Thread[numberOfThreads];

    //cria duas instancia do recurso compartilhado entre as threads
    Vector vectorA = new Vector(18);
    Vector vectorB = new Vector(13);

    //cria as threads da aplicacao
    for (int i = 0; i < threads.length; i++) {
      threads[i] = new VectorSumThread(i, vectorA, vectorB);
    }

    //inicia as threads
    for (int i = 0; i < threads.length; i++) {
      threads[i].start();
    }

    //espera pelo termino de todas as threads
    for (int i = 0; i < threads.length; i++) {
      try { threads[i].join(); } catch (InterruptedException e) { return; }
    }

    // imprime a soma dos vetores
    VectorSumThread.getVectorSum().printVector();
  }
}
