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

Q *q;
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

    q = (Q *) malloc(sizeof(Q));
    init_q(q);
    q_lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(q_lock, NULL);
    q_not_empty = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(q_not_empty, NULL);

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
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    fp = fopen((char *) arg, "r");
    while ((nread = getline(&line, &len, fp)) != -1) {
        pthread_mutex_lock(q_lock);
        enqueue(q, (void *) line);
        pthread_cond_broadcast(q_not_empty);
        pthread_mutex_unlock(q_lock);
        printf("Enqueued line: %s", line);
    }

    int endOfFile = EOF;
    pthread_mutex_lock(q_lock);
    enqueue(q, (void *) &endOfFile);
    pthread_mutex_unlock(q_lock);
    printf("Production finished\n");

    pthread_exit(NULL);
}

/**
 * Dequeues lines in a shared queue, analyzes the contents of the line, and
 * print the analysis
 */
void *consumeLines(void *arg) {
    char *currentLine = NULL;
    bool productionFinished = false;

    while (!productionFinished) {
        pthread_mutex_lock(q_lock);
        while (size(q) == 0)
            pthread_cond_wait(q_not_empty, q_lock);
        currentLine = (char *) dequeue(q);
        pthread_mutex_unlock(q_lock);
        if ((int) currentLine[0] != EOF) {
            printf("Dequeued line: %s", currentLine);
        }
        else
            productionFinished = true;
    }
    pthread_exit(NULL);
}
