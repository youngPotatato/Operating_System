import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.channels.FileLock;

public class SummerLockingDisplay {

    private static final int NOSUMS = 100;

    public static void main(String[] args) {
        FileLock lock;
        for (int i = 0; i < NOSUMS; i++) {
            try {
                RandomAccessFile file = new RandomAccessFile(
                        SummerLockingWriter.FILENAME, "r");
                if (SummerLockingWriter.ENABLELOCK) {
                    lock = file.getChannel().lock(0, Long.MAX_VALUE, true);
                }
                int sum = 0;
                while (file.getFilePointer() < file.length()) {
                    sum += file.readInt();
                }
                if (sum != SummerLockingWriter.SUMVALUE) {
                    System.err.println("BAD SUM:  Expected "
                            + SummerLockingWriter.SUMVALUE + "; Received "
                            + sum);
                } else {
                    System.out.println("Sum checks out");
                }
                if (SummerLockingWriter.ENABLELOCK) {
                    lock.release();
                }
                file.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}