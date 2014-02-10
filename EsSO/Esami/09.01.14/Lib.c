#include "Lib.h"

/*
* Inizializzazione variabili
*/
void init(){
	/*
	* Mutex Init
	*/
	pthread_mutex_init (&CB.lock, NULL);

    /*
    * Cond Init
    */
	pthread_cond_init (&CB.coda[SENIOR][USCITA], NULL);
	pthread_cond_init (&CB.coda[SENIOR][ENTRATA], NULL);
	pthread_cond_init (&CB.coda[STANDARD][USCITA], NULL);
	pthread_cond_init (&CB.coda[STANDARD][ENTRATA], NULL);

    CB.sosp[SENIOR][USCITA] = 0;
    CB.sosp[SENIOR][ENTRATA] = 0;
    CB.sosp[STANDARD][USCITA] = 0;
    CB.sosp[STANDARD][ENTRATA] = 0;

    CB.cabinaOccupata = false;

    CB.inBanca = 0;
    CB.maxBanca = 5;

}

boolean metalDetector(){
    if(rand()%2==0){
        return true;
    } else {
        return false;
    }
}

boolean bancaPiena(){
    if(CB.inBanca >= CB.maxBanca){
        return true;
    } else {
        return false;
    }
}

