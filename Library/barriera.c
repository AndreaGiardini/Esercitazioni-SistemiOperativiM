#include "barriera.h"

void init_barriera(barriera *b, int num_thread)
{
    sem_init(&b->mb, 0, 1);
    sem_init(&b->sb, 0, 0);

    b->arrivati   = 0;
    b->num_thread = num_thread;
}

void sync_barriera(barriera *b)
{
    sem_wait(&b->mb);
    b->arrivati++;

    if (b->arrivati == b->num_thread){
        sem_post(&b->sb);
    }

    sem_post(&b->mb);
    sem_wait(&b->sb);
    sem_post(&b->sb);
}
