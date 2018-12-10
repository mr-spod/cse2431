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
int matrixCSolution[1200][500];

void *multiplyMatricesPortion(void *arg) {
  int i, j, k, upperi, upperj, upperk;
  int *param = (int *) arg;
  int portion = *param % 10;
  int numPortions = *param / 10;
  i = (1200 / numPortions) * portion;
  j = (500 / numPortions) * portion;
  k = (1000 / numPortions) * portion;
  if (portion == (numPortions - 1)) {
    upperi = 1200;
    upperj = 500;
    upperk = 1000;
  } else {
    upperi = (1200 / numPortions) * (portion + 1);
    upperj = (500 / numPortions) * (portion + 1);
    upperk = (1000 / numPortions) * (portion + 1);
  }

  for (i; i < upperi; i++) {
    for (j; j < upperj; j++) {
      matrixC[i][j] = 0;
      for (k = 0; k < 1000; k++) {
        matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
      }
      if (matrixC[i][j] != matrixCSolution[i][j]) {
        fprintf(stdout, "error: incorrect value at row %d, column %d\n", i, j);
        fprintf(stdout, "solution: %d\n", matrixCSolution[i][j]);
        fprintf(stdout, "current: %d\n", matrixC[i][j]);
      }
    }
  }
  fflush(stdout);
}

void initializeMatrices() {
  int i, j, k;
  for (i = 0; i < 1200; i++) {
    for (k = 0; k < 1000; k++) {
      matrixA[i][k] = i + k;
    }
  }
  for (k = 0; k < 1000; k++) {
    for (j = 0; j < 500; j++) {
      matrixB[k][j] = j;
    }
  }
  for (i = 0; i < 1200; i++) {
    for (j = 0; j < 500; j++) {
      matrixC[i][j] = 0;
      matrixCSolution[i][j] = 0;
      for (k = 0; k < 1000; k++) {
        matrixCSolution[i][j] += matrixA[i][k] * matrixB[k][j];
      }
    }
  }
}

int main(void) {
  int i, *argP, arg;
  int n = 2;
  pthread_t *tid;
  pthread_attr_t attr;
  clock_t before, difference;

  tid = malloc(6 * sizeof(pthread_t));
  pthread_attr_init(&attr);
  arg = 0;
  argP = &arg;
  for (n = 2; n <= 6; n++) {
    initializeMatrices();
    printf("n = %d\n", n);
    before = clock();
    for (i = 0; i < n; i++) {
      argP = malloc(sizeof(int));
      *argP = n * 10;
      *argP += i;
      int error = pthread_create(&(tid[i]), &attr, multiplyMatricesPortion, (void *) argP);
      if (error == 0)
        printf("");
      else
        printf("Error %d: could not create thread\n", error);
    }
    for (i = 0; i < n; i++) {
      // printf("about to call pthread_join on thread %d of %d\n", i, n);
      int error = pthread_join(tid[i], NULL);
      if (error == 0)
        printf("");
      else
        printf("Error %d: could not join thread %d of %d\n", error, i, n);
    }
    free(argP);
    difference = clock() - before;
    printf("Running with %d threads took %lu seconds, %lu milliseconds\n", n, difference / 1000, difference % 1000);
  }
}
