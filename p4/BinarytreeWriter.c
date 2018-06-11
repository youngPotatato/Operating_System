#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

typedef struct Node{
    int32_t loc;
    char value[2];

    int32_t  left;
    int32_t  right;
} Node;

int main(int argn,char *arg[]){

    if(argn == 2){
       printf("file name is %s\n",arg[1]);
    }else{
       perror("arg number doesn't match, exit");
       exit(1);
    }

    // number of Node to be generated
    int num = 10000;
    int y = INT32_MAX;
    char *fileName = arg[1];

    FILE *fp = fopen(fileName,"wb");
    if(fp == NULL) {
        perror("fopen");
        exit(1);
    }
    Node *head;
    //!!note: Node size is padded to 16 byte
    //INT32_MAX = Ox7fff_ffff, so the max number of Node
    // should < INT32_MAX, to prevent overflow
    // if test case would override this number anyhow
    // should comply with this rule
    // because in the canvas page, there is no description about
    // a argument of this number, this program does not perform
    // overflow check
    head = (Node*)malloc(sizeof(struct Node) * num);
    if(head == NULL)
        return -1;
    //set all the nodes
    for(int i = 0; i < num; i++){
        (head+i)->loc = i;
        (head+i)-> value[0] = 'A' + (rand() % 26);
        (head+i)->value[1] = 'A' + (rand() % 26);
        int32_t left = (i+1)<<1;
        int32_t right= left + 1;
        (head+i)->left = (left<=num)?(left-1):-1;
        (head+i)->right= (right<=num)?(right-1):-1;
        fwrite((head+i),sizeof(struct Node),1,fp);
    }
    if(fclose(fp)!=0){
        perror("fclose");
        exit(1);
    }
    free(head);
};
