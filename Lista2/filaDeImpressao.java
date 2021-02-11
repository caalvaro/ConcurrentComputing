import java.util.LinkedList;

// Monitor
class PrinterMonitor {
  // será utilizada uma fila para gerenciar a ordem de impressão
  // quando uma thread quiser imprimir algo, ela executará a função askToPrint
  // quando a thread impressora for escalonada, ela ira imprimir um ou mais elementos da fila
  private LinkedList<Integer> queue;

  PrinterMonitor() {
    this.queue = new LinkedList<Integer>();
  }

  // As threads executam esse método para entrar na fila de impressão
  public synchronized void askToPrint(int id) {
    try {
      System.out.println("Thread "+id+" quer imprimir");

      // Se a fila estiver cheia, a thread se bloqueia
      while (this.queue.size() == Main.MaxQueueSize) {
        System.out.println("Thread "+id+" se bloqueou. Fila está cheia");
        wait();
      }

      // adiciona o pedido de impressão na fila
      this.queue.add(id);
      System.out.println("Thread "+id+" entrou na fila pra imprimir");
    } catch (Exception e) { }
  }

  // método executado pela thread impressora
  public synchronized void print() {
    try {
      // imprime os pedidos se houver algum
      if (this.queue.size() > 0) {
        Integer elementToPrint = this.queue.remove();
        System.out.println(">>> IMPRESSORA: imprimindo id da thread "+elementToPrint);
        this.notify();
      } else {
        System.out.println(">>> IMPRESSORA: nada para imprimir...");
      }
    } catch (Exception e) { }
  }

  // método executado pela thread impressora
  public synchronized void consultQueue(Integer id) {
    try {
      if (this.queue.contains(id)) {
        int index = this.queue.indexOf(id);
        System.out.println(">>> Consulta: a impressão da thread "+id+" está na fila. A posição é: "+index);
      } else {
        System.out.println(">>> Consulta: não consta na fila. A thread está em espera");
      }
    } catch (Exception e) { }
  }
}

//--------------------------------------------------------
// Classe das impressoras
class Printer extends Thread {
  PrinterMonitor monitor; //objeto monitor para coordenar a lógica de execução das threads

  // Construtor
  Printer(PrinterMonitor monitor) { this.monitor = monitor; }

  // Método executado pela thread
  public void run() {
    try {
      for (;;) {
        this.monitor.print();
        sleep(1000);
      }
    } catch (InterruptedException e) { return; }
  }
}

//--------------------------------------------------------
// Classe principal
class Main {
  static final int MaxQueueSize = 5;
  static final int NumberOfThreads = 10;

  public static void main (String[] args) {
    int i;
    PrinterMonitor monitor = new PrinterMonitor(); // Monitor (objeto compartilhado entre produtores e consumidores)
    User[] users = new User[NumberOfThreads];
    Printer printer = new Printer(monitor);
    printer.start();

    for (i = 0; i < NumberOfThreads; i++) {
      users[i] = new User(i+1, monitor);
      users[i].start();
    }
  }
}
