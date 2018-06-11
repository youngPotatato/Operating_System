import java.util.Date;
import java.util.Random;
import java.util.ArrayList;
import java.util.concurrent.Semaphore;

public class ManagerSemaphore {

	// Maximum time in between fan arrivals
	private static final int MAX_TIME_IN_BETWEEN_ARRIVALS = 3;

	// Maximum amount of break time in between celebrity photos
	private static final int MAX_BREAK_TIME = 10;

	// Maximum amount of time a fan spends in the exhibit
	private static final int MAX_EXHIBIT_TIME = 10;

	// Minimum number of fans for a photo
	private static final int MIN_FANS = 3;

	// Maximum number of fans allowed in queue
	private static final int MAX_ALLOWED_IN_QUEUE = 10;

	// Holds the queue of fans
	private static ArrayList<Fan> line = new ArrayList<Fan>();

	// The current number of fans in line
	private static int numFansInLine = 0;

	// For generating random times
    private Random rndGen = new Random(new Date().getTime());
    private Semaphore semForAcquiringFanInL = new Semaphore(10);
	private Semaphore semForListOp = new Semaphore(1);
	private Semaphore semForMaxFanBound = new Semaphore(10);

	public static void main(String[] args) {
		new ManagerSemaphore().go();

	}

	private void go() {
		// Create the celebrity thread
		Celebrity c = new Celebrity();
		System.out.println("drains "+ semForAcquiringFanInL.drainPermits()+" sems for acqFan");
		new Thread(c, "Celebrity").start();

		// Continually generate new fans
        int i = 0;
        while (true) {
			try {
				semForMaxFanBound.acquire(1);
			} catch (InterruptedException e) {
				System.err.println(e.toString());
				System.exit(1);
			}
            new Thread(new Fan(), "Fan " + i++).start();
            try {
                Thread.sleep(rndGen.nextInt(MAX_TIME_IN_BETWEEN_ARRIVALS));
            } catch (InterruptedException e) {
            	System.err.println(e.toString());
            	System.exit(1);
            }
        }

	}

	class Celebrity implements Runnable
	{
		@Override
		public void run() {
			while (true){
			    try {
					semForAcquiringFanInL.acquire(3);
				}catch (InterruptedException e) {
	            	System.err.println(e.toString());
	            	System.exit(1);
	            };
				// Check to see if celebrity flips out
				checkCelebrityOK();

				// Take picture with fans

				System.out.println("Celebrity takes a picture with fans");

				// Remove the fans from the line
				for (int i = 0; i < MIN_FANS; i++) {
					semForListOp.acquireUninterruptibly();
					System.out.println(line.remove(0).getName() + ": OMG! Thank you!");
					semForListOp.release();
				}

				// Adjust the numFans variable
				semForListOp.acquireUninterruptibly();
				numFansInLine-= MIN_FANS;
				semForListOp.release();
				semForMaxFanBound.release(3);

				// Take a break
                		try {
                		    //the time in sleep() is the minimum time this thread will not run
							//not equal to the real time between sleep start and resume
                		    Thread.sleep(rndGen
                		            .nextInt(MAX_BREAK_TIME));
                		} catch (InterruptedException e) {
                			System.err.println(e.toString());
                			System.exit(1);
                		}
			}

		}

	}

	public void checkCelebrityOK()
	{
		if (numFansInLine > MAX_ALLOWED_IN_QUEUE)
		{
			System.err.println("Celebrity becomes claustrophobic and flips out");
			System.exit(1);
		}

		if (numFansInLine < MIN_FANS)
		{
			System.err.println("Celebrity becomes enraged that he was woken from nap for too few fans");
			System.exit(1);
		}
	}

	class Fan implements Runnable
	{
		String name;

		public String getName()
		{ return name;}

		@Override
		public void run() {
			// Set the thread name
			name = Thread.currentThread().toString();

            System.out.println(Thread.currentThread() + ": arrives");

			// Look in the exhibit for a little while
	          try {
	                Thread.sleep(rndGen.nextInt(MAX_EXHIBIT_TIME));
	            } catch (InterruptedException e) {
	            	System.err.println(e.toString());
	            	System.exit(1);
	            }

	          // Get in line
              try {
				  System.out.println(Thread.currentThread() + ": gets in line");
				  semForListOp.acquire();
				  line.add(0, this);
				  numFansInLine++;
				  semForAcquiringFanInL.release(1);
				  System.out.println("after " + this.getName() + "get in line " +
						  semForAcquiringFanInL.availablePermits() + " semForFan available");
				  semForListOp.release();
			  }catch(InterruptedException e){
				  System.err.println(e.toString());
				  System.exit(1);
			  }

		}

	}
}
