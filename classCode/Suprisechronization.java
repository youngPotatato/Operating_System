public class Suprisechronization {

    private final static int NOTHREADS = 10;
    private final static long NOINCREMENTS = 10000000;
    
    private static long ct = 0;
    
    public static void main(String args[]) throws InterruptedException {
        
        Thread threadList[] = new Thread[NOTHREADS];
        for (int i=0; i < NOTHREADS; i++) {
            threadList[i] = new Thread(new Runnable() {
                @Override
                public void run() {
                    Thread.yield();
                    for (int i=0; i < NOINCREMENTS; i++) {                          
                      ct++;
                    }
                }
            });
            threadList[i].start();
        }
        
        for (Thread t : threadList) {
            t.join();
        }
        
        System.out.println("Count is " + ct + "; expected " + NOINCREMENTS * NOTHREADS);
    }
}

/**
 * Solutions:
 * 1.  Synchronize increment
 * 2.  Use AtomicLong
 **/