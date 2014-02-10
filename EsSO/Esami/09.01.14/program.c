#include "Lib.h"

void threadSenior(int tid){
    pthread_mutex_lock(&CB.lock);
    do{
        /*
        * Tenta di entrare in cabina. Rifiutato se:
        *  - La cabina è occupata
        *  - Ci sono persone in attesa di uscire
        *  - La banca è piena
        */
        while(CB.cabinaOccupata == true || CB.sosp[SENIOR][USCITA] > 0 || CB.sosp[STANDARD][USCITA] > 0 || bancaPiena()){
            CB.sosp[SENIOR][ENTRATA]++;
            pthread_cond_wait(&CB.coda[SENIOR][ENTRATA], &CB.lock);
            CB.sosp[SENIOR][ENTRATA]--;
        }
        sleep(1);

        /*
        * Dentro la cabina, in attesa di entrare
        * Se il metaldetector mi rifiuta ritorno da capo
        * - Se il metal detector mi rifiuta (false) devo ritornare dentro
        */
    } while (!metalDetector())

    cabinaOccupata = true;
    CB.inBanca++;
    pthread_mutex_unlock(&CB.lock);

    /*
    * Permanenza in banca
    */
    sleep(3);

    /*
    * Uscita dalla banca
    */
    pthread_mutex_lock(&CB.lock);
    while(CB.cabinaOccupata == true){
        CB.sosp[SENIOR][USCITA]++;
        pthread_cond_wait(&CB.coda[SENIOR][USCITA], &CB.lock);
        CB.sosp[SENIOR][USCITA]--;
    }
    CB.
    pthread_mutex_unlock(&CB.lock);
}

void threadStandard(int tid){
    pthread_mutex_lock(&CB.lock);
    
    pthread_mutex_unlock(&CB.lock);
}

void* thread_work(void* t){
    int tid = (intptr_t) t;

    /*
    * Esecuzione thread
    */
    srand(time(NULL)+tid);
    if(rand() % 2 == 0){
        threadSenior(tid);
    } else {
        threadStandard(tid);
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
