#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define NONAMES 1000000
#define NAMELEN 20

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Usage %s <filename>\n", argv[0]);
    exit(1);
  }

  int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd < 0) {
    perror("open() failed");
    exit(1);
  }

  char name[NAMELEN];
  memset(name, ' ', sizeof(name));
  for (int i=0; i < NONAMES; i++) {
    snprintf(name, sizeof(name), "Bob Smith%d", i);
    write(fd, name, sizeof(name));
  }
  close(fd);
}