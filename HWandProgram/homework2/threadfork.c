#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/wait.h>

void *runner(void *param){
	if(fork()==0){
		printf("command line param is %s\n",param);
		//wait another
		wait(NULL);
	}else{
		if(fork() == 0){
			printf("fork() in thread");
		}
		printf("fork() in thread 2nd\n");
	}
	printf("after fork\n");
	pthread_exit(0);
}
int main(int argc, char *ag[]) {
	if(argc != 2){
		perror("arg num is not 2");
		exit(1);
	}
	pthread_t pid;
	pthread_attr_t at;
	pthread_attr_init(&at);
	pthread_create(&pid,&at,runner,ag[1]);
	pthread_join(pid,NULL);
}

