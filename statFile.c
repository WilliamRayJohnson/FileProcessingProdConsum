/*
 * A concurrent program counts the number of vowels, consonants, non-alphabetic
 * characters, and words of each line of a file passed in as an argument
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "q.h"

#define THREAD_CREATION_FAILED -1
#define THREAD_JOIN_FAILED -2

void *produceLines(void *arg);
void *consumeLines(void *arg);

NODE *q;
pthread_mutex_t *q_lock;
pthread_cond_t *q_not_empty;
pthread_cond_t *q_not_full;

int main(int argc, char *argv[]) {
    pthread_t *producerThread;
    pthread_t *consumerThread;

    if(argc!=2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(-1);
    }

    producerThread = (pthread_t *) malloc(sizeof(pthread_t));
    if (pthread_create(producerThread, NULL, produceLines, (void *) argv[1])) {
        fprintf(stderr, "Error creating producer thread");
        exit(THREAD_CREATION_FAILED);
    }

    consumerThread = (pthread_t *) malloc(sizeof(pthread_t));
    if (pthread_create(consumerThread, NULL, consumeLines, NULL)) {
        fprintf(stderr, "Error creating consumer thread");
        exit(THREAD_CREATION_FAILED);
    }

    if (pthread_join(*producerThread, NULL)) {
        fprintf(stderr, "Error joining producer thread");
        exit(THREAD_JOIN_FAILED);
    }

    if (pthread_join(*consumerThread, NULL)) {
        fprintf(stderr, "Error joining consumer thread");
        exit(THREAD_JOIN_FAILED);
    }

    exit(0);
}

/**
 * Reads each line of a file and enqueues each line to a shared queue to be consumed
 */
void *produceLines(void *arg) {
    printf("Producer created to read %s\n", (char *) arg);
    pthread_exit(NULL);
}

/**
 * Dequeues lines in a shared queue, analyzes the contents of the line, and
 * print the analysis
 */
void *consumeLines(void *arg) {
    printf("Consumer created\n");
    pthread_exit(NULL);
}
