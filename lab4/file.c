#include <sys/types.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <omp.h>

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
    printf("Running with %d threads took %f seconds, %f\n", n, difference * 1000);
  }
}
