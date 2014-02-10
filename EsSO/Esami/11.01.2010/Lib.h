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

#define NUM_THREADS 20

#define N1 5
#define N2 5

#define MA 0
#define MI 1
#define C1 0
#define C2 1

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

    int C1inPista;
    int C2inPista;
} pista;

/*
* Definizione variabili
*/

pista ps;

/*
* Dichiarazione funzioni
*/

void init();

#endif
