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

#define NUM_THREADS 10

//Boolean
typedef enum { false, true } boolean;

//Struttura condivisa
typedef struct{
    /*
    * Definizione Lock
    */
    pthread_mutex_t lock;
    /*
    * Definizione semaforo
    */
    sem_t donneIn;
    sem_t uominiIn;
    int sospDonne;
    int sospUomini;
    /*
    * Definizione contatori
    */
    int countDonneIn;
    int countUominiIn;
    int MAX;
} bagno;

/*
* Definizione variabili
*/

bagno B;

/*
* Dichiarazione funzioni
*/

void init();

#endif
