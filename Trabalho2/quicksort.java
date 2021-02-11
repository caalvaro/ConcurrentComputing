import java.util.LinkedList;
import java.util.Random;

// classe para encapsular o intervalo
class Range {
  public int fromIndex, toIndex;

  Range(int fromIndex, int toIndex) {
    this.fromIndex = fromIndex;
    this.toIndex = toIndex;
  }
}

// Monitor
class Monitor {
  // fila de intervalos que as threads irão processar
  // as threads irão concorrer para acessar esses intervalos e processá-los em paralelo
  // a cada recursão do quicksort, serão adicionados e removidos intervalos nessa Fila
  // ela funcionará como uma bolsa de tarefas
  // onde as threads frequentemente irão consultar para pegar os intervalos do array
  private LinkedList<Range> queue;
  private int finishedThreads = 0; // indica a quantidade de threads que já terminaram sua execução

  // construtor
  Monitor() {
    this.queue = new LinkedList<Range>();
    // inicia a fila com um intervao que abrange todo o array
    this.queue.add(new Range(0, Main.ArraySize));
  }

  // algoritmo de ordenação
  public void quicksort(int[] array) {
    // pega algum intervalo disponível para processar
    Range range = this.getRange();

    // condição de parada
    if (range.fromIndex >= range.toIndex) {
      return;
    }

    // faz as alterações no array para ordenar e retorna uma nova partição
    int partition = partition(array, range);

    // adiciona novos intervalos para serem processados
    this.addRanges(range.fromIndex, partition, range.toIndex);

    // chama recursivamente o método para continuar a execução do algoritmo
    this.quicksort(array);
    this.quicksort(array);

    // sinaliza que a thread terminou sua execução
    this.finishThread();
  }

  // remove um intervalo da fila para ser processado
  public synchronized Range getRange() {
    try {
      // a thread se bloquea quando a fila está vazia
      // a não ser que essa thread seja a última ainda executando, nesse caso ela não se bloqueia para evitar deadlock
      while (this.queue.size() == 0 && this.finishedThreads < Main.NumberOfThreads - 1) {
        if (Main.debugMode) {
          System.out.println("Thread "+Thread.currentThread().getId()+" se bloqueou");
        }
        this.wait();
      }
    } catch (Exception e) { e.printStackTrace(); }

    // se a fila está vazia e a thread não se bloqueou
    // retorna um intervalo qualquer que seja condição de parada do algoritmo
    if (this.queue.size() == 0) {
      return new Range(-1,-1);
    }

    return this.queue.remove();
  }

  // adiciona dois novos intervalos na fila para serem processados pelas threads
  public synchronized void addRanges(int fromIndex, int partition, int end) {
    this.queue.add(new Range(fromIndex, partition));
    this.queue.add(new Range(partition + 1, end));

    // notifica as threads que estão esperando que já tem novos intervalos para serem processados
    this.notifyAll();

    if (Main.debugMode) {
      this.printQueue();
      System.out.println("Thread "+Thread.currentThread().getId()+" notificou todas");
    }
  }

  // função para fazer registrar que uma thread terminou a execução
  public synchronized void finishThread() {
    this.finishedThreads++;
    this.notifyAll();
  }

  // faz a ordenação do array e retorna uma nova partição
  public int partition(int[] array, Range range) {
    int fromIndex = range.fromIndex, toIndex = range.toIndex;
    // escolhe o pivot como o último elemento do array
    int pivot = array[toIndex-1];

    for (int i = range.fromIndex; i < range.toIndex; i++) {
      if (array[i] > pivot) {
        toIndex += 1;
      } else {
        toIndex += 1;
        fromIndex += 1;
        swap(array, i, fromIndex - 1);
      }
    }

    return fromIndex - 1;
  }

  // faz a troca de elementos nas posições do array
  public void swap(int[] array, int index1, int index2) {
    int aux = array[index1];
    array[index1] = array[index2];
    array[index2] = aux;
  }

  // função para imprimir a fila com os intervalos a serem processados no quicksort
  public synchronized void printQueue() {
    System.out.print("> Fila (Thread "+Thread.currentThread().getId()+"): ");
    for (int i = 0; i < this.queue.size(); i++) {
      System.out.print("("+this.queue.get(i).fromIndex+", "+this.queue.get(i).toIndex+") ");
    }
    System.out.println();
  }
}

//----------------------------------------------------------
// Classe das threads que executarão as tarefas do quicksort
class Worker extends Thread {
  Monitor monitor; //objeto monitor para coordenar a lógica de execução e sincronização das threads
  int[] array;

  // Construtor
  Worker(Monitor monitor, int[] array) {
    this.monitor = monitor;
    this.array = array;
  }

  // Método executado pela thread
  public void run() {
    if (Main.debugMode) {
      System.out.println("Thread "+this.getId()+" começou a executar");
    }
    monitor.quicksort(array);
  }
}

//--------------------------------------------------------
// Classe principal
class Main {
  // variáveis globais para personalizar a execução do programa
  static final int NumberOfThreads = 4;
  static final int ArraySize = 20;
  static boolean debugMode;

  public static void main (String[] args) {
    int i;
    Monitor monitor = new Monitor();
    Random random = new Random(15158);
    int[] array = new int[ArraySize]; // array a ser ordenado
    Worker[] workers = new Worker[NumberOfThreads]; // array de threads
    boolean sortError = false;

    // configura um modo com prints caso o usuário passe algum argumento para a main
    if (args.length > 0) {
      debugMode = true;
    }

    // cria um array com elementos aleatórios de 1 a 100
    for (i = 0; i < ArraySize; i++) {
       array[i] = random.nextInt() % 100;
    }

    // imprime o array criado
    System.out.print("\nArray inicial: ");
    for (i = 0; i < ArraySize; i++) {
      System.out.print(array[i]+" ");
    }
    System.out.println();

    for (i = 0; i < NumberOfThreads; i++) {
      workers[i] = new Worker(monitor, array);
      workers[i].start();
    }

    // aguarda a finalização das threads
    try {
      for (i = 0; i < NumberOfThreads; i++) {
        long id = workers[i].getId();
        workers[i].join();

        if (Main.debugMode) {
          System.out.println("\nThread "+id+" finalizou");
        }
      }
    } catch (Exception e) {e.printStackTrace();}

    // imprime o array ordenado
    System.out.print("\nArray ordenado: ");
    for (i = 0; i < ArraySize; i++) {
      System.out.print(array[i]+" ");
    }
    System.out.println();

    // verifica se houve algum erro na ordenação
    for (i = 1; i < ArraySize; i++) {
      if (array[i-1] > array[i]) {
        sortError = true;
        System.out.println(">>> Ordenação com ERRO!!! em "+i);
      }
    }

    if (!sortError) {
      System.out.println("\nArray corretamente ordenado!");
    }
  }
}
