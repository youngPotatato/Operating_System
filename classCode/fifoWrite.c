#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

const int BLOCK_SIZE = 4;

/*
 * Demonstrate named pipes.  This is the writer side.
 *
 * Be sure to make the pipe, myfifo, first with "mkfifo myfifo"
 */
int main(int argc, char *argv[]) {
  // Open the fifo for writing.
  int fd = open("myfifo", O_WRONLY);
  if (fd < 0) {
    perror("Cannot open fifo for writing");
    exit(1);
  }

  // Read input from the user and send it over the fifo
  char buffer[BLOCK_SIZE];
  int len;
  while ((len = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
    write(fd, buffer, len);
  }

  // Close the fifo and exit
  close(fd);
  exit(0);
}