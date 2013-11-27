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

#define NUM_THREADS 5
#define K 10

//Boolean
typedef enum { false, true } boolean;

//Barriera

typedef struct{
    sem_t mb;
    sem_t sb; 
    int arrivati;
} barriera;

//Sondaggio per ogni film
typedef struct{
    /*
    * Definizione Lock
    */
    pthread_mutex_t lock;
    /*
    * Voto di ogni utente
    */
    int voti[NUM_THREADS];
} sondaggio;

/*
* Definizione variabili
*/

//Barriera
barriera B;
//Creo un sondaggio per ogni film
sondaggio sondaggi[K];
//Semaforo visione film
pthread_mutex_t vediFilm;
//Film vincitore
extern int winner;

/*
* Dichiarazione funzioni
*/

void init();
void sync_barriera();
void stampaVoti();

#endif
