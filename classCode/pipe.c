#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

void readFully(int desc, void *buf, int size);

/*
 * Takes two numbers from the command line, spawns a child process which writes
 * the numbers over an unnamed pipe to the parent, and the parent prints the sum
 */
int main(int argc, char *argv[]) {

  // Test arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <num1> <num2>\n", argv[0]);
    exit(1);
  }

  // Make a pipe for talking with the child.
  int pfd[2];
  if (pipe(pfd) == -1) {
    perror("Can't create pipe");
    exit(1);
  }

  // Make the child process
  pid_t pid = fork();
  if (pid < 0) {
    perror("Not able to fork a child");
    exit(1);
  }

  // If I'm the child, send two values from command line
  if (pid == 0) {
    // Close the reading end of the pipe.  We don't need it.
    close(pfd[0]);

    // Send the two values to our parent process.
    int val = atoi(argv[1]);
    write(pfd[1], &val, sizeof(int));  // We should test write return!
    val = atoi(argv[2]);
    write(pfd[1], &val, sizeof(int));
    close(pfd[1]);
    exit(0);
  }

  // I'm the parent, I don't need the writing end of the pipe.
  close(pfd[1]);

  // Read the two values and compute the sum
  int val;
  readFully(pfd[0], &val, sizeof(int));
  int sum = val;
  readFully(pfd[0], &val, sizeof(int));
  sum += val;

  // Done reading
  close(pfd[0]);

  printf("Sum = %d\n", sum);

  // Wait for our child to exit.
  wait(0);
  exit(0);
}

/*
 * Reads size bytes from desc into buf
 */
void readFully(int desc, void *buf, int size) {

  int count = 0; // Count of bytes read so far
  int rcvd;      // Byte read in last read()
  while (count < size) {
    rcvd = read(desc, ((char *) buf) + count, size - count);
    if (rcvd < 0) {  // If error
      perror("read() failed");
      exit(1);
    } else if (rcvd == 0) {  // If premature end of stream
      fputs("Unexpected EOS", stderr);
      exit(1);
    }
    count += rcvd;
  }
}
