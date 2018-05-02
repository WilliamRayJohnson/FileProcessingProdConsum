/*
 * A concurrent program counts the number of vowels, consonants, non-alphabetic
 * characters, and words of each line of a file passed in as an argument
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include <q.h>

#define THREAD_CREATION_FAILED -1
#define THREAD_JOIN_FAILED -2

void *produceLines(void *arg);
void *consumeLines(void *arg);

int main(int argc, char *argv[]) {
    if(argc!=2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(-1);
    }

    exit(0);
}
