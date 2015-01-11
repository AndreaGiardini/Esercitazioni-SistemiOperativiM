#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Numero massimo piastrelle nel forno
#define MAXP 25
// Numero massimo lotto piastrelle
#define N 10
// Numero di threads da creare
#define NUM_THREADS 30

// Tipi piastrelle
#define T1 0
#define T2 1

// Qualità piastrelle
#define STANDARD 0
#define SPECIAL 1

typedef struct lotto {
	int num_piastrelle;
	int qualita;
	int tempo;
} lotto;

// Gestore del forno
typedef struct gestore {
	pthread_mutex_t mutex; // Per la mutua esclusione all'accesso del forno

	int temperatura; // temperatura del forno (T1 o T2)
	int piastrelle_in_forno; // contatore piastrelle in forno

	int attesa[2][2][N]; // numero lotti in attesa per tipo piastrelle e tipo qualità (abbiamo una coda per quantità)
	pthread_cond_t coda[2][2][N]; // code di attesa per tipo piastrelle e tipo qualità (abbiamo una coda per quantità)
} gestore;

gestore G;

int morePriorityWaiting(int type, int quality, int num)
{
	int i;

	// Per ogni coda dello stesso tipo e della stessa qualità, controlla che non ci siano lotti di minor quantità in attesa
	for(i=0; i<num; i++)
		if(G.attesa[type][quality][i] > 0)
			return i;

	return -1;
}

void signal()
{
	int s;
	if((s=morePriorityWaiting(T2,STANDARD,N))!=-1)
		pthread_cond_signal(&G.coda[T2][STANDARD][s]);
	if((s=morePriorityWaiting(T2,SPECIAL,N))!=-1)
		pthread_cond_signal(&G.coda[T2][SPECIAL][s]);
	if((s=morePriorityWaiting(T1,STANDARD,N))!=-1)
		pthread_cond_signal(&G.coda[T1][STANDARD][s]);
	if((s=morePriorityWaiting(T1,SPECIAL,N))!=-1)
		pthread_cond_signal(&G.coda[T1][SPECIAL][s]);
}


void thread_T1(tid)
{
	// GENERO LOTTO CASUALE
	lotto l;
	l.num_piastrelle = (rand()%N)+1;
	l.qualita = rand()%2;
	l.tempo = rand()%3;

	//printf("Creato lotto %d T1 qualità: %d da %d pezzi\n", tid, l.qualita, l.num_piastrelle);
	pthread_mutex_lock(&G.mutex);
	// PRIVILEGIA QUALITA' STANDARD
	if(l.qualita == STANDARD)
	{
		// ASPETTA
		while((G.piastrelle_in_forno + l.num_piastrelle > MAXP) || // fintanto che nel forno non possono entrare le piastrelle del lotto
					(G.temperatura == T2 && G.piastrelle_in_forno > 0) ||	// o fintanto che la temperatura sia alta e ci siano ancora piastrelle in forno
					morePriorityWaiting(T2, STANDARD, N) != -1 ||	// o fintanto che ci sono lotti T2+standard in attesa
					morePriorityWaiting(T2, SPECIAL, N) != -1 ||	// o fintanto che ci sono lotti T2+speciali in attesa
					morePriorityWaiting(T1, STANDARD, l.num_piastrelle-1) != -1) // o fintanto che ci sono lotti meno numerosi in attesa
		{
			G.attesa[T1][STANDARD][l.num_piastrelle-1]++;
			signal();
			pthread_cond_wait(&G.coda[T1][STANDARD][l.num_piastrelle-1], &G.mutex);
			G.attesa[T1][STANDARD][l.num_piastrelle-1]--;
		}
	}
	// QUALITA' SPECIALE
	else 
	{
		// ASPETTA
		while((G.piastrelle_in_forno + l.num_piastrelle) > MAXP || // fintanto che nel forno non possono entrare le piastrelle del lotto
					(G.temperatura == T2 && G.piastrelle_in_forno > 0) ||	// o fintanto che la temperatura sia alta e ci siano ancora piastrelle in forno
					morePriorityWaiting(T2, STANDARD, N) != -1 ||	// o fintanto che ci sono lotti T2+standard in attesa
					morePriorityWaiting(T2, SPECIAL, N) != -1 ||	// o fintanto che ci sono lotti T2+speciali in attesa
					morePriorityWaiting(T1, STANDARD, N) != -1 ||	// o fintanto che ci sono lotti T1+standard in attesa
					morePriorityWaiting(T1, SPECIAL, l.num_piastrelle-1) != -1)	// o fintanto che ci sono lotti meno numerosi in attesa
		{
			G.attesa[T1][SPECIAL][l.num_piastrelle-1]++;
			signal();
			pthread_cond_wait(&G.coda[T1][SPECIAL][l.num_piastrelle-1], &G.mutex);
			G.attesa[T1][SPECIAL][l.num_piastrelle-1]--;
		}
	}

	G.temperatura = T1;
	G.piastrelle_in_forno+=l.num_piastrelle;
	if(l.qualita == STANDARD)
		printf("Infornato lotto %d T1 STANDARD di dimensione %d (in forno ora: %d)\n", tid, l.num_piastrelle, G.piastrelle_in_forno);
	else
		printf("Infornato lotto %d T1 SPECIALE di dimensione %d (in forno ora: %d)\n", tid, l.num_piastrelle, G.piastrelle_in_forno);
	pthread_mutex_unlock(&G.mutex);

	// Simulazione tempo di utilizzo forno per il tempo del lotto
	sleep(l.tempo);

	// Esce dal forno
	pthread_mutex_lock(&G.mutex);
	G.piastrelle_in_forno-=l.num_piastrelle;
	if(l.qualita == STANDARD)
		printf("Lotto %d T1 STANDARD di dimensione %d uscito (in forno ora: %d)\n", tid, l.num_piastrelle, G.piastrelle_in_forno);
	else
		printf("Lotto %d T1 SPECIALE di dimensione %d uscito (in forno ora: %d)\n", tid, l.num_piastrelle, G.piastrelle_in_forno);
	
	signal();
	pthread_mutex_unlock(&G.mutex);

	pthread_exit (0);
}

void thread_T2(tid)
{
	// GENERO LOTTO CASUALE
	lotto l;
	l.num_piastrelle = (rand()%N)+1;
	l.qualita = rand()%2;
	l.tempo = rand()%3;

	//printf("Creato lotto %d T2 qualità: %d da %d pezzi\n", tid, l.qualita, l.num_piastrelle);

	pthread_mutex_lock(&G.mutex);
	// PRIVILEGIA QUALITA' STANDARD
	if(l.qualita == STANDARD)
	{
		// ASPETTA
		while((G.piastrelle_in_forno + l.num_piastrelle) > MAXP ||	// fintanto che nel forno non possono entrare le piastrelle del lotto
					(G.temperatura == T1 && G.piastrelle_in_forno > 0) ||	// o fintanto che la temperatura sia bassa e ci siano ancora piastrelle in forno
					morePriorityWaiting(T2, STANDARD, l.num_piastrelle-1) != -1)	// o fintanto che ci siano lotti meno numerosi in attesa
		{
			G.attesa[T2][STANDARD][l.num_piastrelle-1]++;
			signal();
			pthread_cond_wait(&G.coda[T2][STANDARD][l.num_piastrelle-1], &G.mutex);
			G.attesa[T2][STANDARD][l.num_piastrelle-1]--;
		}
	}
	// QUALITA' SPECIALE
	else 
	{
		// ASPETTA
		while((G.piastrelle_in_forno + l.num_piastrelle) > MAXP || // fintanto che nel forno non possono entrare le piastrelle del lotto
					(G.temperatura == T1 && G.piastrelle_in_forno > 0) ||	// o fintanto che la temperatura sia alta e ci siano ancora piastrelle in forno
					morePriorityWaiting(T2, STANDARD, N) != -1 ||								// o fintanto che ci sono lotti T2+standard in attesa
					morePriorityWaiting(T2, SPECIAL, l.num_piastrelle-1) != -1)		// o fintanto che ci sono lotti meno numerosi in attesa
		{
			G.attesa[T2][SPECIAL][l.num_piastrelle-1]++;
			signal();
			pthread_cond_wait(&G.coda[T2][SPECIAL][l.num_piastrelle-1], &G.mutex);
			G.attesa[T2][SPECIAL][l.num_piastrelle-1]--;
		}
	}
	
	G.temperatura = T2;
	G.piastrelle_in_forno+=l.num_piastrelle;
	if(l.qualita == STANDARD)
		printf("Infornato lotto %d T2 STANDARD di dimensione %d (in forno ora: %d)\n", tid, l.num_piastrelle, G.piastrelle_in_forno);
	else
		printf("Infornato lotto %d T2 SPECIALE di dimensione %d (in forno ora: %d)\n", tid, l.num_piastrelle, G.piastrelle_in_forno);
	pthread_mutex_unlock(&G.mutex);

	// Simulazione tempo di utilizzo forno per il tempo del lotto
	sleep(l.tempo);

	// Esce dal forno
	pthread_mutex_lock(&G.mutex);
	G.piastrelle_in_forno-=l.num_piastrelle;
	if(l.qualita == STANDARD)
		printf("Lotto %d T2 STANDARD di dimensione %d uscito (in forno ora: %d)\n", tid, l.num_piastrelle, G.piastrelle_in_forno);
	else
		printf("Lotto %d T2 SPECIALE di dimensione %d uscito (in forno ora: %d)\n", tid, l.num_piastrelle, G.piastrelle_in_forno);

	signal();
	pthread_mutex_unlock(&G.mutex);

	pthread_exit (0);
}

void* thread_work(void* t)
{
	int tid = (intptr_t) t;

	// Genero nuovo seed per ogni thread
	srand(time(NULL)+tid);

	// MA o MI in modo casuale
	if(rand()%2)
		thread_T1(tid);
	else
		thread_T2(tid);

	printf("Tid: %d\n", tid);

	pthread_exit((void*)(intptr_t) tid);
}

int main(int argc, char * argv[])
{
	int i, j, k, ret_code;
	void* status;

	pthread_t thread[NUM_THREADS];

	// Inizializzazione Gestore:
	pthread_mutex_init(&G.mutex, NULL);
	// Inizializzo la temperatura a bassa
	G.temperatura = T1;
	G.piastrelle_in_forno = 0;
	// Azzero le code di attesa 
	for(i=0; i <= 2; i++)
		for(j=0; j <= 2; j++)
			for(k=0; k < N; k++)
			{
				pthread_cond_init (&G.coda[i][j][k], NULL);
				G.attesa[i][j][k]=0;
			}

	//printf("prova 0: %d",morePriorityWaiting(T2, STANDARD, 0));

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
