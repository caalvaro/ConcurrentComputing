/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Leitores e escritores usando monitores em Java */
/* -------------------------------------------------------------------*/

//classe da estrutura de dados (recurso) compartilhado entre as threads
class Buffer {
  private Integer buffer[];
  private int bufferSize;

  //construtor
  Buffer() {
    this.buffer = new Integer[Main.sharedAreaSize];
    this.bufferSize = 0; // quantidade de elementos não nulos

    for (int i = 0; i < buffer.length; i++) {
      this.buffer[i] = null; // inicia um buffer vazio
    }
  }

  //operacao de leitura sobre o recurso compartilhado
  public synchronized void printBuffer() {
    System.out.print("[ ");
    for (int i = 0; i < buffer.length - 1; i++) {
      if (this.buffer[i] == null) {
        System.out.print("--, ");
      } else {
        System.out.print(buffer[i] + ", ");
      }
    }
    if (this.buffer[buffer.length - 1] == null) {
      System.out.println(" -- ]");
    } else {
      System.out.println(buffer[buffer.length - 1] + " ]");
    }
  }

  //operacao de leitura sobre o recurso compartilhado
  public int getBufferSize() {
    return this.bufferSize;
  }

  //operacao de leitura sobre o recurso compartilhado
  public synchronized Integer getElement(int index) {
    return this.buffer[index];
  }

  //operacao de escrita sobre o recurso compartilhado
  public synchronized Integer insertElement(int index, int value) {
    // a função só insere um elemento na posição se ela estiver vazia
    if (this.buffer[index] == null) {
      this.buffer[index] = value;
      this.bufferSize++;
    }

    return this.buffer[index];
  }

  //operacao de escrita sobre o recurso compartilhado
  public synchronized Integer removeElement(int index) {
    Integer removedElement = null;

    // se a posição estiver vazia, não remove o elemento e retorna nulo
    if (this.buffer[index] != null) {
      removedElement = this.buffer[index];
      this.buffer[index] = null;
      this.bufferSize--;
    }

    return removedElement;
  }
}

// Monitor
class ProducerConsumer {
  private int in, out;
  private Buffer buffer;

  // Construtor
  ProducerConsumer(Buffer buffer) {
    this.in = 0; // posição da próxima produção
    this.out = 0; // posição do próximo consumo
    this.buffer = buffer;
  }

  // O consumidor consome o buffer
  public synchronized void consume(int id) {
    try {
      System.out.println("Consumidor "+id+" quer consumir");

      // se o buffer não tiver elementos para serem consumidos, a thread se bloqueia
      while (this.buffer.getBufferSize() == 0) {
        System.out.println ("Consumidor "+id+" se bloqueou. Buffer está vazio");
        wait();  //bloqueia pela condicao logica da aplicacao
      }

      this.buffer.removeElement(this.out);

      this.buffer.printBuffer();

      System.out.println("Consumidor "+id+" consumiu na posição "+this.out);

      this.out = (this.out + 1) % Main.sharedAreaSize;

      this.notify();
    } catch (InterruptedException e) { }
  }

  // Produtor produz elementos
  public synchronized void produce(int id) {
    try {
      System.out.println("Produtor "+id+" quer produzir");

      // se o buffer estiver cheio, a thread se bloqueia
      while(this.buffer.getBufferSize() == Main.sharedAreaSize) {
        System.out.println("Produtor "+id+" se bloqueou. Buffer está cheio");
        wait();  //bloqueia pela condicao logica da aplicacao
      }

      this.buffer.insertElement(this.in, id);

      this.buffer.printBuffer();

      System.out.println("Produtor "+id+" terminou de produzir na posição "+this.in);

      this.in = (this.in + 1) % Main.sharedAreaSize;

      this.notify();
    } catch (InterruptedException e) { }
  }
}

//--------------------------------------------------------
// Consumidor
class Consumer extends Thread {
  int id; //identificador da thread
  int delay; //atraso bobo
  ProducerConsumer monitor; //objeto monitor para coordenar a lógica de execução das threads

  // Construtor
  Consumer(int id, int delayTime, ProducerConsumer monitor) {
    this.id = id;
    this.delay = delayTime;
    this.monitor = monitor;
  }

  // Método executado pela thread
  public void run() {
    try {
      for (;;) {
        this.monitor.consume(this.id);
        sleep(this.delay);
      }
    } catch (InterruptedException e) { return; }
  }
}

//--------------------------------------------------------
// Produtor
class Producer extends Thread {
  int id; //identificador da thread
  int delay; //atraso bobo...
  ProducerConsumer monitor; //objeto monitor para coordenar a lógica de execução das threads

  // Construtor
  Producer(int id, int delayTime, ProducerConsumer monitor) {
    this.id = id;
    this.delay = delayTime;
    this.monitor = monitor;
  }

  // Método executado pela thread
  public void run() {
    try {
      for (;;) {
        this.monitor.produce(this.id);
        sleep(this.delay); //atraso bobo...
      }
    } catch (InterruptedException e) { return; }
  }
}

//--------------------------------------------------------
// Classe principal
class Main {
  static final int NumberOfConsumers = 2;
  static final int NumberOfProducers = 5;
  static final int sharedAreaSize = 5;

  public static void main (String[] args) {
    int i;
    Buffer buffer = new Buffer();
    ProducerConsumer monitor = new ProducerConsumer(buffer); // Monitor (objeto compartilhado entre produtores e consumidores)
    Consumer[] consumer = new Consumer[NumberOfConsumers];   // Threads consumidores
    Producer[] producer = new Producer[NumberOfProducers];   // Threads produtores

    for (i = 0; i < NumberOfConsumers; i++) {
      consumer[i] = new Consumer(i+1, (i+1)*500, monitor);
      consumer[i].start();
    }
    for (i = 0; i < NumberOfProducers; i++) {
      producer[i] = new Producer(i+1, (i+1)*500, monitor);
      producer[i].start();
    }
  }
}
