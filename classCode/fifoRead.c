#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

const int BLOCK_SIZE = 4;

/*
 * Demonstrate named pipes.  This is the reader side.
 *
 * Be sure to make the pipe, myfifo, first with "mkfifo myfifo"
 */
int main(int argc, char *argv[]) {

  // Open the fifo for reading
  int fd = open("myfifo", O_RDONLY);
  if (fd < 0) {
    perror("Cannot open fifo for reading");
    exit(1);
  }

  // Read input from the fifo and dump it to the screen
  char buffer[BLOCK_SIZE];
  ssize_t len;
  while ((len = read(fd, buffer, sizeof(buffer))) > 0) {
    write(STDOUT_FILENO, buffer, len);
  }

  // Close the fifo and exit
  close(fd);
  exit(0);
}