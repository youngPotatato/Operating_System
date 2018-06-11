import java.util.LinkedList;
import java.util.Queue;

/**
 * Demonstration of queue's lack of thread safety.
 */
public class QueueBan {

    private static final int NOELMTS = 20;
    private static final int NODEQS = 100000;
    private static final int NOTHREADS = 10;

    // Use java.util.concurrent.ConcurrentLinkedQueue for thread safety
    private static Queue<Integer> q = new LinkedList<Integer>();
    
    public static void main(String args[]) throws InterruptedException {
        // Load queue
        for (int i=0; i < NOELMTS; i++) {
            q.add(i);
        }
        
        // Start threads
        Thread threadList[] = new Thread[NOTHREADS];
        for (int i=0; i < NOTHREADS; i++) {
            threadList[i] = queueToy(q);
        }
        
        for (Thread t : threadList) {
            t.join();
        }
        
        for (int i=0; i < NOELMTS; i++) {
            if (!q.contains(i)) {
                System.out.println("Where is " + i);
            }
        }
    }
    
    public static Thread queueToy(final Queue<Integer> q) {
        Thread t = new Thread(new Runnable() {
            @Override
            public void run() {
                for (int i=0; i < NODEQS; i++) {
                    int val = q.remove();
                    q.add(val);
                }
            }
        });
        t.start();
        
        return t;
    }
}