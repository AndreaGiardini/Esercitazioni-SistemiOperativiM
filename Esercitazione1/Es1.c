/*
* Si calcoli il massimo in un insieme di valori interi
* di N elementi, memorizzati in un vettore V.
* Si vuole affidare la ricerca del massimo a un
* insieme di M thread concorrenti, ognuno dei quali
* si dovrà occupare della ricerca del massimo in una
* porzione del vettore di dimensione K 
* data (M=N/K).
* Il thread iniziale dovrà quindi:
* –  Inizializzare il vettore V con N valori casuali;
* –  Creare gli M thread concorrenti;
* –  Ricercare il massimo tra gli M risultati ottenuti
* dai thread e stamparne il valore.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_RND_NUM 1050 // Massimo numero random
#define NUM_EL 100 // Numero di elementi
#define NUM_TH 10 // Numero di Thread

// Vettore degli elementi
int V[NUM_EL];
// Numero di elementi che deve processare ogni thread
int el_num[NUM_TH];

// Lavoro di ogni singolo Thread
void* thread_work(void* t)
{
	int tid = (intptr_t) t; //thread-id
	int i, displ=0;
	int result=0;

	// Displacement = numero di partenza dove ogni thread deve partire a esaminare il vettore di elementi V
	// ogni thread si conta autonomamente il displacement		
	for(i=0; i < tid; i++)
		displ += el_num[i];	

	// Trovo il massimo nella parte di vettore di competenza
	for(i=0; i < el_num[tid]; i++)
	{
		if(V[displ+i] > result)
			result = V[displ+i];
	}

	pthread_exit((void*)(intptr_t) result);
}

int main (int argc, char* argv[])
{
	// indice
	int i;
	// esito primitive
	int rc;
	// status - Codici di ritorno
	void* status;
	// thread - Lista thread
	pthread_t thread[NUM_TH];

	printf("Numero di elementi: %d\nNumero casuale massimo: %d\nNumero di thread: %d\n", NUM_EL, MAX_RND_NUM, NUM_TH);

	// Inizializzazione del vettore e stampa 
	printf("\nVettore di elementi:\n");
	srand(time(NULL));

	// Creazione casuale degli elementi
	for(i=0; i < NUM_EL; i++)
	{
		V[i]= rand() % MAX_RND_NUM;
		if(i%(NUM_EL / NUM_TH)==0)
			printf("\n");
		printf("%d\t", V[i]);
	}
	printf("\n");

	// INIZIO CALCOLO (numero di elementi che deve processare ogni thread)
	// Assegno ad ogni thread almeno NUM_EL/NUM_TH
	for(i=0; i < NUM_TH; i++)
		el_num[i] = NUM_EL / NUM_TH;

	int resto = NUM_EL % NUM_TH;
	// In caso il numero degli elementi diviso il numero di thread dia un resto, assegno gli elementi mancanti ai primi (RESTO) thread
	if (resto != 0)
		for(i=0; i < resto; i++)
			el_num[i]++;

	// Creazione Threads (uso primitiva pthread_create)
	for(i=0; i < NUM_TH ; i++)
	{
		/* Passo al thread il suo indice */
		rc = pthread_create(&thread[i], NULL, thread_work, (void*)(intptr_t)i);
		// In caso di errore, stampa e termina        
		if (rc) 
		{
			printf("ERRORE: %d\n", rc);
			exit(-1);
		}
	}

	printf("\n");

	/* Join e calcolo del massimo */
	int max=0;
	for(i=0; i < NUM_TH ; i++)
	{
		rc = pthread_join(thread[i], &status);
		if (rc)
		{
			printf("ERRORE join thread %d codice %d\n", i, rc);
		}
		else
		{
			printf("Finito thread %d con ris. %d\n", i, (int)(intptr_t)status);

			// Al termine di ogni Thread, se ha trovato un massimo più alto, consideralo il massimo
			if((int)(intptr_t)status > max)
				max = (int)(intptr_t)status;
		}
	}

	/* Risultato */
	printf("\nMaximum is: %d\n", max);

	return 0;
}
