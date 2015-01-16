#ifndef __EserciziSistemiOperativi__Template__Semafori__H
#define __EserciziSistemiOperativi__Template__Semafori__H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

//DEFINIZIONI
#define N_VISITATORI 10
#define N_PERSONE_GRUPPO 10
#define MAX_DISPOSITIVI 20


//DICHIARAZIONI TIPI

typedef struct
{
    sem_t visitatore_servito;
    sem_t visitatore_arrivato;
    sem_t inizio_visita;
    sem_t visitatore_da_riconsegnare;
    sem_t visitatore_riconsegnato;
    
    pthread_mutex_t lock;
    
    pthread_cond_t dispositivi_disponibili;
    int n_dispositivi_disponibili;
    
    int visita_forzata;
    int visitatori_serviti;
    int visitatori_entrati;
} Museo;


//DICHIARAZIONI VARIABILI
pthread_t visitatori[N_VISITATORI];
pthread_t museo_t;
Museo museo;


//FUNZIONI

void init(Museo *);



//THREADS
void * visitatore_thread(void *);
void * museo_thread(void *);

#endif
