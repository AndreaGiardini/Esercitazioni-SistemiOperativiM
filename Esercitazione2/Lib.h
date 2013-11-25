#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/*
* Definizione costanti
*/

//Numero utenti
#define NUM_THREADS 5

//Numero films
#define K 10


//Boolean
typedef enum { false, true } boolean;

//Barriera
typedef struct{
    sem_t mb;
    sem_t sb; 
    int arrivati;
} barriera;

//Struttura condivisa
typedef struct{
    pthread_mutex_t lock;
    int voti[NUM_THREADS];
} sondaggio;

/*
* Definizione variabili
*/


barriera B;
sondaggio obj[K];

/*
* Dichiarazione funzioni
*/

void init();

#endif
