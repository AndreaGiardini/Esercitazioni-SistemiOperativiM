/*
* Si consideri la mensa aziendale di una piccola ditta.
* Dall’ora di inizio della “pausa pranzo” in poi, ogni dipendente dell’azienda si reca nel
* locale della mensa e consuma il proprio pasto, composto (per tutti) da:
 
* • un primo piatto (uguale per tutti)
* • un secondo piatto (uguale per tutti).
 
* La mensa è organizzata a “self-service” ed è gestita da un cuoco che deposita i piatti
* su un bancone dal quale i dipendenti possono prelevarli.
* Si assuma che:
 
* • il numero di dipendenti sia noto a priori e pari alla costante N;
* • il bancone sia di dimensioni limitate pari a MAX, che esprime il numero  massimo di piatti che possono essere contemporaneamente depositati su di esso (MAX<N).
* • ogni dipendente prelevi un piatto alla volta dal bancone;
* • ogni dipendente consumi prima il primo e poi il secondo piatto.
* • Il cuoco depositi sul bancone un piatto alla volta
* • Il cuoco depositi prima tutti i primi piatti;
* • prima di depositare i secondi piatti sul bancone, il cuoco attenda che tutti i primi piatti siano stati consumati.
 
* Si realizzi un’applicazione concorrente in c/linuxthreads che rappresenti cuoco e
* dipendenti mediante thread concorrenti e che rispetti i vincoli dati mediante
* un’opportuna politica di sincronizzazione realizzata tramite semafori.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

//macro
#define N_DIPENDENTI 100 //numero dipendenti
#define MAX_PIATTI 60 //piatti massimi che possono stare sul tavolo

//tipi
typedef struct
{
    pthread_mutex_t lock;
    sem_t barrier;
} Barriera;

//definizioni
Barriera barriera;
pthread_t dipendenti[N_DIPENDENTI];
pthread_t cuoco;

//threads
void * cuoco_thread(void * t)
{
    return NULL;
}

void * dipendente_thread(void * t)
{
    return NULL;
}

//utils
void init()
{

}

int main(int argc, char * argv[])
{
    printf("Programma mensa avviato\n");
    int result;
    void * status;
    
    init();
    
    
    for(int i=0; i < N_DIPENDENTI; i++)
    {
        result = pthread_create(&dipendenti[i], NULL, dipendente_thread, (void *)(intptr_t) i);
        if(result)
        {
            perror("Errore nella creazione dei dipendenti\n");
            exit(-1);
        }
    }
    
    pthread_create(&cuoco, NULL, cuoco_thread, NULL);
    
    for(int i=0; i < N_DIPENDENTI; i++)
    {
        result = pthread_join(dipendenti[i], &status);
        if(result)
        {
            perror("Errore nella join\n");
            exit(-1);
        }
    }
    
    pthread_join(cuoco, &status);
    
    return 0;
}