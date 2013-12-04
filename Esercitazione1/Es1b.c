/*
* Si vuole affidare il calcolo del prodotto scalare di due
* vettori A e B di dimensione N ad un insieme M thread
* concorrenti (si supponga che N sia multiplo intero di 
* M, cioè N=KM).
* Ogni thread Ti si dovrà occupare del calcolo del prodotto
* scalare tra due sottovettori di dimensione K data (M=N/K).
* In altre parole, Ti dovrà calcolare il valore
* ai.bi+ai+1.bi+1+ ... +ai+k-1.bi+k-1
* Al termine del calcolo, ogni thread Ti dovrà aggiungere il
* valore calcolato alla variabile condivisa RIS[nella quale viene
* quindi accumulato il risultato finale A.B]; inoltre, per
* registrare l’ordine di aggiornamento della variabile, Ti dovrà
* scrivere il proprio identificatore nella prima posizione libera
* di un vettore LOG di M elementi.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_RND_NUM 10 // Massimo numero random
#define K 2 // Numero di elementi per thread
#define M 5 // Numero di Thread concorrenti
#define N K*M

// Dimensione vettori 
//int N = K*M;
// Vettori di elementi A e B
int A[N], B[N];

// Lavoro di ogni singolo Thread
void* thread_work(void* t)
{
	int tid = (intptr_t) t; //thread-id
	int i; // indice
	int displ=0; // displacement
	int result=0; // risultato

	// Displacement = numero di partenza dove ogni thread deve partire per il calcolo del prodotto scalare dei due vettori
	displ = tid*K;

	// Calcolo il prodotto scalare della parte di competenza
	for(i=displ; i < displ+K; i++)
	{
		result+=(A[i]*B[i]);
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
	pthread_t thread[M];

	printf("Numero di elementi: %d\nNumero casuale massimo: %d\nNumero di thread: %d\n", K, MAX_RND_NUM, M);

	srand(time(NULL));

	// Inizializzazione dei 2 vettori e stampa 
	printf("\nVettore A:\n");
	// Creazione casuale degli elementi
	for(i=0; i < N; i++)
	{
		A[i]= rand() % MAX_RND_NUM;
		printf("%d\t", A[i]);
	}
	printf("\n");

	// Inizializzazione dei 2 vettori e stampa 
	printf("\nVettore B:\n");
	// Creazione casuale degli elementi
	for(i=0; i < N; i++)
	{
			B[i]= rand() % MAX_RND_NUM;
			printf("%d\t", B[i]);
	}
	printf("\n");

	// Creazione Threads (uso primitiva pthread_create)
	for(i=0; i < M; i++)
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

	/* Join e calcolo del risultato */
	int RIS=0;
	for(i=0; i < M; i++)
	{
		rc = pthread_join(thread[i], &status);
		if (rc)
		{
			printf("ERRORE join thread %d codice %d\n", i, rc);
		}
		else
		{
			printf("Finito thread %d con ris. %d\n", i, (int)(intptr_t)status);
			 
			// Aggiorna prodotto scalare
			RIS += (int)(intptr_t)status;
		}
	}

	/* Risultato */
	printf("\nProdotto scalare totale è: %d\n", RIS);

	return 0;
}
