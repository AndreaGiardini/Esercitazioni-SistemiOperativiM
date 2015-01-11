#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREAD 20

#include "barriera.h"

typedef struct {
} museo;

museo mus;
barriera bar;

void init_museo (museo* m);

void* visitatore (void *arg);

int main ()
{
    int i, ret_code;
    void* status;
    pthread_t thread[NUM_THREAD];

    init_barriera(&bar, NUM_THREAD);
    init_museo(&mus);

    for (i = 0; i < NUM_THREAD; ++i) {
        ret_code = pthread_create(&thread[i], NULL, visitatore, (void *) (intptr_t) i);

        if (ret_code) {
            printf("ERRORE: %d\n", ret_code);
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < NUM_THREAD; ++i) {
        ret_code = pthread_join(thread[i], &status);

        if (ret_code) {
            printf("ERRORE join thread %d codice %d\n", i, ret_code);
        }
    }

    return EXIT_SUCCESS;
}

void init_museo (museo* m)
{
}

void* visitatore (void *arg)
{
}
