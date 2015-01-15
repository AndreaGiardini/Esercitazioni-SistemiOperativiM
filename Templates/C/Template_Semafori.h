#ifndef __EserciziSistemiOperativi__Template__Semafori__H
#define __EserciziSistemiOperativi__Template__Semafori__H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

//DEFINIZIONI
#define THREADS 10



//DICHIARAZIONI TIPI
/*
typedef struct
{
    sem_t semaforo1;
    sem_t semaforo2;
    pthread_mutex_t lock;
} nuovo_tipo;
*/

//DICHIARAZIONI VARIABILI
pthread_t threads[THREADS];
//nuovo_tipo n;

//FUNZIONI
/*
void init(nuovo_tipo *);
*/


//THREADS
void * thread1(void *);
void * thread2(void *);

#endif
