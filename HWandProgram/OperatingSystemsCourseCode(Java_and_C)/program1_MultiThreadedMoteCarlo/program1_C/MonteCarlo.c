#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#define READ_END	0
#define WRITE_END	1
#define TRUE 1
#define FALSE 0

typedef struct point{
    float x;
    float y;
} point;

float sq(float x){
    return x*x;
}

void calcInCirPoints(int i,struct point *pp, int *cnt) {
    for (; --i >= 0;) {
        if (sq(pp[i].x) + sq(pp[i].y) <= 1.0)
            (*cnt)++;
    }
}
void createMultiPro(int n,int **fd, int **fdc){
    pid_t id;
    while(--n>0){
        id = fork();
        if(id<0){
            fprintf(stderr, "Fork failed");
            exit(1);
        }
        if(id>0){
            *fd = ((*fd) + 4);
            *fdc = ((*fdc) + 2);
        }else if(id == 0){
            break;
        }
    }
}
int createPipe(int numOfProcess,int *fd, int *fdC){
    while(numOfProcess-->0){
        if(pipe(fd) == -1 || pipe(fd+2)==-1 || pipe(fdC) == -1){
            return FALSE;
        }
        fd+=4;
        fdC+=2;
    }
    return TRUE;
}

void parentCloseReadPipeEnd(int *fd,int numOfProcess,int *fdc){
    while(numOfProcess -- >0) {
        close(*(fd + READ_END));
        close(*(fd + READ_END + 2));
        close(*(fdc + WRITE_END));
        fd += 4;
        fdc +=2;
    }
}
void childCloseWriteEndPipe(int *fd, int *fdc) {
    close(*(fd + WRITE_END));
    close(*(fd + WRITE_END + 2));
    close(*(fdc + READ_END));
}
void chldCloseReadEndPipe(int *fd, int *fdc) {
    close(*(fd + READ_END));
    close(*(fd + READ_END + 2));
    close(*(fdc + WRITE_END));
}
void parentWritePipe(int numOfProcess,int pointsPerProcess, int pointsMod,int *fd, struct point *a,int *fdc,int *inCircleTotal) {
    int i = 0;
    while (i < numOfProcess) {
        int tempInC;
        if (i == (numOfProcess-1)) {
            write(*(fd + WRITE_END), &a[i * pointsPerProcess], sizeof(struct point) * (pointsMod));
            write(*(fd + WRITE_END + 2), &pointsMod, sizeof(int));
        } else {
            write(*(fd + WRITE_END), &a[i * pointsPerProcess], sizeof(struct point) * pointsPerProcess);
            write(*(fd + WRITE_END + 2), &pointsPerProcess, sizeof(int));
        }
        read(*(fdc+READ_END),&tempInC,sizeof(int));
        *inCircleTotal +=tempInC;
        close(*(fd + WRITE_END));
        close(*(fd + WRITE_END + 2));
        close(*(fdc +READ_END));
        fd += 4;
        fdc +=2;
	i++;
    }
}

float randFloat(){
   return -1 + 2*((float)rand()/RAND_MAX);
}
void createFile(FILE *fp,char* name,int numOfPoints){
    fp = fopen(name,"w");
    if(fp == NULL) {
        perror("fopen");
        exit(1);
    }
    fprintf(fp,"%d\n",numOfPoints);
    while(numOfPoints-->0){
        float x = randFloat();
        float y = randFloat();
        fprintf(fp,"%f,%f\n",x,y);
    }
    if(fclose(fp)!=0){
        perror("fclose");
        exit(1);
    }
}
int readFile(FILE *fp,char *name,struct point **a){
    char *line = NULL;
    char seperators[] = ", ";
    fp = fopen(name,"r");
    size_t len = 0;
    ssize_t sizeR;
    int i=0;
    int num;
    if(fp == NULL) {
        perror("fopen");
        exit(1);
    }
    if((sizeR=getline(&line,&len,fp)!=-1)) {
        num = (int)atof(strtok(line,"\n"));
    }
    *a = malloc(num* sizeof(struct point));
    while((sizeR=getline(&line,&len,fp)!=-1)){
        if(i>num){
           break;
        }
        (*a)[i].x = atof(strtok(line,seperators));
        (*a)[i].y = atof(strtok(NULL,seperators)); i++;
    }
    if(i != num){
        perror("num of points doesn't match\n");
        fclose(fp);
        free(*a);
        exit(1);
    }
    free(line);
    if(fclose(fp)!=0){
        perror("fclose");
        exit(1);
    }
    return num;
}
int main(int argn,char *arg[])
{
    if(argn == 3){
       printf("file name is %s\n",arg[1]);
       printf("number of processes is %d\n",(int)atof(arg[2]));
    }else{
       perror("arg number doesn't match, exit");
       exit(1);
    }
    char *fn = arg[1];
    int numOfProcess = (int)atof(arg[2]);
    FILE *fp;
    struct point *a1;
    int numOfPoint=readFile(fp,fn,&a1);
    int numInCircle=0;
    //split points evenly
    int pointsPerProcess = numOfPoint/numOfProcess;
    int pointsMod = numOfPoint%numOfProcess+pointsPerProcess;
    //adjusting num of processes if needed
    if(numOfPoint<numOfProcess){
        printf("num of points is too small when compared with num of processes,adjusting process number to point number\n");
        numOfProcess = numOfPoint;
        pointsPerProcess = pointsMod = 1;
    }
    pid_t pid;
    //each child process has 3 pipes
    //1 for pass points
    //1 for pass num of points to be processed
    //1 for pass back num of in-circle points to parent
    int fda[numOfProcess][2][2];
    int fdCircle[numOfProcess][2];
    int *fd = &fda[0][0][0];
    int *fdC=&fdCircle[0][0];
    /* create the pipe */
    if (!createPipe(numOfProcess,fd,fdC)) {
        fprintf(stderr,"Pipe failed");
        exit(1);
    }
    /* now fork several child process */
    if (pid=fork(), pid < 0) {
        fprintf(stderr, "Fork failed");
        exit(1);
    }

    if (pid > 0) {  /* parent process */
        //parent close unused pipes
        parentCloseReadPipeEnd(fd,numOfProcess,fdC);
        //papent using pipes to pass info to children
        //and get processed info from children
        parentWritePipe(numOfProcess,pointsPerProcess,pointsMod,fd,a1,fdC,&numInCircle);
        wait(NULL);
        printf("number of sample points is %d, number of in-circle points is %d\nEstimated PI value is %f\n",numOfPoint,numInCircle,4*(float)numInCircle/(float)numOfPoint);
    }
    else { /* child process */
        //create multiple child process
        createMultiPro(numOfProcess,&fd,&fdC);
        /* close the unused end of the pipe */
        childCloseWriteEndPipe(fd,fdC);
        int i;//index
        //use one pipe to get the num of points to be processed
        read(*(fd+READ_END+2),&i,sizeof(int));
        struct point *pp;
        pp = malloc(i * sizeof(struct point));
        //use another pipe to get points
        read(*(fd+READ_END), pp, i*sizeof(struct point));
        int cnt=0;//points in circle
        //calc in-circle points and pass back
        calcInCirPoints(i,pp,&cnt);
        write(*(fdC+WRITE_END),&cnt,sizeof(int));
        //close pipes
        chldCloseReadEndPipe(fd,fdC);
        free(pp);
        wait(NULL);
    }
    free(a1);
}
