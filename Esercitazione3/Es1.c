#include "Es1_lib.h"

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
	pthread_mutex_lock (&ps->lock);
	int i;
	/*
	 * Principianti con gruppi poco numerosi hanno precedenza
	 */

	//Controllo se c'è un gruppo di principianti più piccolo
	//O se la capacità massima è già stata raggiunta
	//O se il rapporto non viene rispettato
	while(menoPrioritarioP_IN(ps, num) || ps->PinPista + ps->EinPista + num > MAX || ps->EinPista > ps->PinPista + num){
		ps->sospCodaP_IN[num]++;
		pthread_cond_wait (&ps->CodaP_IN[i], &ps->lock);
		ps->sospCodaP_IN[num]--;
	}

	ps->PinPista += num;

	pthread_cond_signal(ps->CodaE_IN);
	pthread_cond_signal(ps->CodaP_OUT);

	pthread_mutex_unlock (&ps->lock);
}

/*
 * Gruppo principianti esce dalla pista
 */
void OutPistaP(pista *ps, int num){
	int i;
	pthread_mutex_lock (&ps->lock);

	//Verifico che il rapporto sia rispettato prima di uscire
	while(&ps->EinPista > &ps->PinPista - num){
			ps->sospCodaP_OUT[num]++;
			pthread_cond_wait (&ps->CodaP_OUT[i], &ps->lock);
			ps->sospCodaP_OUT[num]--;
	}

	ps->PinPista -= num;

	pthread_cond_signal(ps->CodaP_IN);
	pthread_cond_signal(ps->CodaE_IN);

	pthread_mutex_unlock (&ps->lock);
}

/*
 * Gruppo esperti entra in pista
 */
void InPistaE(pista *ps, int num){
	int i;
	pthread_mutex_lock (&ps->lock);

	/*
	 * Devo verificare che non ci siano gruppi di principianti in attesa
	 * O che non ci siano gruppi minori di esperi
	 * Che la capacità massima sia rispettata
	 * O che il rapporto non sia rispettato
	 */
	while(menoPrioritarioP_IN(ps, maxG) || menoPrioritarioE_IN(ps, num) || ps->EinPista + ps->PinPista + num > MAX || ps->EinPista + num > ps->PinPista){
			ps->sospCodaP_IN[num]++;
			pthread_cond_wait (&ps->CodaP_IN[i], &ps->lock);
			ps->sospCodaP_IN[num]--;
	}

	pthread_mutex_unlock (&ps->lock);
}

/*
 * Gruppo esperti esce dalla pista
 */
void OutPistaE(pista *ps, int num){
	pthread_mutex_lock (&ps->lock);

	//Nessuna condizione un esperto può sempre uscire
	ps->EinPista -= num;

	pthread_mutex_unlock (&ps->lock);
}

void *gruppoPrincipiante(void * arg){
	int num; // numero di componenti
	pista * Pista = (pista *) arg;
	InPistaP(Pista, num); // possibilità di attesa
	/* simulazione uso pista*/
	sleep(1);
	OutPistaP(Pista, num); // possibilità di attesa
	pthread_exit (0);
}

void *gruppoEsperto(void * arg){
	int num; // numero di componenti
	pista * Pista = (pista *) arg;
	InPistaE(Pista, num); // possibilità di attesa
	/* simulazione uso pista*/
	sleep(1);
	OutPistaE(Pista, num);
	pthread_exit (0);
}



int main (int argc, char * argv[]){

	pista ps;
	init(&ps);

	int i=0, tipo;
	pthread_t pattinatori[NUM_TH];

    for(i = 0; i < NUM_TH; i++){
	    tipo=rand()%2;
		if (tipo)
			pthread_create(&pattinatori[i], NULL, gruppoEsperto, (void*)(rand() % maxG + 1));
		else
			pthread_create(&pattinatori[i], NULL, gruppoPrincipiante, (void*)(rand() % maxG + 1));
	}

	for(i = 0; i < NUM_TH; i++)
		pthread_join(pattinatori[i], NULL);

	return 0;





}
