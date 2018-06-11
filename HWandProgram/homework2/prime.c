#include <stdio.h>
#include <zconf.h>
#include <pthread.h>
#include <stdlib.h>
int result[200];
void *runner(void *param){
    //para is the bound
    int para = atoi(param);
    //1 is not prime
    result[0]=0;
    result[1]=0;
    int ii,iii;
    for(ii=2;ii<para;ii++)
        result[ii]= 1;
    //any int == ii * iii, is not prime
    for(ii=2;ii<=para/2;ii++)
        for(iii=2;iii<=para/ii;ii++)
           result[ii*iii]=0;
    pthread_exit(0);
};

int main(int argc, char *ag[]) {
    if(argc != 2) {
        perror("arg num is not2,");
        exit(1);
    }
    if(atoi(ag[1])<2) {
        perror("the num should be >2");
        exit(1);
    }
    pthread_t pid;
    pthread_attr_t at;
    int i;
    //get attr
    pthread_attr_init(&at);
    //creat thread
    pthread_create(&pid,&at,runner,ag[1]);
    //wait thread finish
    pthread_join(pid,NULL);
    for(i=0;i<atoi(ag[1]);i++){
        if(result[i])
            printf("prime: %d",i);
    }
}