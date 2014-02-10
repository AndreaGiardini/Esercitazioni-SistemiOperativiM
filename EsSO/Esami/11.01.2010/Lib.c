#include "Lib.h"

/*
* Inizializzazione variabili
*/
void init(){
	/*
	* Mutex Init
	*/
	pthread_mutex_init (&ps.lock, NULL);

    /*
    * Cond Init
    */
	pthread_cond_init (&ps.coda[MA][C2], NULL);
	pthread_cond_init (&ps.coda[MA][C1], NULL);
	pthread_cond_init (&ps.coda[MI][C2], NULL);
	//pthread_cond_init (&ps.coda[MA][C1], NULL);

    ps.sosp[MA][C2] = 0;
    ps.sosp[MA][C1] = 0;
    ps.sosp[MI][C2] = 0;

    ps.C1inPista = 0;
    ps.C2inPista = 0;

}
