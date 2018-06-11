public class ThreadEx {

	public static void main(String[] args) throws InterruptedException {

		if (args.length < 1) {
			System.err.println("Parameter(s): <greeting> [<greeting>...]");
			System.exit(1);
		}

		Thread threadList[] = new Thread[args.length];
		for (int i=0; i < args.length; i++) {
			threadList[i] = new Thread(new Greeter(args[i]));
			threadList[i].start();
		}
		
		for (Thread t : threadList) {
			t.join();
		}
		
		System.out.println("Done!");
	}
}

class Greeter implements Runnable {
	private String greeting; // Message to print to console

	public Greeter(String greeting) {
		this.greeting = greeting;
	}

	public void run() {
		for (int i = 0; i < 1000; i++) {
			System.out.println(Thread.currentThread().getName() + ":  "
					+ greeting);
		}
	}
}
