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
#define MaxG 10
#define MAXF 10

#define T1 0
#define T2 1
#define STANDARD 0
#define SPECIALI 1
#define TEMPBASSA 0
#define TEMPALTA 1

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
	pthread_cond_t coda[2][2][MaxG];
	int sosp[2][2][MaxG];

    int inForno;
    int temperatura;
} forno;

/*
* Definizione variabili
*/

forno F;

/*
* Dichiarazione funzioni
*/

void init();

#endif
