#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Numero persone che partecipano al sondaggio
#define N 10
// Numero persone che guardano il film senza partecipare al sondaggio
#define F 20
// Numero films
#define K 5
// Numero massimo utenti a vedere il film
#define MAXC 3

// Gestore dei sondaggi
typedef struct gestore {
	pthread_mutex_t mutex; // Per la mutua esclusione all'accesso dei sondaggi
	/*
	* Tabella dei voti
	* Abbiamo K film con N voti
	*/
	int risultatiSondaggi[K][N];        
	int n; // Numero sondaggi effettuati finora
	float avg[K]; // Medie voti dei film
	int bestFilm;        

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
		// Stampo la media attuale
		for(j=0, sum=0; j<N; j++)
      // Se il voto è 0, non è stato ancora assegnato, quindi non calcolarlo per la media
      if(G.risultatiSondaggi[i][j]!=0)
        sum+=G.risultatiSondaggi[i][j];

		G.avg[i] = (float)sum/G.n;
		printf("Risultati dopo %d sondaggi:\n", G.n);
		printf("Film %i ha una media voto di: %.2f\n", i, G.avg[i]);
	}
	printf("\n\n");
}

void printBestFilm()
{
  int i;
  float maxAvg=1;
  for (i=0; i<K; i++)
    if (G.avg[i]>maxAvg)
    {
		  G.bestFilm=i;
		  maxAvg=G.avg[i];
    }

  printf("Il miglior film della stagione è stato il film %i con una media voto di: %.2f\n", G.bestFilm, maxAvg);
}

void * filmUser(void * t)
{
  int tid = (intptr_t) t;
 	
	// fintanto che non sia finito il sondaggio aspetta l'inizio del film
	// fintanto che la sala è piena, aspetta prima di entrare
  while(G.n!=N || G.in_visione_film>=MAXC)
  {
		pthread_mutex_lock(&G.mutex);
    G.in_attesa_film++;
    pthread_mutex_unlock(&G.mutex);
    
    // Si blocca fintanto che non verrà risvegliato da qualcuno
    sem_wait(&G.semph);

    // Si toglie dall'attesa
    pthread_mutex_lock(&G.mutex);
    G.in_attesa_film--;
    pthread_mutex_unlock(&G.mutex);
  }

	pthread_mutex_lock(&G.mutex);
  G.in_visione_film++;
	printf("Persona %d ha iniziato la visione del film (in sala %d, in attesa %d)\n", tid, G.in_visione_film, G.in_attesa_film); 	
	// Se c'è almeno un altro posto in sala, risveglia un altro processo
  if(G.in_visione_film<MAXC)
    sem_post(&G.semph);
	pthread_mutex_unlock(&G.mutex);

	// Simulazione tempo di visione del film	
	sleep(2); 

  // Film visto
  pthread_mutex_lock(&G.mutex);        
  G.in_visione_film--;
	printf("Persona %d ha finito di guardare il film (in sala %d, in attesa %d)\n", tid, G.in_visione_film, G.in_attesa_film);
  // risveglia un altro processo in attesa
	if(G.in_attesa_film>0)
  	sem_post(&G.semph);
	pthread_mutex_unlock(&G.mutex);

	pthread_exit (0);
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

  if(G.n!=N)
  {
    G.in_attesa_film++;
    pthread_mutex_unlock(&G.mutex);
    
    // Si blocca fintanto che non verrà risvegliato da qualcuno
    sem_wait(&G.semph);

    pthread_mutex_lock(&G.mutex);
    // Si toglie dall'attesa
    G.in_attesa_film--;

    pthread_mutex_unlock(&G.mutex);
  }
  else // Il thread che entra qui è l'ultimo, ovvero quello che si occupa del calcolo del miglior film
  {
    printBestFilm();

    // Una volta calcolato il miglior film
    // tutti i thread possono iniziarne la visione                
    printf("Le proiezioni del film %d iniziano ora, massimo %d persone per volta\n\n", G.bestFilm, MAXC);        
  }
  pthread_mutex_unlock(&G.mutex);

  // Vedi il film        
  filmUser(t);
  
  pthread_exit (0);
}

int main (int argc, char * argv[])
{
  int i, j;
  
  // Il numero dei thread è dato dalla somma dei partecipanti al sondaggio più i soli spettatori del film
  pthread_t thread[N+F];

  // Randomizzo
  srand(time(NULL));
  /* inizializzazione G: */
  pthread_mutex_init(&G.mutex, NULL);
  G.n=0;
	G.in_visione_film=0;
	G.in_attesa_film=0;

  // Inizializzo il semaforo per la visione dei film a 0 ->
  // non è possibile guardare il film fintanto che non è stato decretato il film vincitore
  sem_init(&G.semph, 0, 0);
  
  // Inizializzo tutti i voti a zero
  for(i=0; i < K; i++)
    for(j=0; j < N; j++)
      G.risultatiSondaggi[K][N]=0;

  // Per ogni partecipante al sondaggio avvio un processo
  for(i=0; i < N; i++)
    pthread_create(&thread[i], NULL, sondaggioUser, (void*)(intptr_t)i);
  
  // Per ogni mero spettatore del film avvio un processo
  for(i=N; i < N+F; i++)
  	pthread_create(&thread[i], NULL, filmUser, (void*)(intptr_t)i);
  
  // Attendo la fine di tutti processi
  for(i=0; i < N+F; i++)
    pthread_join(thread[i], NULL);
  
  return 0;
}
