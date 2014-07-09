#ifndef BARRIERA_H
#define BARRIERA_H

#include <semaphore.h>

typedef struct {
    sem_t sem_mutex;
    sem_t sem_barriera;
    int arrivati;
    int num_thread;
} barriera;

void init_barriera(barriera *b, int num_thread);
void sync_barriera(barriera *b);

#endif
