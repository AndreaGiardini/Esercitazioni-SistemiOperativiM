#include "Lib.h"

void thread_cuoco(int tid){
    int i;
    //Serve i primi
    for(i=0; i < NUM_THREADS -1; i++){
        //Servo un primo
        sem_wait(&BANC.semDepositoPrimi);
        sem_post(&BANC.semPrelievoPrimi);
        printf("Sono il cuoco e ho servito un primo\n");
    }

    sync_barriera();

    for(i=0; i < NUM_THREADS -1; i++){
        //Servo un secondo
        sem_wait(&BANC.semDepositoSecondi);
        sem_post(&BANC.semPrelievoSecondi);
        printf("Sono il cuoco e ho servito un secondo\n");
    }
    //Serve i secondi
    printf("fine cuoco!\n");
}

void thread_cliente(int tid){

    sem_post(&BANC.semDepositoPrimi);
    sem_wait(&BANC.semPrelievoPrimi);
    printf("Prelevato un primo!\n");

    sleep(2);
    sync_barriera();

    sem_post(&BANC.semDepositoSecondi);
    sem_wait(&BANC.semPrelievoSecondi);
    printf("Prelevato un secondo!\n");

    sleep(2);
    printf("Cliente terminato!\n");
}


void* thread_work(void* t){
    int tid = (intptr_t) t;

    /*
    * Esecuzione thread
    */
    if(tid==0){
        thread_cuoco(tid);
    } else {
        thread_cliente(tid);
    }

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
