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
#define N_DIPENDENTI 10 //numero dipendenti
#define MAX_PIATTI 6 //piatti massimi che possono stare sul tavolo
#define MAX_TEMPO_PREPARAZIONE 2
#define MAX_TEMPO_CONSUMAZIONE 3

//definizioni
pthread_t dipendenti[N_DIPENDENTI];
pthread_t cuoco;

typedef struct
{
    int primi_piatti_consumati;
    sem_t primo_piatto, secondo_piatto, seconda_fase, posto_libero;
    pthread_mutex_t lock;
    
} Mensa;

Mensa mensa;

//threads
void * cuoco_thread(void * t)
{
    unsigned int tempo_di_preparazione;
    /*
     * [Comportamento del cuoco]
     * 1) Mettere tutti i primi piatti sul tavolo senza avere PIATTI > MAX
     * 2) Aspettare che tutti i dipendenti finiscano i primi piatti
     * 3) Mettere tutti i secondi piatti sul tavolo senza avere PIATTI > MAX
     * 4) Aspettare che tutti i dipendenti finiscano i secondi piatti
     */
    for(int i=0; i < N_DIPENDENTI; i++)
    {
        sem_wait(&mensa.posto_libero);
    
        tempo_di_preparazione = rand() % MAX_TEMPO_PREPARAZIONE;

        printf("[Cuoco] Preparo un nuovo primo piatto\n");
        sleep(tempo_di_preparazione);

        sem_post(&mensa.primo_piatto);
    }
    
    sem_wait(&mensa.seconda_fase);
    
    for(int i=0; i < N_DIPENDENTI; i++)
    {
        sem_wait(&mensa.posto_libero);
        
        tempo_di_preparazione = rand() % MAX_TEMPO_PREPARAZIONE;
        
        printf("[Cuoco] Preparo un nuovo secondo piatto\n");
        sleep(tempo_di_preparazione);
        
        sem_post(&mensa.secondo_piatto);
    }
    
    printf("[Cuoco] finito!\n");
    
    pthread_exit(NULL);
}

void * dipendente_thread(void * t)
{
    /*
     * [Comportamento del dipendente]
     * 1) Aspetto che sia presente un primo piatto sul bancone
     * 2) Prelevo primo piatto dal bancone
     * 3) Consumo primo piatto
     * 4) Aspetto che sia presente un secondo piatto sul bancone
     * 5) Consumo secondo piatto
     */
    
    unsigned int tempo_di_consumazione;
    int tid = (int)(intptr_t) t;
    
    sem_wait(&mensa.primo_piatto);
    
    tempo_di_consumazione = rand() % MAX_TEMPO_CONSUMAZIONE;
    printf("[Dipendente %d] consumo primo piatto\n", tid);
    sleep(tempo_di_consumazione);
    
    //CONSUMO PRIMO PIATTO
    pthread_mutex_lock(&mensa.lock);
    
    mensa.primi_piatti_consumati++;
    
    printf("[Dipendente %d] finito primo piatto\n", tid);
    
    sem_post(&mensa.posto_libero);
    
    if(mensa.primi_piatti_consumati == N_DIPENDENTI)
    {
        printf("[Dipendente %d] sono l'ultimo a finire il primo piatto! Si dia il via ai secondi piatti!\n", tid);
        sem_post(&mensa.seconda_fase);
    }
    pthread_mutex_unlock(&mensa.lock);

    sem_wait(&mensa.secondo_piatto);
    
    printf("[Dipendente %d] consumo secondo piatto\n", tid);
    sleep(tempo_di_consumazione);
    
    //CONSUMO SECONDO PIATTO
    sem_post(&mensa.posto_libero);
    
    printf("[Dipendente %d] grazie del pranzo\n", tid);
    
    return NULL;
}

//utils
void init(Mensa *m)
{
    sem_init(&m->posto_libero, 0, MAX_PIATTI);
    sem_init(&m->primo_piatto, 0, 0);
    sem_init(&m->seconda_fase, 0, 0);
    sem_init(&m->secondo_piatto, 0, 0);
    pthread_mutex_init(&m->lock, NULL);
}

int main(int argc, char * argv[])
{
    printf("Programma mensa avviato\n");
    int result;
    void * status;
    
    init(&mensa);
    
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