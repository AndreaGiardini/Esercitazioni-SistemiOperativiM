#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>

typedef enum { false, true } boolean;

#define NI 5
#define MAX 10
#define maxG 5

typedef struct{
	int personeInPista;
	pthread_mutex_t lock;
	pthread_cond_t CodaE_IN[maxG];
	pthread_cond_t CodaP_IN[maxG];
	pthread_cond_t CodaP_OUT[maxG];
	int sospCodaE_IN[maxG];
	int sospCodaP_IN[maxG];
	int sospCodaP_OUT[maxG];
} pista;

/*
 * Se c'è un processo meno prioritario in attesa rispondo true
 */
boolean menoPrioritarioE_IN(pista *ps, int num){
	int i;
	for (i=0; i < num; i++){
		if(ps->sospCodaE_IN[i]>0){
			return true;
		}
	}
	return false;
}

/*
 * Se c'è un processo meno prioritario in attesa rispondo true
 */
boolean menoPrioritarioP_IN(pista *ps, int num){
	int i;
	for (i=0; i < num; i++){
		if(ps->sospCodaP_IN[i]>0){
			return true;
		}
	}
	return false;
}

/*
 * Gruppo principianti entra in pista
 */
void InPistaP(pista *ps, int num){
	pthread_mutex_lock (ps->lock);
	int i;
	/*
	 * Principianti con gruppi poco numerosi hanno precedenza
	 */

	//Controllo se c'è un gruppo di principianti più piccolo
	//O se la capacità massima è già stata raggiunta
	while(menoPrioritarioP_IN(&ps, num) || ps->personeInPista + num > MAX){
		ps->sospCodaP_IN[num]++;
		pthread_cond_wait (ps->CodaP_IN[i], ps->lock);
		ps->sospCodaP_IN[num]--;
	}

	ps->personeInPista += num;

	//TODO: manca la signal

	pthread_mutex_unlock (ps->lock);
}

/*
 * Gruppo principianti esce dalla pista
 */
void OutPistaP(pista *ps, int num){
	pthread_mutex_lock (ps->lock);

	pthread_mutex_unlock (ps->lock);
}

/*
 * Gruppo esperti entra in pista
 */
void InPistaE(pista *ps, int num){
	pthread_mutex_lock (ps->lock);

	/*
	 * Devo verificare che non ci siano gruppi di principianti in attesa
	 */

	pthread_mutex_unlock (ps->lock);
}

/*
 * Gruppo esperti esce dalla pista
 */
void OutPistaE(pista *ps, int num){
	pthread_mutex_lock (ps->lock);

	pthread_mutex_unlock (ps->lock);
}

void *gruppoPrincipiante(void * arg){
	int num; // numero di componenti
	pista Pista = (pista) arg;
	InPistaP(&Pista, num); // possibilità di attesa
	/* simulazione uso pista*/
	sleep(1);
	OutPistaP(&Pista, num); // possibilità di attesa
}

void *gruppoEsperto(void * arg){
	int num; // numero di componenti
	pista Pista = (pista) arg;
	InPistaE(&Pista, num); // possibilità di attesa
	/* simulazione uso pista*/
	sleep(1);
	OutPistaE(&Pista, num);
}

void init(pista *ps){
	int i;
	ps->personeInPista=0;
	pthread_mutex_init (ps->lock, NULL);
	for (i=0; i < maxG; i++){
		pthread_cond_init (ps->CodaE_IN[i], NULL);
		pthread_cond_init (ps->CodaP_IN[i], NULL);
		pthread_cond_init (ps->CodaP_OUT[i], NULL);
		ps->sospCodaE_IN[i] = 0;
		ps->sospCodaP_IN[i] = 0;
		ps->sospCodaP_OUT[i] = 0;
	}
}


int main (int argc, char * argv[]){

	//Inizializzazione pista
	pista ps;
	init(&ps);


}
