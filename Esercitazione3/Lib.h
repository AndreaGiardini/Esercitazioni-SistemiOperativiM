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

//Barriera
typedef struct{
    sem_t mb;
    sem_t sb; 
    int arrivati;
} barriera;

//Struttura condivisa
typedef struct{
    /*
    * Definizione Lock
    */
    //pthread_mutex_t lock;
    /*
    * Definizione semaforo
    */
    //sem_t sem;
    /*
    * Definizione condizione;
    */
	//pthread_cond_t coda;
} sharedObj;

/*
* Definizione variabili
*/

barriera B;
sharedObj obj;

/*
* Dichiarazione funzioni
*/

void initObj(sharedObj *obj);

#endif
