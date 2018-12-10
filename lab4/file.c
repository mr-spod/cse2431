#include <sys/types.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <omp.h>

/*
Lab 4
Author: Sean O'Donnell

Compile: gcc -O1 -Wall -o exec_file file.c -fopenmp -lpthread
Run: exec_file
*/

#define X 1200
#define Y 1000
#define Z 500
int matrixA[X][Y];
int matrixB[Y][Z];
int matrixC[X][Z];
int matrixCSolution[X][Z];

void *multiplyMatricesPortion(void *arg) {
  int i, j, k, upperi, upperj;
  int *param = (int *) arg;
  int portion = *param % 10;
  int numPortions = *param / 10;
  i = (X / numPortions) * portion;
  j = (Z / numPortions) * portion;
  k = (Y / numPortions) * portion;
  if (portion == (numPortions - 1)) {
    upperi = X;
    upperj = Z;
    upperk = Y;
  } else {
    upperi = (X / numPortions) * (portion + 1);
    upperj = (Z / numPortions) * (portion + 1);
    upperk = (Y / numPortions) * (portion + 1);
  }

  for (i < upperi; i++) {
    for (j < upperj; j++) {
      matrixC[i][j] = 0;
      for (k = 0; k < Y; k++) {
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
  for (i = 0; i < X; i++) {
    for (k = 0; k < Y; k++) {
      matrixA[i][k] = i + k;
    }
  }
  for (k = 0; k < Y; k++) {
    for (j = 0; j < Z; j++) {
      matrixB[k][j] = j;
    }
  }
  for (i = 0; i < X; i++) {
    for (j = 0; j < Z; j++) {
      matrixC[i][j] = 0;
      matrixCSolution[i][j] = 0;
      for (k = 0; k < Y; k++) {
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
  double before, difference;

  tid = malloc(6 * sizeof(pthread_t));
  pthread_attr_init(&attr);
  arg = 0;
  argP = &arg;
  for (n = 2; n <= 6; n++) {
    initializeMatrices();
    printf("n = %d\n", n);
    before = omp_get_wtime();
    for (i = 0; i < n; i++) {
      argP = malloc(sizeof(int));
      *argP = n * 10;
      *argP += i;
      if (pthread_create(&(tid[i]), &attr, multiplyMatricesPortion, (void *) argP) != 0)
        printf("Error: could not create thread\n");
    }
    for (i = 0; i < n; i++) {
      if (pthread_join(tid[i], NULL) != 0)
        printf("Error: could not join thread %d of %d\n", i, n);
    }
    difference = omp_get_wtime() - before;
    free(argP);
    printf("Running with %d threads took %.4f seconds\n", n, difference * 1000);
  }
}
