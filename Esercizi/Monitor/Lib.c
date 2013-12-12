#include "Lib.h"

/*
* Ritorna l'utente opposto
*/
utente utenteOpposto(utente usr){
    if(usr == magri){
        return grassi;
    } else {
        return magri;
    }
}

/*
* Ritorna la direzione opposta
*/
direzione dirOpposta(direzione dir){
    if(dir==sx){
        return dx;
    } else {
        return sx;
    }
}

/*
* Ritorna il tipo di utente
*/
char* getUtente(utente usr){
    if(usr==magri){
        return "magri";
    } else {
        return "grassi";
    }
}

/*
* Ritorna la stringa di direzione
*/
char* getDirezione(direzione dir){
    if(dir==dx){
        return "dx";
    } else {
        return "sx";
    }
}

/*
* Ritorna true se il ponte è pieno
*/
boolean ePieno(){
    int tot = P.inTransito[magri][dx] + P.inTransito[magri][sx] + P.inTransito[grassi][dx] + P.inTransito[grassi][sx];
    if( tot >= MAX_PONTE ){
        return true;
    } else {
        return false;
    }
}

/*
* Inizializzazione variabili
*/
void init(){
	/*
	* Mutex Init
	*/
	pthread_mutex_init (&P.lock, NULL);

    /*
    * Cond Init
    */
    pthread_cond_init (&P.coda[magri][dx], NULL);
    pthread_cond_init (&P.coda[magri][sx], NULL);
    pthread_cond_init (&P.coda[grassi][dx], NULL);
    pthread_cond_init (&P.coda[grassi][sx], NULL);

    P.sospesi[magri][dx] = 0;
    P.sospesi[magri][sx] = 0;
    P.sospesi[grassi][dx] = 0;
    P.sospesi[grassi][sx] = 0;
    P.inTransito[magri][dx] = 0;
    P.inTransito[magri][sx] = 0;
    P.inTransito[grassi][dx] = 0;
    P.inTransito[grassi][sx] = 0;

}