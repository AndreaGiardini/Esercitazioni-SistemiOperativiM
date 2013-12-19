#include "Lib.h"

/*
* Inizializzazione variabili
*/
void init(){
	/*
	* Mutex Init
	*/
	//pthread_mutex_init (&BANC.lock, NULL);

    /*
    * Semaphore Init (ultimo campo: valore iniziale)
    */
    sem_init(&BANC.semDepositoPrimi, 0, MAX);
    sem_init(&BANC.semPrelievoPrimi, 0, 0);
    sem_init(&BANC.semDepositoSecondi, 0, MAX);
    sem_init(&BANC.semPrelievoSecondi, 0, 0);

    /*
    * Barriera Init
    */
    sem_init(&B.mb,0,1);
    sem_init(&B.sb,0,0);
    B.arrivati=0;
}

/*
* Sincronizzazione barriera
*/
void sync_barriera(){
    sem_wait(&B.mb);
    B.arrivati++; 
    if (B.arrivati==NUM_THREADS){
        sem_post(&B.sb);
    }
    sem_post(&B.mb);
    sem_wait(&B.sb); 
    sem_post(&B.sb);
    return;
}
