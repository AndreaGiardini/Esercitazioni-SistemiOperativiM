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
#define MAX_PONTE 4

//Boolean
typedef enum { false, true } boolean;
typedef enum { dx, sx } direzione;
typedef enum { magri, grassi } utente;

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
    int sospesi[2][2];

    int inTransito[2][2];

} ponte;

/*
* Definizione variabili
*/

ponte P;

/*
* Dichiarazione funzioni
*/

void init();
boolean ePieno();
char* getDirezione(direzione dir);
char* getUtente(utente usr);
direzione dirOpposta(direzione dir);
utente utenteOpposto(utente usr);

#endif
