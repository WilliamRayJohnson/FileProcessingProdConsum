/*
 * A concurrent program counts the number of vowels, consonants, non-alphabetic
 * characters, and words of each line of a file passed in as an argument
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "q.h"

#define THREAD_CREATION_FAILED -1
#define THREAD_JOIN_FAILED -2

#define PADDING 4

void *produceLines(void *arg);
void *consumeLines(void *arg);

bool isVowel(char letter);
void analyzeLine(char *line);

Q *q;
pthread_mutex_t *q_lock;
pthread_cond_t *q_not_empty;

int main(int argc, char *argv[]) {
    pthread_t *producerThread;
    pthread_t *consumerThread;

    if(argc!=2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(-1);
    }

    /**
     * Initialize queue and associated variables [lock and condition variable]
     */
    q = (Q *) malloc(sizeof(Q));
    init_q(q);
    q_lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(q_lock, NULL);
    q_not_empty = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(q_not_empty, NULL);

    /**
     * Create producer thread
     */
    producerThread = (pthread_t *) malloc(sizeof(pthread_t));
    if (pthread_create(producerThread, NULL, produceLines, (void *) argv[1])) {
        fprintf(stderr, "Error creating producer thread");
        exit(THREAD_CREATION_FAILED);
    }

    /**
     * Print out header of output
     */
    char *pad = "-----------------------------------------------";
    printf("%.*s %.*s %.*s %.*s: the line\n", PADDING, "vowels", PADDING, "consonants",
            PADDING, "non-alphabetics", PADDING, "words");
    printf("%.*s %.*s %.*s %.*s\n", PADDING, pad, PADDING, pad, PADDING, pad, PADDING, pad);

    /**
     * Create consumer thread
     */
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
    char *queueLine;

    /**
     * Read each line of a file, and add them to the queue
     */
    fp = fopen((char *) arg, "r");
    while ((nread = getline(&line, &len, fp)) != -1) {
        queueLine = (char *) malloc(strlen(line)+1);
        strcpy(queueLine, line);
        pthread_mutex_lock(q_lock);
        enqueue(q, (void *) queueLine);
        pthread_cond_broadcast(q_not_empty);
        pthread_mutex_unlock(q_lock);
    }

    /**
     * Add EOF to queue to signal to the consumer that the file has been
     * completely read
     */
    int endOfFile = EOF;
    pthread_mutex_lock(q_lock);
    enqueue(q, (void *) &endOfFile);
    pthread_cond_broadcast(q_not_empty);
    pthread_mutex_unlock(q_lock);

    pthread_exit(NULL);
}

/**
 * Dequeues lines in a shared queue, analyzes the contents of the line, and
 * print the analysis
 */
void *consumeLines(void *arg) {
    char *currentLine = NULL;
    bool productionFinished = false;

    /**
     * Wait for an item to appear on the queue, then analyze the line until EOF
     * is found
     */
    while (!productionFinished) {
        pthread_mutex_lock(q_lock);
        while (size(q) == 0)
            pthread_cond_wait(q_not_empty, q_lock);
        currentLine = (char *) dequeue(q);
        pthread_mutex_unlock(q_lock);
        if (currentLine[0] != EOF && currentLine[0] != '\0') {
            analyzeLine(currentLine);
        }
        else
            productionFinished = true;
    }
    pthread_exit(NULL);
}

/**
 * Analyzes line and prints analysis
 */
void analyzeLine(char *line) {
    int i = 0;
    int vowelCount = 0;
    int consonantCount = 0;
    int nonAlphaCount = 0;
    int wordCount = 0;
    char prevChar = ' ';

    /**
     * Count each type of character in the given string
     */
    while (line[i] != '\0') {
        if(isdigit(line[i]) || ispunct(line[i])) {
            nonAlphaCount++;
            prevChar = line[i];
        }
        else if(isalpha(line[i]) && isVowel(line[i])) {
            vowelCount++;
            prevChar = line[i];
        }
        else if(isalpha(line[i]) && !isVowel(line[i])) {
            consonantCount++;
            prevChar = line[i];
        }
        else if(isspace(line[i]) && !isspace(prevChar)) {
            wordCount++;
            prevChar = line[i];
        }
        i++;
    }
    /**
     * If the last character is not a space, wordCount must be
     * incremented since it wouldn't have been conunted
     */
    if(!isspace(prevChar))
        wordCount++;

    /**
     * Print the stats of the line
     */
    printf("%*d %*d %*d %*d: %s", PADDING,
            vowelCount, PADDING, consonantCount, PADDING,
            nonAlphaCount, PADDING, wordCount, line);
}

/**
 * Determines if character is a vowel
 */
bool isVowel(char letter) {
    letter = tolower(letter);
    if ('a' == letter || 'e' == letter || 'i' == letter || 'o' == letter || 'u' == letter)
        return true;
    else
        return false;
}
