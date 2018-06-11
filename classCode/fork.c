#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void sayGreeting(); // Handle single greeting

// Global greeting string (Bad design!!  Don't do this!)
char *greeting;

int main(int argc, char *argv[]) {

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <greeting> [<greeting>...]\n", argv[0]);
    exit(1);
  }

  int greetingCt = argc - 1; // Number of command-line greetings
  pid_t pid[greetingCt - 1]; // List of process ids

  // Create greeting processes
  int procIdx;
  for (procIdx = 0; procIdx < greetingCt - 1; procIdx++) {
    pid_t newPid = fork();
    if (newPid < 0) {
      perror("fork() failed");
      exit(1);
    }
    if (newPid > 0) { // Parent
      pid[procIdx] = newPid;
    } else { // Child
      greeting = argv[procIdx + 1];
      sayGreeting();
      exit(0);
    }
  }

  greeting = argv[procIdx+1];
  sayGreeting();

  // Wait for processes to complete
  for (int i = 0; i < greetingCt - 1; i++) {
    if (waitpid(pid[i], NULL, 0) == -1) {
      perror("waitpid() failed");
      exit(1);
    }
  }

  exit(0);
}

// Function to handle single greeting
void sayGreeting() {
  // Repeatedly print message
  for (int i = 0; i < 10000; i++) {
    printf("%d: %s\n", getpid(), greeting);
  }
}
