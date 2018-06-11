#include <stdio.h>
#include <zconf.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
int result[200];
void sig_handle(int sig){
    printf("terminated!");
};

int main() {
    signal(SIGINT,sig_handle);
    sleep(100);
}