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
#define NI 5
#define MAX_PISTA 25
#define MAX_GRUPPO 10

//Boolean
typedef enum { false, true } bool;

//Struttura condivisa
typedef struct{
    /*
    * Definizione Lock
    */
    pthread_mutex_t lock;
    /*
    * Definizione condizione (code);
    */
	pthread_cond_t codaP_IN[MAX_GRUPPO];
    pthread_cond_t codaE_IN[MAX_GRUPPO];
    pthread_cond_t codaP_OUT[MAX_GRUPPO];

    //Stato delle code
    int sosp_codaP_IN[MAX_GRUPPO];
    int sosp_codaE_IN[MAX_GRUPPO];
    int sosp_codaP_OUT[MAX_GRUPPO];

    //Stato della pista
    int P_inPista;
    int E_inPista;
    int istr_inPista;
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
