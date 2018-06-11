#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

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

  // Randomly select and fetch records
  char result[NAMELEN];
  for (int i=0; i < NONAMES; i++) {
    int namePos = rand() % NONAMES;  // Randomly select record
    
    // Go to record offset
    if (lseek(fd, namePos*NAMELEN, SEEK_SET) < 0) {
      perror("lseek() failed");
      exit(1);
    }
    
    // Fetch record
    int totBytesRead = 0;
    do {
      int bytesRead = read(fd, result, NAMELEN-totBytesRead);
      if (bytesRead < 0) {
        perror("read() failed");
        exit(1);
      }
      totBytesRead += bytesRead;
    } while (totBytesRead < NAMELEN);
  }
  close(fd);
}