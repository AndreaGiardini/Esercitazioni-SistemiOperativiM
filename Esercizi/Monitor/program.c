#include "Lib.h"

void* thread_work(void* t){
    int tid = (intptr_t) t;

    /*
    * Esecuzione thread
    */
    srand(time(NULL)+tid);

    direzione dir = rand() % 2;
    utente tipo = rand() % 2;
    //printf("Creato thread %d - %s con direzione %s\n", tid, getUtente(tipo), getDirezione(dir));

    pthread_mutex_lock(&P.lock);

    if(tipo == magri){
        /*
        * Utenti magri
        *
        * Sospendono se:
        * - Ponte pieno
        * - Grassi in direzione opposta
        */
        while(ePieno() || P.inTransito[grassi][dirOpposta(dir)] > 0){
            P.sospesi[tipo][dir]++;
            pthread_cond_wait(&P.coda[tipo][dir], &P.lock);
            P.sospesi[tipo][dir]--;
        }

    } else {
        /*
        * Utenti grassi
        *
        * Sospendono se:
        * - Pinte pieno
        * - Grassi in direzione opposta
        * - Magri in attesa
        */
        while(ePieno() || P.inTransito[grassi][dirOpposta(dir)] > 0 || P.sospesi[magri][dir] > 0){
            P.sospesi[tipo][dir]++;
            pthread_cond_wait(&P.coda[tipo][dir], &P.lock);
            P.sospesi[tipo][dir]--;
        }

    }

    printf("Thread %d - %s con direzione %s - Entrato sul ponte\n", tid, getUtente(tipo), getDirezione(dir));
    P.inTransito[tipo][dir]++;

    //Risveglio thread
    risveglioThread(tipo, dir);
    pthread_mutex_unlock(&P.lock);

    //Permanenza
    sleep(2);

    //Uscita
    pthread_mutex_lock(&P.lock);
    P.inTransito[tipo][dir]--;
    printf("Thread %d - %s con direzione %s - Uscito dal ponte\n", tid, getUtente(tipo), getDirezione(dir));

    //Risveglio thread
    risveglioThread(tipo, dir);
    pthread_mutex_unlock(&P.lock);

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
