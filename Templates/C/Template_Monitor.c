//
//  Template.c
//
//  Created by Marco Moschettini
//
//  Compilare con
//  gcc -D_REENTRANT -std=c99 -o Esame Esame.c -lpthread
//

#include "Template_Monitor.h"

void * thread1(void * t)
{
    //Attendo che qualcuno segnali una condizione
    pthread_cond_wait(&monitor.condition1, &monitor.lock);
    
    pthread_mutex_lock(&monitor.lock);
    
    //SEZIONE CRITICA
    
    pthread_mutex_unlock(&monitor.lock);
    
    //Segnalo una condizione
    pthread_cond_signal(&monitor.condition1);
    
    
    
    pthread_exit(NULL);
}

void * thread2(void * t)
{
    int result = 42;
    
    pthread_exit((void *)(intptr_t) result);
}


void init_monitor(Monitor * monitor)
{
    pthread_mutex_init(&monitor->lock, NULL);
    pthread_cond_init(&monitor->condition1, NULL);
    pthread_cond_init(&monitor->condition2, NULL);
    
    monitor->n = 0;
}

int main(int argc, char *  argv[])
{
    int result;
    void * status;
    
    printf("Programma avviato.\n");
    
    init_monitor(&monitor);
    
    for(int i = 0; i < THREADS; i++)
    {
        result = pthread_create(&threads[i], NULL, thread1, (void *)(intptr_t) i);
        if(result)
        {
            printf("Errore nella creazione del thread %d con stato %d\n", i, result);
            exit(-1);
        }
    }
    
    //avvio di ulteriori threads
    
    //join dei threads
    for(int i = 0; i < THREADS; i++)
    {
        result = pthread_join(threads[i], status);
        if(result)
        {
            printf("Errore nel join del thread %d con stato %d e risultato %d\n", i, result, (int)(intptr_t)status);
        }
    }
    
    printf("Fine del programma.\n");
}