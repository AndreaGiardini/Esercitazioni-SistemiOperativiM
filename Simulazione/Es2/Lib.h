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
#define MAX 3

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
    * Definizione semaforo
    */
    sem_t semDepositoPrimi;
    sem_t semPrelievoPrimi;
    sem_t semDepositoSecondi;
    sem_t semPrelievoSecondi;
} bancone;

/*
* Definizione variabili
*/

barriera B;
bancone BANC;

/*
* Dichiarazione funzioni
*/

void sync_barriera();
void init();

#endif
