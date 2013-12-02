#include "Lib.h"

void signalAll(){
    int i;
    for(i=0; i < MAX_GRUPPO; i++){
        pthread_cond_broadcast(&ps.codaP_IN[i]);
        pthread_cond_broadcast(&ps.codaE_IN[i]);
        pthread_cond_broadcast(&ps.codaP_OUT[i]);
    }
}

/*
* Ritorna se la pista è piena o meno
*/
bool isFull(){
    if ( ps.P_inPista + ps.E_inPista + ps.istr_inPista >= MAX_PISTA){
        return true;
    } else {
        return false;
    }
}

/*
* Cerca se c'è un gruppo di principianti con priorità maggiore in entrata
* Ritorna: true o false a seconda del risultato
*/
bool hasMorePrioSospP_IN(int num_gruppo){
    int i;
    for(i=num_gruppo; i >= 0; i--){
        if(ps.sosp_codaP_IN[i] > 0){
            return true;
        }
    }
    return false;
}

/*
* Cerca se c'è un gruppo di esperti con priorità maggiore in entrata
* Ritorna: true o false a seconda del risultato
*/
bool hasMorePrioSospE_IN(int num_gruppo){
    int i;
    for(i=num_gruppo; i >= 0; i--){
        if(ps.sosp_codaE_IN[i] > 0){
            return true;
        }
    }
    return false;
}

/*
* Cerca se c'è un gruppo di principianti con priorità maggiore in uscita
* Ritorna: true o false a seconda del risultato
*/
bool hasMorePrioSospP_OUT(int num_gruppo){
    int i;
    for(i=num_gruppo; i < MAX_GRUPPO; i++){
        if(ps.sosp_codaP_OUT[i] > 0){
            return true;
        }
    }
    return false;
}

/*
* Ritorna il numero dei principianti sospesi in entrata
*/
int hasSospP_IN(){
    int sum=0, i;
    for(i=0; i < MAX_GRUPPO; i++){
        sum += ps.sosp_codaP_IN[i];
    }
    return sum;
}

/*
* Ritorna il numero degli esperti sospesi in entrata
*/
int hasSospE_IN(){
    int sum=0, i;
    for(i=0; i < MAX_GRUPPO; i++){
        sum += ps.sosp_codaE_IN[i];
    }
    return sum;
}

/*
* Ritorna il numero dei principianti sospesi in uscita
*/
int hasSospP_OUT(){
    int sum=0, i;
    for(i=0; i < MAX_GRUPPO; i++){
        sum += ps.sosp_codaP_OUT[i];
    }
    return sum;
}

/*
* Thread gruppo esperti
*/
void gruppoE(int n_gruppo, int tid){
    {
        //Entrata in pista esperto
        pthread_mutex_lock (&ps.lock);
        /*
        * Possibilità di sospensione se:
        * - Ci sono principianti in attesa di entrare
        * - Ci sono gruppi di esperti meno numerosi
        * - La pista è piena
        * - P >= E non è più rispettato
        */
        while( hasSospP_IN() || hasMorePrioSospE_IN(n_gruppo) || isFull() || ps.P_inPista < ps.E_inPista + n_gruppo ){
            ps.sosp_codaE_IN[n_gruppo]++;
            pthread_cond_wait(&ps.codaE_IN[n_gruppo], &ps.lock);
            ps.sosp_codaE_IN[n_gruppo]--;
        }
        ps.E_inPista += n_gruppo;
        pthread_mutex_unlock (&ps.lock);
        signalAll();
    }
    printf("Thread %d - %d ESPERTI: Entrato in pista\n", tid, n_gruppo);
    sleep (rand() % 3);
    {
        //Uscita dalla pista esperto
        pthread_mutex_unlock (&ps.lock);
        /*
        * Nessuna condizione per l'uscita
        */
        ps.E_inPista -= n_gruppo;
        pthread_mutex_unlock (&ps.lock);
        signalAll();
    }
    printf("Thread %d - %d ESPERTI: Uscito dalla pista\n", tid, n_gruppo);
}

/*
* Thread gruppo principianti
*/
void gruppoP(int n_gruppo, int tid){
    {
        //Entrata in pista principiante
        pthread_mutex_lock (&ps.lock);
        /*
        * Possibilità di sospensione se:
        * - Ci sono gruppi di principianti meno numerosi
        * - La pista è piena
        * - P >= E non è più rispettato
        */
        while( hasMorePrioSospP_IN(n_gruppo) || isFull() || ps.P_inPista +n_gruppo < ps.E_inPista ){
            ps.sosp_codaP_IN[n_gruppo]++;
            pthread_cond_wait(&ps.codaP_IN[n_gruppo], &ps.lock);
            ps.sosp_codaP_IN[n_gruppo]--;
        }
        ps.P_inPista += n_gruppo;
        ps.istr_inPista++;
        pthread_mutex_unlock (&ps.lock);
        signalAll();
    }
    printf("Thread %d - %d PRINCIPIANTI: Entrato in pista\n", tid, n_gruppo);
    sleep (rand() % 5);
    {
        //Uscita dalla pista principiante
        pthread_mutex_lock (&ps.lock);
        /*
        * Possibilità di sospensione se:
        *  - Ci sono gruppi di principianti più numerosi
        *  - P >= E non è più rispettato
        *  - Ci sono esperti in attesa di entrare (previene deadlock)
        */
        while( hasMorePrioSospP_OUT(n_gruppo) || ps.P_inPista - n_gruppo < ps.E_inPista ){
            ps.sosp_codaP_OUT[n_gruppo]++;
            pthread_cond_wait(&ps.codaP_OUT[n_gruppo], &ps.lock);
            ps.sosp_codaP_OUT[n_gruppo]--;
        }

        ps.P_inPista -= n_gruppo;
        ps.istr_inPista--;
        pthread_mutex_unlock (&ps.lock);
        signalAll();
    }
    printf("Thread %d - %d PRINCIPIANTI: Uscito dalla pista\n", tid, n_gruppo);
}

/*
* Funzione thread
*/
void* thread_work(void* t){
    int tid = (intptr_t) t;

    /*
    * Esecuzione thread
    */
    //printf("Element: %d\n", tid);
    srand(time(NULL) + tid);

    if(tid % 2 == 0){
        gruppoE(rand() % MAX_GRUPPO, tid);
    } else {
        gruppoP(rand() % MAX_GRUPPO, tid);
    }

    pthread_exit((void*) (long) tid);
}

int main(int argc, char * argv[]){

    int i, ret_code;
    //int element = 3;

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
    //printf("Creazione threads completata...\n");

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

    printf("Exit...");
    return EXIT_SUCCESS;
}
