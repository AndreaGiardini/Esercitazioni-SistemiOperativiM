#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Numero di threads da creare
#define NUM_THREADS 30

// Sale ristorante
#define S1 0 // Sala 1
#define S2 1 // Sala 2

// Tipo Clienti
#define F 0 // Fumatori
#define NF 1 // Non Fumatori

// Numero tavoli Sala 1
#define N1 5
// Numero tavoli Sala2
#define N2 10

// Boolean
typedef enum { false, true } boolean;

// Gestore del ristorante
typedef struct gestore {
	pthread_mutex_t mutex; // Per la mutua esclusione 

	int persone[2]; // numero coppie presenti per ciascuna sala
	int tipoClientiSala2; // F o NF

	int attesa[2]; // numero coppie in attesa per tipo cliente
	pthread_cond_t coda[2]; // code di attesa per tipo cliente
} gestore;

gestore G;

boolean sala1Piena()
{
	return G.persone[S1] == N1;
}

boolean sala2Piena()
{
	return G.persone[S2] == N2;
}

boolean sala2Vuota()
{
	return G.persone[S2] == 0;
}

void signal()
{
	if(G.attesa[NF]>0)
		pthread_cond_signal(&G.coda[NF]);
	else if(G.attesa[F]>0)
		pthread_cond_signal(&G.coda[F]);
}

void thread_F(tid)
{
	pthread_mutex_lock(&G.mutex);
	// ASPETTA
	while(sala2Piena() || // se non c'è posto in sala 2
				(G.tipoClientiSala2 == NF && !sala2Vuota()))// o se ci sono clienti NF in sala 2 
	{
		G.attesa[F]++;
		signal();
		pthread_cond_wait(&G.coda[F], &G.mutex);
		G.attesa[F]--;
	}
	// ENTRA
	G.tipoClientiSala2=F;
	G.persone[S2]++;
	printf("Entrata coppia %d F in SALA 2 (in SALA 2 ora: %d)\n", tid, G.persone[S2]);
	pthread_mutex_unlock(&G.mutex);

	// Simulazione tempo pasto
	sleep(2);

	// ESCE
	pthread_mutex_lock(&G.mutex);
	G.persone[S2]--;
	printf("Uscita coppia %d F da SALA 2 (in SALA 2 ora: %d)\n", tid, G.persone[S2]);

	signal();
	pthread_mutex_unlock(&G.mutex);

	pthread_exit (0);
}

void thread_NF(tid)
{
	pthread_mutex_lock(&G.mutex);
	// ASPETTA
	while(sala1Piena() && // se non c'è posto in sala 1
				(sala2Piena() || (G.tipoClientiSala2 == F && !sala2Vuota())))// e se non c'è posto in sala 2 o ci sono fumatori in sala 2 
	{
		G.attesa[NF]++;
		signal();
		pthread_cond_wait(&G.coda[NF], &G.mutex);
		G.attesa[NF]--;
	}
	// ENTRA (qui verifico quale delle 2 condizioni ha smesso di essere vera)
	if(!sala1Piena()) // SALA 1
	{
		G.persone[S1]++;
		printf("Entrata coppia %d NF in SALA 1 (in SALA 1 ora: %d)\n", tid, G.persone[S1]);
		pthread_mutex_unlock(&G.mutex);

		// Simulazione tempo pasto
		sleep(2);

		// ESCE
		pthread_mutex_lock(&G.mutex);
		G.persone[S1]--;
		printf("Uscita coppia %d NF da SALA 1 (in SALA 1 ora: %d)\n", tid, G.persone[S1]);
	}
	else // SALA 2
	{
		G.tipoClientiSala2=NF;
		G.persone[S2]++;
		printf("Entrata coppia %d NF in SALA 2 (in SALA 2 ora: %d)\n", tid, G.persone[S2]);
		pthread_mutex_unlock(&G.mutex);

		// Simulazione tempo pasto
		sleep(2);

		// ESCE
		pthread_mutex_lock(&G.mutex);
		G.persone[S2]--;
		printf("Uscita coppia %d NF da SALA 2 (in SALA 2 ora: %d)\n", tid, G.persone[S2]);
	}

	// Risveglio clienti in attesa
	signal();
	pthread_mutex_unlock(&G.mutex);

	pthread_exit (0);
}

void* thread_work(void* t)
{
	int tid = (intptr_t) t;

	// Genero nuovo seed per ogni thread
	srand(time(NULL)+tid);

	// F o NF in modo casuale
	if(rand()%2)
		thread_F(tid);
	else
		thread_NF(tid);

	printf("Tid: %d\n", tid);

	pthread_exit((void*)(intptr_t) tid);
}

int main(int argc, char * argv[])
{
	int i, ret_code;
	void* status;

	pthread_t thread[NUM_THREADS];

	// Inizializzazione Gestore:
	pthread_mutex_init(&G.mutex, NULL);
	G.tipoClientiSala2=NF; // inizializzo la sala 2 per NON FUMATORI
	for(i=0; i < 2; i++)
	{
		G.persone[i]=0;
		G.attesa[i]=0;
		pthread_cond_init (&G.coda[i], NULL);
	}

	// Creo i thread
	for(i=0; i < NUM_THREADS; i++)
	{
		ret_code=pthread_create(&thread[i], NULL, thread_work, (void*)(intptr_t)i);
		if(ret_code)
		{
			printf("ERRORE: %d\n", ret_code);
			exit(EXIT_FAILURE);
		}
	}

	// Join dei thread
	for(i=0; i < NUM_THREADS; i++)
	{
		ret_code=pthread_join(thread[i], &status);
		if (ret_code)
			printf("ERRORE join thread %d codice %d\n", i, ret_code);
	}

	return EXIT_SUCCESS;
}
