//
//  Template.c
//
//  Created by Marco Moschettini
//
//  Compilare con
//  gcc -D_REENTRANT -std=c99 -o Esame Esame.c -lpthread
//

#include "Template_Semafori.h"

void * thread1(void * t)
{
    /*
    pthread_mutex_lock(&monitor.lock);
    
    //SEZIONE CRITICA
    
    pthread_mutex_unlock(&monitor.lock);
    */
    
    pthread_exit(NULL);
}

void * thread2(void * t)
{
    int result = 42;
    
    pthread_exit((void *)(intptr_t) result);
}

/*
void init(nuovo_tipo * t)
{
    sem_init(&t->semaforo1, 0, 0);
    sem_init(&t->semaforo2, 0, 0);
    pthread_mutex_init(&t->lock, NULL);
}
*/

int main(int argc, char *  argv[])
{
    int result;
    void * status;
    
    printf("Programma avviato.\n");
    
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