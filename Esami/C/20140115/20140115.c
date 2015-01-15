//
//  Template.c
//
//  Created by Marco Moschettini
//
//  Compilare con
//  gcc -D_REENTRANT -std=c99 -o Esame Esame.c -lpthread
//

#include "20140115.h"

void * visitatore_thread(void * t)
{
    int tid = (int)(intptr_t) t;
    printf("[Visitatore %d] arrivato al museo\n", tid);

    sem_wait(&museo.visitatore_servito);
    pthread_mutex_lock(&museo.lock);
    
    while(museo.n_dispositivi_disponibili == 0)
        pthread_cond_wait(&museo.dispositivi_disponibili, &museo.lock);
    
    museo.n_dispositivi_disponibili--;
    pthread_mutex_unlock(&museo.lock);
    
    printf("[Visitatore %d] effettuato il check-in\n", tid);
    
    sem_post(&museo.visitatore_arrivato);
    
    sleep(1);
    
    pthread_mutex_lock(&museo.lock);
    
    if(museo.visitatori_serviti == N_VISITATORI)
    {
        printf("[Visitatore %d] Sono l'ultimo. Inizia la visita\n", tid);
        sem_post(&museo.inizio_visita);
    }
    
    pthread_mutex_unlock(&museo.lock);
    
    sem_wait(&museo.inizio_visita);
    sem_post(&museo.inizio_visita);
    sleep(3);
    printf("[Visitatore %d] Visita finita. Riconsegno il dispositivo.\n", tid);
    
    sem_wait(&museo.visitatore_riconsegnato);
    printf("[Visitatore %d] Dispositivo riconsegnato.\n", tid);
    sem_post(&museo.visitatore_da_riconsegnare);
    
    printf("[Visitatore %d] Esco dal museo.\n", tid);

    pthread_exit(NULL);
}

void * museo_thread(void * t)
{
    for(int i=0; i < N_VISITATORI; i++)
    {
        sem_wait(&museo.visitatore_arrivato);
        
        printf("[Museo] servito visitatore %d\n", i);
        pthread_mutex_lock(&museo.lock);
        museo.visitatori_serviti++;
        pthread_mutex_unlock(&museo.lock);
        sleep(1);
        sem_post(&museo.visitatore_servito);
        
        printf("[Museo] visitatori serviti: %d, dispositivi disponibili: %d\n", museo.visitatori_serviti, museo.n_dispositivi_disponibili);
    }
    
    printf("[Museo] Ha inizio la visita\nAttendo che finisca\n");
    
    for(int i=0; i < N_VISITATORI; i++)
    {
        sem_wait(&museo.visitatore_da_riconsegnare);
        
        printf("[Museo] riconsegnato visitatore %d\n", i);
        sleep(1);
        sem_post(&museo.visitatore_riconsegnato);
        
        printf("[Museo] visitatori serviti: %d, dispositivi disponibili: %d\n", museo.visitatori_serviti, museo.n_dispositivi_disponibili);
    }
    
    printf("[Museo] giornata terminata. Chiusura\n");
    pthread_exit(NULL);
}


void init(Museo * m)
{
    sem_init(&m->inizio_visita, 0, 0);
    
    sem_init(&m->visitatore_servito, 0, 1);
    sem_init(&m->visitatore_arrivato, 0, 0);
    
    sem_init(&m->visitatore_riconsegnato, 0, 1);
    sem_init(&m->visitatore_da_riconsegnare, 0, 0);

    pthread_mutex_init(&m->lock, NULL);
    
    pthread_cond_init(&museo.dispositivi_disponibili, NULL);
    
    
    m->n_dispositivi_disponibili = MAX_DISPOSITIVI;
    m->visitatori_serviti = 0;
}


int main(int argc, char *  argv[])
{
    int result;
    void * status;
    
    printf("Programma avviato.\n");
    init(&museo);
    
    for(int i = 0; i < N_VISITATORI; i++)
    {
        result = pthread_create(&visitatori[i], NULL, visitatore_thread, (void *)(intptr_t) i);
        if(result)
        {
            printf("Errore nella creazione del thread %d con stato %d\n", i, result);
            exit(-1);
        }
    }
    
    //avvio di ulteriori threads
    result = pthread_create(&museo_t, NULL, museo_thread, NULL);
    if(result)
    {
        printf("Errore nella creazione del thread Museo con stato %d\n", result);
        exit(-1);
    }
    
    //join dei threads
    for(int i = 0; i < N_VISITATORI; i++)
    {
        result = pthread_join(visitatori[i], status);
        if(result)
        {
            printf("Errore nel join del thread %d con stato %d e risultato %d\n", i, result, (int)(intptr_t)status);
        }
    }
    
    result = pthread_join(museo_t, status);
    if(result)
    {
        printf("Errore nel join del thread %d con stato Museo e risultato %d\n", result, (int)(intptr_t)status);
    }
    
    printf("Fine del programma.\n");
}