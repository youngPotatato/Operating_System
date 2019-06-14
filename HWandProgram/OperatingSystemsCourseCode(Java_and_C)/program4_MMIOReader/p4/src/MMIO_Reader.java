import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteOrder;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.charset.StandardCharsets;
import java.util.Date;
import java.util.Random;
import java.util.concurrent.Semaphore;

public class MMIO_Reader {

    private static String FILE_NAME;
    private Random rdG = new Random(new Date().getTime());
    private RandomAccessFile rf;
    private MappedByteBuffer mBB;
    private static long fileSize;
    private static final int READER_RUNNING_ROUND= 1000;
    //private static final int READER_RUNNING_ROUND= 10000;
    private static final int SLEEP_TIME = 20;
    //number of reader/writer
    private static final int READER_WRITER_NUMBER = 2;
    //for random number gen object
    private Semaphore semForRndGen = new Semaphore(1);

    private int rdCount = 0, wrCount = 0;
    //semaphore for writers preference, including
    //reader mutex, writer mutex, reader entry mutex, resourse mutex;
    private Semaphore rMutex = new Semaphore(1), wMutex = new Semaphore(1),
                      readTry = new Semaphore(1), resourseSemaphore = new Semaphore(1);

    private class Node{
       int index;
       int leftChildIndex;
       int rightChildIndex;
       char[] value;
       public Node(int index, int leftChildIndex, int rightChildIndex, char[] value){
            this.index = index;
            this.leftChildIndex = leftChildIndex;
            this.rightChildIndex = rightChildIndex;
            this.value = value;
       }
       public Node(int index){
            int basePointer = setFPLocation(index);
            this.index= mBB.getInt() ;
            this.value = new char[2];
            value[0] = (char)(mBB.get()&0xff);
            value[1] = (char)(mBB.get()&0xff);
            mBB.position(basePointer + 8);
            this.leftChildIndex = mBB.getInt();
            this.rightChildIndex = mBB.getInt();
       }
       @Override
       public String toString(){
           return String.format("Node index %d, leftChildIndex %d, rightChildIndex %d, value[%c,%c]\n",this.index,this.leftChildIndex,this.rightChildIndex,this.value[0],this.value[1]);
       }
    }
    public MMIO_Reader(String fn){
        this.FILE_NAME = "yy1.txt";
        try {
            this.rf = new RandomAccessFile(FILE_NAME,"rw");
            this.fileSize = rf.length();
            this.mBB =  rf.getChannel().map(FileChannel.MapMode.READ_WRITE,0,fileSize);
            mBB.order(ByteOrder.LITTLE_ENDIAN);
        }catch(IOException ex){
           ex.printStackTrace();
           System.exit(1);
        }
    }

    // TODO: 4/14/2018 rf.close();
    // TODO:           need??mBB.close??

    private Node getNode(int index){
       int nodeTotalNumber = (int)getFileSize()>>4;
       if(index >= nodeTotalNumber) {
           System.err.println(String.format("invalid index, need to be smaller than %d, but %d provided", nodeTotalNumber, index));
           System.exit(1);
       }
        return new Node(index);
    }
    protected int setFPLocation(int nodeIndex){
        mBB.position(nodeIndex<<4);
        return nodeIndex<<4;
    }
    protected long getFileSize(){
        return fileSize;
    }

    protected byte[] genRdValue(){
       semForRndGen.acquireUninterruptibly(1);
       byte[] result =  new StringBuilder().append((char)(rdG.nextInt(26)+'A')).append((char)(rdG.nextInt(26)+'A')).toString().getBytes(StandardCharsets.US_ASCII);
       semForRndGen.release();
       return result;

    }
    protected int getRandIndex(){
        semForRndGen.acquireUninterruptibly(1);
        int result =  rdG.nextInt((int)(getFileSize()>>4));
        semForRndGen.release();
        return result;
    }
    private void go() {
        int i = 0;
        while(i++<READER_WRITER_NUMBER){
            new Thread(new reader(),"reader" + i).start();
            new Thread(new writer(),"writer" + i).start();
        }
        try {
            this.rf.close();
        }catch(IOException e){
            e.printStackTrace();
        }
    }

    private class reader implements Runnable{
        private void printFromIndex(int index){
            int nodeTotalNumber = (int)getFileSize()>>4;
            int lIndex = 2*(index + 1)-1;
            //print if there is right child
            if(2*(index+1)<nodeTotalNumber) {
                readTry.acquireUninterruptibly(1);
                rMutex.acquireUninterruptibly(1);
                rdCount++;
                if (rdCount == 1)
                    resourseSemaphore.acquireUninterruptibly(1);
                rMutex.release();
                readTry.release();
                System.out.println("READER: " + getNode(2 * (index + 1)).toString());
                rMutex.acquireUninterruptibly(1);
                rdCount--;
                if(rdCount == 0)
                    resourseSemaphore.release();
                rMutex.release();
            }
            //print all its left children
            while(lIndex<nodeTotalNumber){
                readTry.acquireUninterruptibly(1);
                rMutex.acquireUninterruptibly(1);
                rdCount++;
                if (rdCount == 1)
                    resourseSemaphore.acquireUninterruptibly(1);
                rMutex.release();
                readTry.release();
                System.out.println("READER: " + getNode(lIndex).toString());
                lIndex = 2*(lIndex+1) -1;
                rMutex.acquireUninterruptibly(1);
                rdCount--;
                if(rdCount == 0)
                    resourseSemaphore.release();
                rMutex.release();
            }
        }
        @Override
        public void run(){
            //TODO: run several round of (print + sleep)
            int i = READER_RUNNING_ROUND;
            while(i-->0){
               printFromIndex(getRandIndex());
               try {
                   Thread.sleep(SLEEP_TIME);
               } catch(InterruptedException e){
                  System.err.println(e.toString());
                  System.exit(1);
               }
            }
        }

    }
    private class writer implements Runnable{
        private void modifyOneNodeVal(byte[] newVal, int nodeIndex){
            mBB.position((nodeIndex<<4) + 4);
            mBB.put(newVal);
            System.out.println("WRITER: modified node#"+nodeIndex);
            System.out.println("        " + getNode(nodeIndex).toString());
        }

        @Override
        public void run(){
            int i = READER_RUNNING_ROUND;
            while(i-->0){
                wMutex.acquireUninterruptibly(1);
                wrCount++;
                if(wrCount == 1)
                    readTry.acquireUninterruptibly(1);
                wMutex.release();
                resourseSemaphore.acquireUninterruptibly(1);
                modifyOneNodeVal(genRdValue(),getRandIndex());
                resourseSemaphore.release();
                wMutex.acquireUninterruptibly(1);
                wrCount--;
                if(wrCount == 0)
                    readTry.release();
                wMutex.release();
                try {
                    Thread.sleep(SLEEP_TIME);
                } catch(InterruptedException e){
                    System.err.println(e.toString());
                    System.exit(1);
                }
            }
        }
    }
    public static void main(String[] args){
        //todo : take args file name
        //todo : cite reference wiki
        new MMIO_Reader("").go();
        //MMIO_Reader yy = new MMIO_Reader("");
        ////yy.new reader().printFromIndex(0);
        //yy.new reader().run();
        ////System.out.println(yy.getNode(1).toString());
        ////yy.new writer().modifyOneNodeVal(yy.genRdValue(),1);
        //yy.new writer().run();
        ////System.out.println(yy.getNode(1).toString());
        //try {
        //    yy.rf.close();
        //}catch(IOException e){
        //    e.printStackTrace();
        //}
    }
}
