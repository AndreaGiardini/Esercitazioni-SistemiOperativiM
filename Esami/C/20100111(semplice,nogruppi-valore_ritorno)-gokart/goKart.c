#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// GoKart di ogni cilindrata
#define N1 3
#define N2 2
// N thread creati
#define MAX_MA  4
#define MAX_MI  5

#define C1	0
#define C2	1
typedef struct{
	pthread_mutex_t mutex;		// per accedere al gestore in mutex
	pthread_cond_t codaMA;	 	// sospensione per i MA
	pthread_cond_t codaMI;		// sospensione per i MI
	int numeroMA_C1;			// numero di MA in pista (goKart C1)
	int numeroMA_C2;			// numero di MA in pista (goKart C2)
	int numeroMI;				// numero di MI in pista (per forza goKart C2)

	int attesaMA; 				// MA in attesa
	int attesaMI; 				// MI in attesa

} pista;

pista S;

void noleggioMaggiorenne(pista *s, int *goKart)
{ 	
	/*
		Un maggiorenne che noleggia resta in attesa se:
		- goKart finti
		- goKart C1 finiti e in pista ci sono goKart C1 (non importa guardare se può noleggiare C2, tanto non potrebbe entrare)
		- goKart C2 finiti e in pista ci sono goKart C2 (C2 si controlla dopo C1, quindi se non ci sono C2 i gokart sono proprio finiti)
	*/
	int i;
	pthread_mutex_lock(&s->mutex);
	// Il controllo se i gokart sono finiti è implicito in quelli fatti
	while ( s->numeroMA_C1 == N1 ||			// Se sono finiti è perchè sono in pista
			s->numeroMA_C2 + s->numeroMI == N2
			)
	{	
		// Mi sospendo
		s->attesaMA++;
		printf("MA sospesi (C1 liberi: %d - C2 liberi: %d)\n",N1-s->numeroMA_C1,N2-(s->numeroMA_C2+s->numeroMI));	
		pthread_cond_wait(&(s->codaMA), &(s->mutex));
		// Risveglio
		s->attesaMA--;
		printf("MA risvegliati (C1 liberi: %d - C2 liberi: %d)\n",N1-s->numeroMA_C1,N2-(s->numeroMA_C2+s->numeroMI));		
	}
	// MA entrati, con quale cilindrata?
	if (N1 - s->numeroMA_C1 > 0)
	{
		s->numeroMA_C1++;
		*goKart = C1;
	} else
	{
		s->numeroMA_C2++;
		*goKart = C2;
	}
	printf("MA in pista con goKart %d (C1 liberi: %d - C2 liberi: %d)\n",*goKart,N1-s->numeroMA_C1,N2-(s->numeroMA_C2+s->numeroMI));	
	
	// In entrata risveglio prima un MA, (se c'è), altrimenti risveglio un MI
	if (s->attesaMA > 0)
		pthread_cond_signal (&(s->codaMA));
	else
		if (s->attesaMI > 0)
		pthread_cond_signal (&(s->codaMI));
	 
 	pthread_mutex_unlock(&s->mutex);
}

void restituzioneMaggiorenne(pista *s, int goKart)
{	
	int i;	
	pthread_mutex_lock(&s->mutex);
	
	// Ma riconsegna, quale goKart?
	if (goKart == C1)
		s->numeroMA_C1--;
	else
		s->numeroMA_C2--;

	printf("MA riconsegna goKArt %d (C1 liberi: %d - C2 liberi: %d)\n",goKart, N1-s->numeroMA_C1,N2-(s->numeroMA_C2+s->numeroMI));	
	
	// In uscita risveglio prima un MA, (se c'è), altrimenti risveglio un MI
	if (s->attesaMA > 0)
		pthread_cond_signal (&(s->codaMA));
	else
		if (s->attesaMI > 0)
		pthread_cond_signal (&(s->codaMI));
		
	pthread_mutex_unlock(&s->mutex);
}

void noleggioMinorenne(pista *s) // Per forza goKart C2
{ 	
	/*
		Un minorenne si blocca se:
		- Ci sono MA in attesa
		- non ci sono goKart C2 liberi
		- ci sono C1 in pista
	*/
	int i, stop = 0;
	pthread_mutex_lock(&s->mutex);
	
	while ( s->attesaMA > 0 ||
			s->numeroMA_C2 + s->numeroMI == N2 ||
			s->numeroMA_C1 + s->numeroMI != 0
		   )
	{	
		// Mi sospendo
		s->attesaMI++;
		printf("MI sospesi (C1 liberi: %d - C2 liberi: %d)\n",N1-s->numeroMA_C1,N2-(s->numeroMA_C2+s->numeroMI));	
		pthread_cond_wait(&(s->codaMI), &(s->mutex));
		// Risveglio
		s->attesaMI--;
		printf("MI risvegliati (C1 liberi: %d - C2 liberi: %d)\n",N1-s->numeroMA_C1,N2-(s->numeroMA_C2+s->numeroMI));	
	}
	// MI noleggiano C2
	s->numeroMI++;	
	printf("MI in pista con goKart %d (C1 liberi: %d - C2 liberi: %d)\n",C2,N1-s->numeroMA_C1,N2-(s->numeroMA_C2+s->numeroMI));		
	
	// Se sono entrato io sicuramente non ci sono MA in attesa, quindi risveglio solo un MI 		
	if (s->attesaMI > 0)
		pthread_cond_signal (&(s->codaMI));
	 
 	pthread_mutex_unlock(&s->mutex);
}

void restituzioneMinorenne(pista *s)
{	
	int i;	
	pthread_mutex_lock(&s->mutex);
	
	// MI riconsegna C2
	s->numeroMI--;
	printf("MI riconsegna goKArt %d (C1 liberi: %d - C2 liberi: %d)\n",C2, N1-s->numeroMA_C1,N2-(s->numeroMA_C2+s->numeroMI));		
	
	// In uscita risveglio prima un MA, (se c'è), altrimenti risveglio un MI
	if (s->attesaMA > 0)
		pthread_cond_signal (&(s->codaMA));
	else
		if (s->attesaMI > 0)
		pthread_cond_signal (&(s->codaMI));
			
	pthread_mutex_unlock(&s->mutex);
}

void *thread_MA(void * arg)
{	
	int goKart; // I non fumatori dobbiamo sapere quale goKart gli è stato assegnato
	sleep(1);
	noleggioMaggiorenne(&S, &goKart);
	sleep(1); 
	restituzioneMaggiorenne(&S, goKart);
	pthread_exit(0);
}
void *thread_MI(void * arg)
{	
	sleep(1);
	noleggioMinorenne(&S);
	sleep(1); 
	restituzioneMinorenne(&S);
	pthread_exit(0);
}

int main ()
{  	
	pthread_t ma[MAX_MA], mi[MAX_MI];
	int i;

  	/* inizializzazione S: */
	memset (&S, 0, sizeof(pista) );

	pthread_mutex_init(&S.mutex, NULL);
	pthread_cond_init (&S.codaMA,NULL);
	pthread_cond_init (&S.codaMI,NULL);
	
	/* Creazione thread: */
	for (i = 0; i < MAX_MA; i++)
		pthread_create (&ma[i], NULL, thread_MA, NULL);

	for (i = 0; i < MAX_MI; i++)
		pthread_create (&mi[i], NULL, thread_MI, NULL);
	
	/* Join thread: */
   	for (i = 0; i < MAX_MA; i++)
		pthread_join (ma[i], NULL);
		
	for (i = 0; i < MAX_MI; i++)
		pthread_join (mi[i], NULL);

	return 0;
}

