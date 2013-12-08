#include "Lib.h"

spogliatoio S;

/*
* Inizializzazione variabili
*/
void init(){
	/*
	* Mutex Init
	*/
	pthread_mutex_init (&S.lock, NULL);

    /*
    * Cond Init
    */
    int i;
	for(i=0; i < MAX_GRUPPO; i++){
        pthread_cond_init (&S.codaF_IN[i], NULL);
        pthread_cond_init (&S.codaF_OUT[i], NULL);
        pthread_cond_init (&S.codaS_IN[i], NULL);
        pthread_cond_init (&S.codaS_OUT[i], NULL);

        S.sospCodaF_IN[i] = 0;
        S.sospCodaF_OUT[i] = 0;
        S.sospCodaS_IN[i] = 0;
        S.sospCodaS_OUT[i] = 0;
    }

    S.cabineOccupate = 0;

    pthread_cond_init (&S.codaInSPA, NULL);
    pthread_cond_init (&S.codaInFUN, NULL);
    S.inSPA = 0;
    S.inFUN = 0;

    S.inservienteIn = false;

}

/*
* Controlla se lo spogliatoio è pieno
*/
boolean spogliatoioPieno(){
    if (S.cabineOccupate == NC){
        return true;
    } else {
        return false;
    }
}

boolean SPAPieno(){
    if(MAXS == S.inSPA){
        return true;
    }
    return false;
}

boolean FUNPieno(){
    if(MAXF == S.inFUN){
        return true;
    }
    return false;
}