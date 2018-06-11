#include <stdio.h>
#include <fcntl.h>
#include <zconf.h>

int fake_write(int fd,const void *buffer,size_t sz){
    return sz;
}
int main() {
    //open file
    int file = open("/dev/null",O_WRONLY);
    char *s = "1234567890";
    int i = 0;
    //write
    for(i = 0; i<100000000;++i)
        fake_write(file,s,10);
}