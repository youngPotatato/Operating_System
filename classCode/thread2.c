#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

void *sayGreeting(void *param); // Handle single greeting

// Global greeting string (Bad design!!  Don't do this!)
char *greeting;

int main(int argc, char *argv[]) {

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <greeting> [<greeting>...]\n", argv[0]);
    exit(1);
  }

  int greetingCt = argc - 1; // Number of command-line greetings
  pthread_t tid[greetingCt - 1]; // List of thread ids

  // Create greeting threads
  int threadIdx;
  for (threadIdx = 0; threadIdx < greetingCt-1; threadIdx++) {
    greeting = argv[threadIdx + 1];
    int rtn = pthread_create(&tid[threadIdx], NULL, sayGreeting, greeting);
    if (rtn != 0) {
      fprintf(stderr, "Unable to create pthread: %s\n", strerror(rtn));
      exit(1);
    }
  }

  greeting = argv[threadIdx+1];
  sayGreeting(greeting);

  // Wait for threads to complete
  for (int i = 0; i < greetingCt - 1; i++) {
    int rtn = pthread_join(tid[i], NULL);
    if (rtn != 0) {
      fprintf(stderr, "Unable to join thread: %s\n", strerror(rtn));
      exit(1);
    }
  }

  exit(0);
}

// Function to handle single greeting
void *sayGreeting(void *param) {
  // Repeatedly print message
  for (int i = 0; i < 1000; i++) {
    printf("%lu: %s\n", pthread_self(), (char*)param);
  }

  pthread_exit(NULL);
}
