#include "Lib.h"

/*
* Inizializzazione variabili
*/
void init(){
	/*
	* Mutex Init
	*/
	pthread_mutex_init (&F.lock, NULL);

    int i;
    for(i=0; i < MaxG; i++){
        pthread_cond_init(&F.coda[T1][STANDARD][i], NULL);
        pthread_cond_init(&F.coda[T1][SPECIALI][i], NULL);
        pthread_cond_init(&F.coda[T2][STANDARD][i], NULL);
        pthread_cond_init(&F.coda[T2][SPECIALI][i], NULL);
        F.sosp[T1][STANDARD][i] = 0;
        F.sosp[T1][SPECIALI][i] = 0;
        F.sosp[T2][STANDARD][i] = 0;
        F.sosp[T2][SPECIALI][i] = 0;
    }

    F.inForno = 0;
    F.temperatura = 1;

}
