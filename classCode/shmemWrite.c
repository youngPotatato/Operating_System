#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>

const size_t BLOCK_SIZE = 4;
const key_t SHMEM_KEY = 9876;
const int SHMEM_MODE = (SHM_R | SHM_W);

int main(int argc, char *argv[]) {

  // Make a shared memory segment of the specified size
  int shmid = shmget(SHMEM_KEY, BLOCK_SIZE, SHMEM_MODE | IPC_CREAT);
  if (shmid == -1) {
    perror("shmget() failed");
    exit(1);
  }

  // Attach the shared memory segment
  char *sbuffer = shmat(shmid, NULL, 0);
  if (sbuffer == (void *) -1) {
    perror("shmat() failed");
    exit(1);
  }

  // Read from keyboard, saving last two positions for null terminator,
  // and signal
  ssize_t len = read(STDIN_FILENO, sbuffer, BLOCK_SIZE - 2);
  if (len < 0) {
    perror("read() failed");
    exit(1);
  }
  sbuffer[len] = '\0';

  // Tell the receiver that we have filled in the string
  sbuffer[BLOCK_SIZE - 1] = 1;

  // Busy wait for the receiver to use the string
  // COMPUTATIONALLY EXPENSIVE!
  while (sbuffer[BLOCK_SIZE - 1] == 1);

  // Release our reference to the shared memory segment
  if (shmdt(sbuffer) == -1) {
    perror("shmdt() failed");
    exit(1);
    }

  // Tell the OS we no longer need the segment
  if (shmctl(shmid, IPC_RMID, 0) == -1) {
    perror("shmctl() failed");
    exit(1);
  }

  exit(0);
}
