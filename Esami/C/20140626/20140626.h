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
#define N_PASSEGGERI 10
#define N_KIT N_PASSEGGERI
#define MAX_CESTO 20



//DICHIARAZIONI TIPI
typedef struct
{
    sem_t nuovo_passeggero_servito;
    sem_t nuovo_passeggero_arrivato;
    
    sem_t cesta_da_ritirare;
    sem_t cesta_ritirata;
    
    sem_t passeggero_deve_entrare;
    sem_t passeggero_entrato;
    
    //fasi
    sem_t fase_ceste;
    sem_t fase_finale;
    
    pthread_mutex_t lock;
    pthread_mutex_t cesta;
    
    pthread_cond_t cesta_piena;
    
    int passeggeri_controllati;
    int passeggeri_ceste;
    int passeggeri_entrati;
    
    int cesta_n;
    
} Aeroporto;

//DICHIARAZIONI VARIABILI
pthread_t passeggeri_threads[N_PASSEGGERI];
pthread_t addetto_thread;

Aeroporto aeroporto;


//FUNZIONI

void init(Aeroporto *);

//THREADS
void * addetto(void *);
void * passeggero(void *);

#endif
