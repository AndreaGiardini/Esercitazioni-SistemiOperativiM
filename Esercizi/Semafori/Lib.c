#include "Lib.h"

/*
* Inizializzazione variabili
*/
void init(){
	/*
	* Mutex Init
	*/
	pthread_mutex_init (&B.lock, NULL);

    /*
    * Semaphore Init (ultimo campo: valore iniziale)
    */
    sem_init(&B.donneIn, 0, 0);
    sem_init(&B.uominiIn, 0, 0);
    B.sospDonne = 0;
    B.sospUomini = 0;

    B.countDonneIn = 0;
    B.countUominiIn = 0;
    B.MAX = 2;
}