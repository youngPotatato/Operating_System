#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

const size_t BLOCK_SIZE = 4;
const key_t SHMEM_KEY = 9876;
const int SHMEM_MODE = (SHM_R | SHM_W);

int main(int argc, char *argv[]) {

  // Attach existing shared memory segment
  int shmid = shmget(SHMEM_KEY, 0, SHMEM_MODE);
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

  // Busy wait until the sender marks the memory as ready
  // COMPUTATIONALLY EXPENSIVE!
  while (sbuffer[BLOCK_SIZE - 1] == 0);

  // Print out the string
  printf("%s\n", sbuffer);

  // Tell the sender we are done with the string
  sbuffer[BLOCK_SIZE - 1] = 0;

  // Release our reference to the shared memory segment
  if (shmdt(sbuffer) == -1) {
    perror("shmdt() failed");
    exit(1);
  }

  exit(0);
}
