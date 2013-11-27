#include "Lib.h"

void* thread_work(void* t){
    int elements = (intptr_t) t;
    int i, j;

    /*
    * Esecuzione thread
    */
    srand(time(NULL)+elements);
    for(i=0; i < K; i++){
        pthread_mutex_lock(&sondaggi[i].lock);
        /*
        * Per ogni sondaggio scrivo un voto
        * Devo cercare la prima cella libera in cui inserire il voto
        */
        for(j=0; j < NUM_THREADS; j++){
            if(sondaggi[i].voti[j] == 0){
                /*
                * Cella libera - Assegno il voto
                */
                sondaggi[i].voti[j] = rand() % 10 + 1;
                break;
            }
        }

        pthread_mutex_unlock(&sondaggi[i].lock);
    }

    /*
    * Barriera e visione film;
    */
    sync_barriera();
    pthread_mutex_lock(&vediFilm);
    printf("Thread id: %d - Visione Film %d...\n", elements, winner);
    sleep(2);
    pthread_mutex_unlock(&vediFilm);

    long result = elements;
    pthread_exit((void*) result);
}

int main(int argc, char * argv[]){

    int ret_code;
    int i;

    void* status;
    pthread_t thread[NUM_THREADS];

    /*
    * Inizializzazione risorse
    */
    printf("Inizializzazione...\n");
    init();
    printf("Inizializzazione completata\n");

    /*
    * Creazione threads
    */

    printf("Creazione threads...\n");
    for(i=0; i < NUM_THREADS; i++){
        ret_code=pthread_create(&thread[i], NULL, thread_work, (void *) (intptr_t) i);
        if (ret_code) { 
            printf("ERRORE: %d\n", ret_code); 
            exit(EXIT_FAILURE);
        }
    }
    printf("Creazione threads completata\n");

    /*
    * Join threads
    */
    printf("Join threads...\n");
    for(i=0; i < NUM_THREADS; i++){
        ret_code=pthread_join(thread[i], &status);
        if (ret_code){
            printf("ERRORE join thread %ld codice %d\n", i, ret_code); 
        }
    }

    printf("Exit\n");
    return EXIT_SUCCESS;
}
