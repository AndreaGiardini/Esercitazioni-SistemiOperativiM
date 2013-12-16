#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Numero go-kart di tipo C1
#define N1 5
// Numero go-kart di tipo C2
#define N2 4
// Numero di threads da creare
#define NUM_THREADS 30

// Tipi cilindrate kart
#define C1 0
#define C2 1
// Tipi clienti
#define MA 0
#define MI 1

// Boolean
typedef enum { false, true } boolean;

// Gestore del circuito go-kart
typedef struct gestore {
	pthread_mutex_t mutex; // Per la mutua esclusione all'accesso della pista
	
	int kart_pista[2]; // numero cart in pista per tipo
	int attesa[2][2]; // numero clienti in attesa per tipo cart e tipo cliente
	pthread_cond_t coda[2][2]; // code di attesa per tipo cart e tipo cliente
} gestore;

gestore G;

void signal()
{
	if(G.attesa[C1][MA]>0)
		pthread_cond_signal(&G.coda[C1][MA]);
	else if(G.attesa[C2][MA]>0)
		pthread_cond_signal(&G.coda[C2][MA]);
	else 
		pthread_cond_signal(&G.coda[C2][MI]);
}


void thread_MA(tid)
{
	pthread_mutex_lock(&G.mutex);
	
	// Prima di tutto, controlla che ci siano kart C1 disponibili
	if(G.kart_pista[C1] < N1)
	{
		// ASPETTA
		while(G.kart_pista[C2] > 0 || // fintanto che in pista ci siano kart di tipo C2
					G.kart_pista[C1] == N1) // o fintanto che la pista sia piena
		{
			G.attesa[C1][MA]++;
			signal();
			pthread_cond_wait(&G.coda[C1][MA], &G.mutex);
			G.attesa[C1][MA]--;
		}

		G.kart_pista[C1]++;
		printf("Persona %d MAGGIORENNE è entrata in pista con un KART C1 (in pista ora: %d)\n", tid, G.kart_pista[C1]); 
		pthread_mutex_unlock(&G.mutex);

		// Simulazione tempo di utilizzo pista
		sleep(2);
	
		// Esce dalla pista
		pthread_mutex_lock(&G.mutex);
		G.kart_pista[C1]--;
		printf("Persona %d MAGGIORENNE è uscita dalla pista con un KART C1 (in pista ora: %d)\n", tid, G.kart_pista[C1]);
		pthread_mutex_unlock(&G.mutex);
	}
	// Se non ci sono kart C1, mettiti in coda per usare un kart C2
	else
	{
		// ASPETTA
		while(G.kart_pista[C1] > 0 || // fintanto che in pista ci siano kart di tipo C1 
					G.kart_pista[C2] == N1) // o fintanto che la pista sia piena 
		{
			G.attesa[C2][MA]++;
			signal();
			pthread_cond_wait(&G.coda[C2][MA], &G.mutex);
			G.attesa[C2][MA]--;
		}

		G.kart_pista[C2]++;
		printf("Persona %d MAGGIORENNE è entrata in pista con un KART C2 (in pista ora: %d)\n", tid, G.kart_pista[C2]); 
		pthread_mutex_unlock(&G.mutex);

		// Simulazione tempo di utilizzo pista
		sleep(2);
	
		// Esce dalla pista
		pthread_mutex_lock(&G.mutex);
		G.kart_pista[C2]--;
		printf("Persona %d MAGGIORENNE è uscita dalla pista con un KART C2 (in pista ora: %d)\n", tid, G.kart_pista[C2]);
		pthread_mutex_unlock(&G.mutex);
	}
	
	signal();
	pthread_exit (0);
}

void thread_MI(tid)
{
	pthread_mutex_lock(&G.mutex);
	// ASPETTA
	while(G.kart_pista[C2]==N2 || // fintanto che la pista sia piena
				G.kart_pista[C1] > 0 || // o fintanto che in pista ci siano kart di tipo C1
				G.attesa[C1][MA] > 0 || G.attesa[C2][MA] > 0) // o fintanto che ci siano clienti maggiorenni in coda
	{
		G.attesa[C2][MI]++;
		signal();
		pthread_cond_wait(&G.coda[C2][MI], &G.mutex);
		G.attesa[C2][MI]--;
	}
	
	G.kart_pista[C2]++;
	printf("Persona %d MINORENNE è entrata in pista con un KART C2 (in pista ora: %d)\n", tid, G.kart_pista[C2]); 
	pthread_mutex_unlock(&G.mutex);

	// Simulazione tempo di utilizzo pista
	sleep(2);
	
	// Esce dalla pista
	pthread_mutex_lock(&G.mutex);
	G.kart_pista[C2]--;
	printf("Persona %d MINORENNE è uscita dalla pista con un KART C2 (in pista ora: %d)\n", tid, G.kart_pista[C2]);
	pthread_mutex_unlock(&G.mutex);

	signal();
	pthread_exit (0);
}

void* thread_work(void* t)
{
	int tid = (intptr_t) t;

	// Genero nuovo seed per ogni thread
	srand(time(NULL)+tid);

	// MA o MI in modo casuale
	if(rand()%2)
		thread_MA(tid);
	else
		thread_MI(tid);

	printf("Tid: %d\n", tid);

	pthread_exit((void*)(intptr_t) tid);
}

int main(int argc, char * argv[])
{
	int i, j, ret_code;
	void* status;

	pthread_t thread[NUM_THREADS];

	// Inizializzazione Gestore:
	pthread_mutex_init(&G.mutex, NULL);
	// Azzero i kart in pista e le code di attesa 
	for(i=0; i <= C2; i++)
	{
		G.kart_pista[i]=0;
		for(j=0; j <= MI; j++)
		{
			pthread_cond_init (&G.coda[i][j], NULL);
			G.attesa[i][j]=0;
		}
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
