#include <sys/types.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

int matrixA[1200][1000];
int matrixB[1000][500];
int matrixC[1200][500];

void *multiplyMatricesPortion(void *arg) {
  int i, j, k, upperi, upperj, newval;
  int param = *((int *) arg);
  int portion = param % 10;
  int numPortions = param % 10;
  i = (portion / numPortions) * 1200;
  j = (portion / numPortions) * 500;
  upperi = ((portion + 1) / numPortions) * 1200;
  printf("portion %d, upperi %d", portion, upperi);
  upperj = ((portion + 1) / numPortions) * 500;
  printf("upperj %d", upperj);

  for (i; i < upperi; i++) {
    for (j; j < upperj; j++) {
      for (k = 0; k < 1000; k++) {
        newval = matrixA[i][k] * matrixB[k][j];
        if (matrixC[i][j] != newval && matrixC[i][j] != 0) {
          printf("error: got a different value for same indices. old: %d, new: %d", matrixC[i][j], newval);
          printf("i: %d, j: %d, k: %d", i, j, k);
          printf("portion: %d, numPortions: %d", portion, numPortions);
        }
        matrixC[i][j] = matrixA[i][k] * matrixB[k][j];
      }
    }
  }

  free(arg);
}

void initializeMatrices() {
  int i, j, k;
  for (i = 0; i < 1200; i++) {
    for (j = 0; j < 500; j++) {
      for (k = 0; k < 1000; k++) {
        matrixA[i][k] = i + k;
        matrixB[k][j] = j;
        matrixC[i][j] = 0;
      }
    }
  }
}

int main(void) {
  int i, arg;
  int n = 2;
  pthread_t tid[6];
  pthread_attr_t attr;
  clock_t before, difference;

  signal(SIGSEGV, handler);

  pthread_attr_init(&attr);
  printf("attr init passed\n");
  initializeMatrices();
  printf("initialized matrices\n");

  for (n = 2; n <= 6; n++) {
    printf("n = %d\n", n);
    before = clock();
    for (i = 0; i < n; i++) {
      arg = n * 10;
      arg += i;
      int error = pthread_create(&tid[i], &attr, multiplyMatricesPortion, arg);
      if (error == 0)
        printf("thread dispatch successfully\n");
      else
        printf("Error %d: could not create thread\n", error);
    }
    for (i = 0; i < n; i++) {
      printf("about to call pthread_join on thread %d of %d", i, n);
      int error = pthread_join(tid[i], NULL);
      if (error == 0)
        printf("thread joined successfully\n");
      else
        printf("Error %d: could not join thread %d of %d", error, i, n);
    }
    difference = clock() - before;
    printf("Running with %d threads took %lu seconds, %lu milliseconds", n, difference / 1000, difference % 1000);
  }
}
