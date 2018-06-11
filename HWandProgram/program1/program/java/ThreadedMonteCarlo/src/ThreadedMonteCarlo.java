import java.util.Random;
import java.io.BufferedWriter;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.FileNotFoundException;
import java.io.IOException;

public class ThreadedMonteCarlo implements Runnable{
    private static String SOURCE_FILE;
    private int total = 0;
    private int inCircle = 0;
    private int num;
    BufferedReader reader;

    public ThreadedMonteCarlo(String file){
        try {
            SOURCE_FILE = file;
            reader = new BufferedReader(new FileReader(file));
            total = 0;
            inCircle = 0;
        }catch(FileNotFoundException ex){
            ex.printStackTrace();
        }
    }
    private class point{
        float x;
        float y;
        public void setCood(String[] cood){
            x = Float.valueOf(cood[0]);
            y = Float.valueOf(cood[1]);
        }

    }
    private void creatFile(int num) {
        this.num = num;
        try {
            BufferedWriter os = new BufferedWriter(new FileWriter(SOURCE_FILE));
            os.write(num+"\n");//total number of points
            Random r = new Random();
            while(num-->0){
                float x = -1 + r.nextFloat()*2;
                float y = -1 + r.nextFloat()*2;
                os.write(x+","+y+"\n");
                os.flush();
            }
            os.close();
        }catch(IOException ex){
            ex.printStackTrace();
        }

    }
    private synchronized void incIcircle(){
        ++inCircle;
    }
    private synchronized void incTotal(){
        ++total;
    }
    private int getTotal(){
        return total;
    }
    //read points info
    private boolean readOneline(BufferedReader rder,point m){
			String line = null;
		    try{
			    if((line = rder.readLine()) != null) {
                    m.setCood(line.trim().split(",\\s*"));
                }else{
			        return false;
                }
		    }catch(IOException ex) {
		        ex.printStackTrace();
            }
            incTotal();
		    return true;
    }
    private boolean isInCircle(point m){
        return (Math.pow(m.x,2)+Math.pow(m.y,2) <= 1);
    }
    private synchronized boolean proOneLine(point p){
        return    readOneline(this.reader, p);
    }
    public void run(){
        point p = new point();
        while(proOneLine(p)) {
            if(isInCircle(p)){
                incIcircle();
            }
        }
        //System.out.println(Thread.currentThread().getName()+ " finished"+ " total num is "+ this.getTotal()+" inC is " + this.inCircle);
    }
    public void passFirstLine(){
        String line = null;
        try {
            if((line = this.reader.readLine()) != null) {
                this.num =  Integer.parseInt(line);
            }
        }catch(IOException ex){
            ex.printStackTrace();
        }
    }
    public static void main(String [] args){
        //constructing object and read fileName
        ThreadedMonteCarlo myT = new ThreadedMonteCarlo(args[0]);
        //read number of points
        myT.passFirstLine();
        //create threads
        if(Integer.valueOf(args[1])<=0){
            System.err.println("invalid process number");
            return;
        }
        Thread th[] = new Thread[Integer.valueOf(args[1])];
        for(int i = 0; i < th.length;i++) {
            th[i] = new Thread(myT);
            //set name, only used for debugging
            th[i].setName("th"+i);
        }
        try {
            for(Thread temp:th) {
                temp.start();
            }
            for (Thread temp : th) {
                temp.join();
            }
        }catch(InterruptedException ex){
           System.err.println("Interupted!");
        }
        //check if number of points matches
        if(myT.getTotal() != myT.num){
            System.err.println("number of points doesn't match");
            return;
        }
        //display result
        System.out.print("Calc finished! Total num of points is " + myT.getTotal() + " .");
        System.out.println("Num of in-circle points is " + myT.inCircle);
        System.out.println("Estimated PI value is " + (4* (float)myT.inCircle/(float)myT.num));
    }
}
