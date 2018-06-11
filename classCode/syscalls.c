
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <buffer size>\n", argv[0]);
    exit(1);
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd < 0) {
    perror("open() failed");
    exit(1);
  }

  int bufSize = atoi(argv[2]);

  ssize_t bytesRead;
  unsigned long totBytesRead = 0;
  char buf[bufSize];
  do {
    bytesRead = read(fd, buf, bufSize);
     if (bytesRead == -1) {
       perror("read() failed");
       exit(1);
     }
    totBytesRead += bytesRead;
  } while (bytesRead > 0);

  close(fd);

  printf("Bytes Read = %lu\n", totBytesRead);
}
