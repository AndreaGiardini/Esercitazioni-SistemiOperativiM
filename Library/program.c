#include "Lib.h"

void* thread_work(void* t){
    int* elements = (int*) t;
    long result= *elements;

    /*
    * Esecuzione thread
    */
    //printf("Element: %d\n", *elements);

    pthread_exit((void*) result);
}

int main(int argc, char * argv[]){

    int i, ret_code;
    int element = 3;

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
        ret_code=pthread_create(&thread[i], NULL, thread_work, (void*)&element);
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
        //printf("Finito thread con ris. %ld\n",(long)status);
    }


    return EXIT_SUCCESS;
}
