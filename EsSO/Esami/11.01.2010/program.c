#include "Lib.h"

void segnala(){
    if(ps.sosp[MA][C1] > 0 || ps.sosp[MA][C2] > 0){
        pthread_cond_signal(&ps.coda[MA][C1]);
        pthread_cond_signal(&ps.coda[MA][C2]);
    } else {
        pthread_cond_signal(&ps.coda[MI][C2]);
    }
}

boolean C1nonDisponibili(){
    if (ps.C1inPista >= N1){
        return true;
    }
    return false;
}

boolean C2nonDisponibili(){
    if (ps.C2inPista >= N2){
        return true;
    }
    return false;
}

void thread_MA(int tid){
    /*
    * Posso noleggiare sia C1 che C2
    */

    pthread_mutex_lock(&ps.lock);

    if(C1nonDisponibili()){
        //MA noleggia C2
        /*
        * Si sospende se:
        *  - Ci sono C1 in pista
        *  - C2 non disponibili
        */
        while(ps.C1inPista > 0 || C2nonDisponibili()){
            ps.sosp[MA][C2]++;
            segnala();
            pthread_cond_wait(&ps.coda[MA][C2], &ps.lock);
            ps.sosp[MA][C2]--;
        }
        ps.C2inPista++;
        printf("Thread %d - MA - C2 - ENTRATO in pista\n", tid);
        pthread_mutex_unlock(&ps.lock);

        sleep(2);

        pthread_mutex_lock(&ps.lock);
        ps.C2inPista--;
        printf("Thread %d - MA - C2 - USCITO in pista\n", tid);
        pthread_mutex_unlock(&ps.lock);
    } else {
        //MA noleggia C1
        /*
        * Si sospende se:
        *  - Ci sono C2 in pista
        *  - C1 non disponibili
        */
        while(ps.C2inPista > 0 || C1nonDisponibili()){
            ps.sosp[MA][C1]++;
            segnala();
            pthread_cond_wait(&ps.coda[MA][C1], &ps.lock);
            ps.sosp[MA][C1]--;
        }
        ps.C1inPista++;
        printf("Thread %d - MA - C1 - ENTRATO in pista\n", tid);
        pthread_mutex_unlock(&ps.lock);

        sleep(2);

        pthread_mutex_lock(&ps.lock);
        ps.C1inPista--;
        printf("Thread %d - MA - C1 - USCITO in pista\n", tid);
        pthread_mutex_unlock(&ps.lock);
    }
    segnala();
}

void thread_MI(int tid){
    /*
    * Posso noleggiare solo C2
    * Mi sospendo se:
    *  - Non ci sono C2 disponibili
    *  - Ci sono C1 in pista
    *  - Ci sono MA in attesa
    */

    pthread_mutex_lock(&ps.lock);

    while(C2nonDisponibili() || ps.C1inPista > 0 || ps.sosp[MA][C1] > 0 || ps.sosp[MA][C2] > 0){
        ps.sosp[MI][C2]++;
        segnala();
        pthread_cond_wait(&ps.coda[MI][C2], &ps.lock);
        ps.sosp[MI][C2]--;
    }

    ps.C2inPista++;
    printf("Thread %d - MI - C2 - ENTRATO in pista\n", tid);
    pthread_mutex_unlock(&ps.lock);

    //Giro in pista
    sleep(2);

    //Uscita dalla pista
    pthread_mutex_lock(&ps.lock);
    ps.C2inPista--;
    printf("Thread %d - MI - C2 - USCITO in pista\n", tid);
    pthread_mutex_unlock(&ps.lock);

    segnala();
}

void* thread_work(void* t){
    int tid = (intptr_t) t;

    /*
    * Esecuzione thread
    */
    srand(time(NULL)+tid);
    if(rand() % 2 == 0){
        thread_MA(tid);
    } else {
        thread_MI(tid);
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
