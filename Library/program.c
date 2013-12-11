#include "Lib.h"

void* thread_work(void* t){
    int tid = (intptr_t) t;

    /*
    * Esecuzione thread
    */
    //printf("Tid: %d\n", tid);

    pthread_exit((void*)(intptr_t) tid);
}

int main(int argc, char * argv[]){

    int i, ret_code;

    void* status;
    pthread_t thread[NUM_THREADS];

    /*
    * Inizializzazione risorse
    */
    init();

    /*
    * Creazione threads
    */
    for(i=0; i < NUM_THREADS; i++){
        ret_code=pthread_create(&thread[i], NULL, thread_work, (void *) (intptr_t) i);
        if (ret_code) { 
            printf("ERRORE: %d\n", ret_code); 
            exit(EXIT_FAILURE);
        }
    }

    /*
    * Join threads
    */
    for(i=0; i < NUM_THREADS; i++){
        ret_code=pthread_join(thread[i], &status);
        if (ret_code){
            printf("ERRORE join thread %d codice %d\n", i, ret_code); 
        }
        //printf("Finito thread con ris. %d\n",(int)(intptr_t)status);
    }


    return EXIT_SUCCESS;
}
