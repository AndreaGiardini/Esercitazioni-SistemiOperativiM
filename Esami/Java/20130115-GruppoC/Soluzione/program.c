#include "Lib.h"

boolean piuPrioS_OUT(int n_gruppo){
    int i;
    for(i=n_gruppo; i < MAX_GRUPPO; i++){
        if(S.sospCodaS_OUT[i] > 0){
            return true;
        }
    }
    return false;
}

boolean piuPrioS_IN(int n_gruppo){
    int i;
    for(i=n_gruppo-1; i >= 0; i--){
        if(S.sospCodaS_IN[i] > 0){
            return true;
        }
    }
    return false;
}

boolean piuPrioF_OUT(int n_gruppo){
    int i;
    for(i=n_gruppo; i < MAX_GRUPPO; i++){
        if(S.sospCodaF_OUT[i] > 0){
            return true;
        }
    }
    return false;
}

boolean piuPrioF_IN(int n_gruppo){
    int i;
    for(i=n_gruppo-1; i >= 0; i--){
        if(S.sospCodaF_IN[i] > 0){
            return true;
        }
    }
    return false;
}

void thread_FUN(int tid, int n_gruppo){
    /*
    * ENTRATA SPOGLIATOIO
    *  - C'è inserviente
    *  - C'è posto nello spogliatoio
    *  - Non ci sono gruppi meno numerosi
    */
    pthread_mutex_lock(&S.lock);
    {
        while(!S.inservienteIn || spogliatoioPieno() || piuPrioF_IN(n_gruppo)){
            S.sospCodaF_IN[n_gruppo]++;
            pthread_cond_wait(&S.codaF_IN[n_gruppo], &S.lock);
            S.sospCodaF_IN[n_gruppo]--;
        }
        S.cabineOccupate++;
       printf("Thread: %d - Persone: %d - Esecuzione FUN - ENTRATA spogliatoio\n", tid, n_gruppo);
    }
    pthread_mutex_unlock(&S.lock);
    /*
    * ENTRATA FUN
    *  - C'è posto nell'area FUN
    */
    pthread_mutex_lock(&S.lock);
    {
        while(FUNPieno()){
            pthread_cond_wait(&S.codaInFUN, &S.lock);
        }
        S.inFUN += n_gruppo;
        S.cabineOccupate--;
        printf("Thread: %d - Persone: %d - Esecuzione FUN - ENTRATA Fun\n", tid, n_gruppo);
    }
    pthread_mutex_unlock(&S.lock);

    /*
    * USCITA FUN
    *  - C'è inserviente
    *  - C'è posto nello spogliatorio
    *  - Non ci sono clienti SPA in attesa di uscire
    *  - Non ci sono gruppi più numerosi
    */
    pthread_mutex_lock(&S.lock);
    {
        while (!S.inservienteIn || spogliatoioPieno() || S.sospCodaS_OUT || piuPrioF_OUT(n_gruppo)){
            S.sospCodaF_OUT[n_gruppo]++;
            pthread_cond_wait(&S.codaF_OUT[n_gruppo], &S.lock);
            S.sospCodaF_OUT[n_gruppo]--;
        }
        S.inFUN -= n_gruppo;
        printf("Thread: %d - Persone: %d - Esecuzione FUN - USCITA Fun\n", tid, n_gruppo);
    }
    pthread_mutex_unlock(&S.lock);

    /*
    * USCITA SPOGLIATORIO (sempre possibile)
    */
    pthread_mutex_lock(&S.lock);
    {
        S.cabineOccupate--;
        printf("Thread: %d - Persone: %d - Esecuzione FUN - USCITA spogliatoio\n", tid, n_gruppo);
    }
    pthread_mutex_unlock(&S.lock);
}

void thread_SPA(int tid, int n_gruppo){
    /*
    * ENTRATA SPOGLIATOIO
    *  - C'è inserviente
    *  - C'è posto nello spogliatoio
    *  - Non ci sono clienti FUN in attesa di entrare
    *  - Non ci sono gruppi meno numerosi
    */
    pthread_mutex_lock(&S.lock);
    {
        while(!S.inservienteIn || spogliatoioPieno() || S.sospCodaF_IN || piuPrioS_IN(n_gruppo)){
            S.sospCodaS_IN[n_gruppo]++;
            pthread_cond_wait(&S.codaS_IN[n_gruppo], &S.lock);
            S.sospCodaS_IN[n_gruppo]--;
        }
        S.cabineOccupate++;
        printf("Thread: %d - Persone: %d - Esecuzione SPA - ENTRATA spogliatoio\n", tid, n_gruppo);
    }
    pthread_mutex_unlock(&S.lock);

    /*
    * ENTRATA SPA
    *  - C'è posto nell'area SPA
    */
    pthread_mutex_lock(&S.lock);
    {
        while(SPAPieno()){
            pthread_cond_wait(&S.codaInSPA, &S.lock);
        }
        S.inSPA += n_gruppo;
        S.cabineOccupate--;
        printf("Thread: %d - Persone: %d - Esecuzione SPA - ENTRATA Spa\n", tid, n_gruppo);
    }
    pthread_mutex_unlock(&S.lock);

    /*
    * USCITA SPA
    *  - C'è inserviente
    *  - C'è posto nello spogliatorio
    *  - Non ci sono gruppi più numerosi
    */
    pthread_mutex_lock(&S.lock);
    {
        while(!S.inservienteIn || spogliatoioPieno() || piuPrioS_OUT(n_gruppo)){
            S.sospCodaS_OUT[n_gruppo]++;
            pthread_cond_wait(&S.codaS_OUT[n_gruppo], &S.lock);
            S.sospCodaS_OUT[n_gruppo]--;
        }
        S.inSPA -= n_gruppo;
        S.cabineOccupate++;
        printf("Thread: %d - Persone: %d - Esecuzione SPA - USCITA Spa\n", tid, n_gruppo);
    }
    pthread_mutex_unlock(&S.lock);

    /*
    * USCITA SPOGLIATORIO (sempre possibile)
    */
    pthread_mutex_lock(&S.lock);
    {
        S.cabineOccupate--;
        printf("Thread: %d - Persone: %d - Esecuzione SPA - USCITA spogliatoio\n", tid, n_gruppo);
    }
    pthread_mutex_unlock(&S.lock);
}

void thread_Inserviente(){
    int i;
    while ( i < 10){
        pthread_mutex_lock(&S.lock);
        {
            S.inservienteIn=true;
        }
        pthread_mutex_unlock(&S.lock);
        printf("LOLLLLL");
        printf("ENTRATA INSERVIENTE\n");
        sleep(5);

        pthread_mutex_lock(&S.lock);
        {
            S.inservienteIn=false;
        }
        pthread_mutex_unlock(&S.lock);

        printf("USCITA INSERVIENTE\n");
        sleep(5);
        i++;
    }
}

void* thread_work(void* t){
    int tid = (intptr_t) t;
    int n_gruppo;
    /*
    * Esecuzione thread
    */
    srand(time(NULL));
    n_gruppo = rand() % MAX_GRUPPO;

    if (tid == 0){
        thread_Inserviente();
    }else if(tid % 2 == 0){
        thread_FUN(tid, n_gruppo);
    } else {
        thread_SPA(tid, n_gruppo);
    }

    pthread_exit(0);
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
        ret_code=pthread_create(&thread[i], NULL, thread_work, (void*) (intptr_t) i);
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
