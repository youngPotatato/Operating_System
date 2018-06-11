#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

#define NONAMES 100000
#define NAMELEN 20

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Usage %s <filename>\n", argv[0]);
    exit(1);
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd < 0) {
    perror("open() failed");
    exit(1);
  }

  // Establish mapped memory
  char *start = mmap(0, NONAMES*NAMELEN, PROT_READ, MAP_PRIVATE, fd, 0);
  if (start == MAP_FAILED) {
    perror("mmap() failed");
    exit(1);
  }

  // Randomly select and fetch records
  char result[NAMELEN];
  for (int i=0; i < NONAMES; i++) {
    int namePos = rand() % NONAMES;   // Randomly select record
    char *c = start+namePos*NAMELEN;  // Compute record offset
    memcpy(result, c, NAMELEN);       // Fetch record
  }

  if (munmap(start, NONAMES*NAMELEN) == -1) {
    perror("munmap() failed");
    exit(1);
  }

  close(fd);
}