#include <stdio.h>
#include <fcntl.h>
#include <zconf.h>

int main() {
    int file = open("/dev/null",O_WRONLY);
    char *s = "1234567890";
    int i = 0;
    for(i = 0; i<100000000;++i)
        write(file,s,10);
}
