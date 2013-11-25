#include "Lib.h"

void init(){
	/*
	* Mutex Init
	*/
	int i;
	for(i=0; i < K; i++){
	    pthread_mutex_init(&obj[i].lock, NULL);
	}

    /*
    * Semaphore Init (ultimo campo: valore iniziale)
    */
    //sem_init(&obj.sem, 0, 0);

    /*
    * Cond Init
    */
	//pthread_cond_init (&obj.coda, NULL);

    /*
    * Barriera Init
    */
    //sem_init(&B.mb,0,1);
    //sem_init(&B.sb,0,0);
    //B.arrivati=0;

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
