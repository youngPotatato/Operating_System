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

//void createFile(FILE *fp,char* name,int numOfPoints){
//    fp = fopen(name,"w");
//    if(fp == NULL) {
//        perror("fopen");
//        exit(1);
//    }
//    fprintf(fp,"%d\n",numOfPoints);
//    while(numOfPoints-->0){
//        float x = randFloat();
//        float y = randFloat();
//        fprintf(fp,"%f,%f\n",x,y);
//    }
//    if(fclose(fp)!=0){
//        perror("fclose");
//        exit(1);
//    }
//}
int main() {
    int num = 10000;
    char *fileName = "yy1.txt";

    FILE *fp = fopen(fileName,"wb");
    if(fp == NULL) {
        perror("fopen");
        exit(1);
    }
    Node *head;
    //toDo: Node size is padded to 16 byte, param check to prevent int_32 overflow
    //todo: accept aug of file path name
    head = (Node*)malloc(sizeof(struct Node) * num);
    if(head == NULL)
        return -1;
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