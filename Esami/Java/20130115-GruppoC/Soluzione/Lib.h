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

//Numero massimo di persone per gruppo
#define MAX_GRUPPO 6

#define NUM_THREADS 10
//Numero massimo nell'area Fun
#define MAXF 10
//Numero massimo nell'area Spa
#define MAXS 10
//Numero cabine
#define NC 5

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
	pthread_cond_t codaF_IN[MAX_GRUPPO];
    pthread_cond_t codaF_OUT[MAX_GRUPPO];
    pthread_cond_t codaS_IN[MAX_GRUPPO];
    pthread_cond_t codaS_OUT[MAX_GRUPPO];

    int sospCodaF_IN[MAX_GRUPPO];
    int sospCodaF_OUT[MAX_GRUPPO];
    int sospCodaS_IN[MAX_GRUPPO];
    int sospCodaS_OUT[MAX_GRUPPO];

    int cabineOccupate;

    pthread_cond_t codaInSPA;
    pthread_cond_t codaInFUN;
    int inSPA;
    int inFUN;

    boolean inservienteIn;

} spogliatoio;

/*
* Definizione variabili
*/

extern spogliatoio S;

/*
* Dichiarazione funzioni
*/

void init();
boolean spogliatoioPieno();
boolean SPAPieno();
boolean FUNPieno();

#endif
