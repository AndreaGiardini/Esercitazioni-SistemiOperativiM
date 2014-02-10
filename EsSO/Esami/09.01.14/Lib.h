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

#define SENIOR 0
#define STANDARD 1
#define USCITA 0
#define ENTRATA 1

//Boolean
typedef enum { false, true } boolean;

//Struttura condivisa
typedef struct{
    /*
    * Definizione Lock
    */
    pthread_mutex_t lock;
    /*
    * Definizione condizione;
    */
	pthread_cond_t coda[2][2];
	int sosp[2][2];

    boolean cabinaOccupata;

    int inBanca, maxBanca;
} CabinaBanca;

/*
* Definizione variabili
*/

CabinaBanca CB;

/*
* Dichiarazione funzioni
*/

void init();
boolean bancaPiena();

#endif
