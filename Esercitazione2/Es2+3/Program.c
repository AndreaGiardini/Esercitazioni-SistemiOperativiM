#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Numero persone che partecipano al sondaggio
#define N 10
// Numero films
#define K 5
// Numero massimo utenti a vedere il film
#define MAXC 3

// Gestore dei sondaggi
typedef struct gestore {
	pthread_mutex_t mutex; // Per la mutua esclusione all'accesso dei sondaggi
	/* 
	* Tabella dei voti
	*	Abbiamo K film con N voti
	*/
	int risultatiSondaggi[K][N];	
	int n; // Numero sondaggi effettuati finora
	float avg[K]; // Medie voti dei film

	sem_t semph; // Semaforo per la visione del film
	int in_visione_film; // Numero di persone che stanno guardando il film
	int in_attesa_film; // Numero di persone in attesa di guardare il film
} gestore;

gestore G;

void printResults()
{
	int i, j, sum;
	
	// Per ogni film
	for(i=0; i<K; i++)
	{
		//printf("Media del film numero %d:\n", i);
		//printf("Voti: ");
		// Stampo la media attuale
		for(j=0, sum=0; j<N; j++)
		{
			// Se il voto è 0, non è stato ancora assegnato, quindi non calcolarlo per la media
			if(G.risultatiSondaggi[i][j]!=0)
			{
				sum+=G.risultatiSondaggi[i][j];
				//printf("\t%d", G.risultatiSondaggi[i][j]);
			}
		}
		G.avg[i] = (float)sum/G.n;
		printf("Risultati dopo %d sondaggi:\n", G.n);
		printf("Film %i ha una media voto di: %.2f\n", i, G.avg[i]);
	}
	printf("\n\n");
}

void printBestFilm()
{
	int i, maxFilm;
	float maxAvg=1;
	for (i=0; i<K; i++)
		if (G.avg[i]>maxAvg)
		{
			maxFilm=i;
			maxAvg=G.avg[i];
		}

	printf("Il miglior film della stagione è stato il film %i con una media voto di: %.2f\n", maxFilm, maxAvg);
}

void * sondaggioUser(void * t)
{
	int tid = (intptr_t) t; //thread-id
	int i;
	pthread_mutex_lock(&G.mutex);

	// Per ogni film
	for(i=0; i < K; i++)
	{	
		// Assegno un voto
		G.risultatiSondaggi[i][tid] = (rand() % 10) + 1;
		printf("Persona %d - Voto: %d al film %d\n", tid, G.risultatiSondaggi[i][tid], i);
	}

	// Incremento il numero dei sondaggi effettuati
	G.n++;
	printResults();
	
	while(G.n!=N || G.in_visione_film>=MAXC)
	{
		G.in_attesa_film++;
		pthread_mutex_unlock(&G.mutex);

		// Si blocca fintanto che non verrà risvegliato da qualcuno
		sem_wait(&G.semph);

		pthread_mutex_lock(&G.mutex);
		G.in_attesa_film--;
	}
	G.in_visione_film++;

	pthread_mutex_unlock(&G.mutex);
	
	filmUser(t);
	
	pthread_exit (0); 
}

void * filmUser(void * t)
{
	pthread_mutex_lock(&G.mutex);

	sleep(2);	
	printf("Film guardato\n");
	G.in_visione_film--;
	
	// Risveglia se ci sono persone in coda
	if(G.in_attesa_film>0)
		sem_post(&G.semph);
	
	pthread_mutex_unlock(&G.mutex);
}

int main (int argc, char * argv[])
{
	int i, j;

	pthread_t thread[N];

	// Randomizzo
	srand(time(NULL));
	/* inizializzazione G: */
	pthread_mutex_init(&G.mutex, NULL); 
	G.n=0;
	// Inizializzo il semaforo per la visione dei film a 0 ->
	// non è possibile guardare il film fintanto che non è stato decretato il film vincitore
	sem_init(&G.semph, 0, 0);
	
	// Inizializzo tutti i voti a zero
	for(i=0; i < K; i++)
		for(j=0; j < N; j++)
			G.risultatiSondaggi[K][N]=0;

	// Per ogni utente avvio un processo 
	for(i=0; i < N; i++)
	{
		pthread_create(&thread[i], NULL, user, (void*)(intptr_t)i);
	}

	// Attendo la fine dei processi 
	for(i=0; i < N; i++)
	{
		pthread_join(thread[i], NULL);
	}

	printBestFilm();
	

	return 0;
}
