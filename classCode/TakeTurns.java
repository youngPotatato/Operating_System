public class TakeTurns {

    private static int value;
    private static boolean loaded = false;

    public static void main(String[] args) {
        takeTurn(0);
        takeTurn(1);

        value = 50;
        loaded = true;
    }

    private static void takeTurn(final int remainder) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                while (!loaded);
                while (true) {
                    while (value % 2 != remainder);
                    System.out.println(remainder + " turn: " + value++);
                }
            }
        }).start();
    }
}
