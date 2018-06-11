import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.channels.FileLock;
import java.util.Random;

public class SummerLockingWriter {

    public static final String FILENAME = "data.txt";
    public static final int SUMVALUE = 1000;
    public static final boolean ENABLELOCK = false;

    public static void main(String[] args) throws IOException {
        Random randNoGen = new Random();
        FileLock lock;
        while (true) {
            RandomAccessFile file = new RandomAccessFile(FILENAME, "rw");
            if (ENABLELOCK) {
                lock = file.getChannel().lock();
            }
            int left = SUMVALUE;
            while (left > 0) {
                int dec = randNoGen.nextInt(Math.min(10, left + 1));
                file.writeInt(dec);
                left -= dec;
            }
            file.setLength(file.getFilePointer()); // Truncate file
            if (ENABLELOCK) {
                lock.release();
            }
            file.close();
        }
    }
}
