#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void interruptSignalHandler(int signalType); // Interrupt signal handling function

int main(int argc, char *argv[]) {
  struct sigaction handler;    // Signal handler specification structure

  // Set InterruptSignalHandler() as handler function
  handler.sa_handler =  interruptSignalHandler;
  // Create mask that mask all signals
  if (sigfillset(&handler.sa_mask) < 0) {
    perror("sigfillset() failed");
    exit(1);
  }
  // No flags
  handler.sa_flags = 0;

  // Set signal handling for interrupt signals
  if (sigaction(SIGINT, &handler, 0) < 0) {
    perror("sigaction() failed");
    exit(1);
  }

  for (;;) {
    pause();  // Suspend program until signal received
  }

  exit(0);
}

void interruptSignalHandler(int signalType) {
  printf("Interrupt Received.\n");
}
