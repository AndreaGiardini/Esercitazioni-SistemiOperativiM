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
    int i;
    for(i=0; i < MAX_GRUPPO; i++){
        pthread_cond_init (&ps.codaP_IN[i], NULL);
        pthread_cond_init (&ps.codaE_IN[i], NULL);
        pthread_cond_init (&ps.codaP_OUT[i], NULL);
        ps.sosp_codaP_IN[i] = 0;
        ps.sosp_codaE_IN[i] = 0;
        ps.sosp_codaP_OUT[i] = 0;
    }

    ps.P_inPista = 0;
    ps.E_inPista = 0;
    ps.istr_inPista = 0;

}