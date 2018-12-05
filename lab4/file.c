#include <sys/types.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

int matrixA[1200][1000];
int matrixB[1000][500];
int matrixC[1200][500];

void *multiplyMatricesPortion(void *arg) {
  int i, j, k, upperi, upperj, newval;
  // fprintf(stderr, "hello again?\n");
  int *param = (int *) arg;
  // fprintf(stderr, "param: %d\n", *param);
  int portion = *param % 10;
  int numPortions = *param % 100;
  i = (portion / numPortions) * 1200;
  j = (portion / numPortions) * 500;
  upperi = ((portion + 1) / numPortions) * 1200;
  // printf("portion %d, upperi %d\n", portion, upperi);
  upperj = ((portion + 1) / numPortions) * 500;
  // printf("upperj %d\n", upperj);

  for (i; i < upperi; i++) {
    for (j; j < upperj; j++) {
      for (k = 0; k < 1000; k++) {
        newval = matrixA[i][k] * matrixB[k][j];
        if (matrixC[i][j] != newval && matrixC[i][j] != 0) {
          printf("error: got a different value for same indices. old: %d, new: %d\n", matrixC[i][j], newval);
          printf("i: %d, j: %d, k: %d\n", i, j, k);
          printf("portion: %d, numPortions: %d\n", portion, numPortions);
        }
        matrixC[i][j] = matrixA[i][k] * matrixB[k][j];
      }
    }
  }
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

  pthread_attr_init(&attr);
  // printf("attr init passed\n");
  initializeMatrices();
  // printf("initialized matrices\n");

  for (n = 2; n <= 6; n++) {
    printf("n = %d\n", n);
    before = clock();
    for (i = 0; i < n; i++) {
      arg = n * 10;
      arg += i;
      int error = pthread_create(&tid[i], &attr, multiplyMatricesPortion, (void *) &arg);
      // if (error == 0)
      //   // printf("thread dispatch successfully\n");
      // else
      //   // printf("Error %d: could not create thread\n", error);
    }
    for (i = 0; i < n; i++) {
      // printf("about to call pthread_join on thread %d of %d\n", i, n);
      int error = pthread_join(tid[i], NULL);
      // if (error == 0)
      //   // printf("thread joined successfully\n");
      // else
      //   // printf("Error %d: could not join thread %d of %d\n", error, i, n);
    }
    difference = clock() - before;
    printf("%diff: lu, before: %lu", difference, before);
    printf("Running with %d threads took %lu seconds, %lu milliseconds\n", n, difference / 1000, difference % 1000);
  }
}
