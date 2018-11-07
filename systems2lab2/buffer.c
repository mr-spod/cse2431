#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"

/* the buffer */
buffer_item buffer[BUFFER_SIZE];
int in;
int out;
/* mutex lock */
pthread_mutex_t mutex;
/* the semaphores */
sem_t empty, full;

int insert_item(buffer_item item) {
  if (in < BUFFER_SIZE) {
    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    return 0;
  } else { return -1; }
}

void *producer(void *param) {
  buffer_item item;
  int seed;
  seed = pthread_self();

  while (1) {
    /* sleep for a rendom time */
    int rNum = rand() / 1000000000;
    sleep(rNum);

    /* make a random number */
    item = rand_r(&seed);

    /* wait for empty, then lock mutex */
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    if (insert_item(item)) {
      printf("Producer error from insert_item -- bad index\n");
    } else {
      printf("Producer produced %d\n", item);
    }

    /* unlock mutex, then post full */
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
  }
}

int remove_item(buffer_item *item) {
  if (out < BUFFER_SIZE) {
    *item = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    return 0;
  } else { return 1; }
}

void *consumer(void *param) {
  buffer_item item;

  while (1) {
    /* sleep for a rendom time */
    int rNum = rand() / 1000000000;
    sleep(rNum);

    /* wait for full, then lock mutex */
    sem_wait(&full);
    pthread_mutex_lock(&mutex);

    if (remove_item(&item)) {
      printf("Consumer error from remove_item -- bad index\n");
    } else {
      printf("Consumer consumed %d\n", item);
    }

    /* unlock mutex, then post empty */
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
  }
}

int main(int argc, char *argv[]) {
  int i;
  pthread_t tid;
  pthread_attr_t attr;
  /* Get default attribute */
  pthread_attr_init(&attr);

  /* 1. Get command line arguments argv[1], argv[2], argv[3] */
  if (argc != 4) fprintf(stderr, "Usage error: 3 and only 3 parameters allowed");
  int sleepTime = atoi(argv[1]);
  int numProducers = atoi(argv[2]);
  int numConsumers = atoi(argv[3]);
  /* 2. Initialize buffer, mutex, semaphores, and other global vars */
  in, out = 0;
  /* init the mutex lock */
  pthread_mutex_init(&mutex, NULL);
  /* init the semaphores */
  sem_init(&full, 0, 0);
  sem_init(&empty, 0, BUFFER_SIZE);
  /* 3. Create producer thread(s) */
  for (i = 0; i < numProducers; i++) {
    pthread_create(&tid, &attr, producer, NULL);
  }
  /* 4. Create consumer thread(s) */
  for (i = 0; i < numConsumers; i++) {
    pthread_create(&tid, &attr, consumer, NULL);
  }
  /* 5. Sleep */
  sleep(sleepTime);
  /* 6. Release resources, e.g. destroy mutex and semaphores */
  pthread_mutex_destroy(&mutex);
  sem_destroy(&full);
  sem_destroy(&empty);
  /* 7. Exit */
  return (0);
}
