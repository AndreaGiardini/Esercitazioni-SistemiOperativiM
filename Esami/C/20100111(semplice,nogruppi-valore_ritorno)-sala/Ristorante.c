#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Capienza sale
#define N1 3
#define N2 2
// N thread creati
#define MAX_F  4
#define MAX_NF 5

/*
	Non ci sono problemi di priorità tra gli stessi thread dello stesso tipo
	(tra fumatori e fumatori nessuno ha precedenza sugli altri)
	QUINDI -> non serve che le cond_t siano degli array, bastano singole e di conseguenza
			  bastano int e non array i fumatoriAttesa
*/
typedef struct{
	pthread_mutex_t mutex;		// per accedere al gestore in mutex
	pthread_cond_t fumatori; 	// sospensione per i fumatori
	pthread_cond_t nfumatori;	// sospensione per i non fumatori
	int occupatiS1_nf;			// numero non fumatori S1
	int occupatiS2_f;			// numero fumatori S2
	int occupatiS2_nf;			// numero non fumatori S2
	int fumatoriAttesa; 		// per saprere quanti fumatori sono in attesa
	int nfumatoriAttesa; 

} sala;

sala S;

void nfumatori_entra(sala *s, int *sala)
{ 	
	/*
		Una coppia di non fumatori:
		- Entra in S1 se ci sono posti
		- Altrimenti entra in S2 se:
			- c'è posto in S2
			- non ci sono F in S2
	*/
	int i;
	pthread_mutex_lock(&s->mutex);
	
	// I Non fumatori si sospendono se non riescono a entranre da nessuna parta
	while ( N1 - s->occupatiS1_nf < 1	&& // Non c'è abbastanza spazio in S1
			(N2 - (s->occupatiS2_nf + s->occupatiS2_f) < 1 || s->occupatiS2_f != 0))
	{	
		// Mi sospendo
		s->nfumatoriAttesa++;
		printf("Non fumatori sospesi (sala1 liberi: %d - sala2 liberi: %d)\n",N1-s->occupatiS1_nf,N2-(s->occupatiS2_nf+s->occupatiS2_f));	
		pthread_cond_wait(&(s->nfumatori), &(s->mutex));
		// Risveglio
		s->nfumatoriAttesa--;
		printf("Non fumatori risvegliato (sala1 liberi: %d - sala2 liberi: %d)\n",N1-s->occupatiS1_nf,N2-(s->occupatiS2_nf+s->occupatiS2_f));	
	}
	// Gruppo entrato, in quale sala?
	if (N1 - s->occupatiS1_nf > 0)
	{
		s->occupatiS1_nf++;
		*sala = 1;
	} else
	{
		s->occupatiS2_nf++;
		*sala = 2;
	}
	printf("Gruppo non fumatori entrato (sala: %d) (sala1 liberi: %d - sala2 liberi: %d)\n",*sala,N1-s->occupatiS1_nf,N2-(s->occupatiS2_nf+s->occupatiS2_f));	
	
	// In entrata risveglio prima UN non fumatore (se c'è), altrimenti risveglio UN fumatore
	if (s->nfumatoriAttesa > 0)
		pthread_cond_signal (&(s->nfumatori));
	else
		if (s->fumatoriAttesa > 0)
		pthread_cond_signal (&(s->fumatori));
	 
 	pthread_mutex_unlock(&s->mutex);
}

void nfumatori_esce(sala *s, int sala)
{	
	int i;	
	pthread_mutex_lock(&s->mutex);
	
	// Non fumatori usciti, sa che sala?
	if (sala == 1)
		s->occupatiS1_nf--;
	else
		s->occupatiS2_nf--;

	printf("Non fumatori usciti da sala %d (sala1 liberi: %d - sala2 liberi: %d)\n",sala,N1-s->occupatiS1_nf,N2-(s->occupatiS2_nf+s->occupatiS2_f));	
	
	// In uscita risveglio prima UN non fumatore (se c'è), altrimenti risveglio UN fumatore
	if (s->nfumatoriAttesa > 0)
		pthread_cond_signal (&(s->nfumatori));
	else
		if (s->fumatoriAttesa > 0)
			pthread_cond_signal (&(s->fumatori));
		
	pthread_mutex_unlock(&s->mutex);
}

void fumatori_entra(sala *s)
{ 	
	/*
		Una coppia di fumatori:
		- Entra se non ci sono NF in attesa
		- Entra in S2 se:
			- c'è posto in S2
			- non ci sono NF in S2
	*/
	int i, stop = 0;
	pthread_mutex_lock(&s->mutex);
	
	while ( s->nfumatoriAttesa > 0  || 					// Ci sono non fumatori in attesa
			s->occupatiS2_nf > 0 ||						// Ci sono non fumatori in s2
			N2 - (s->occupatiS2_nf + s->occupatiS2_f) < 1)	// Non c'è spazio in S2
	{	
		// Mi sospendo
		s->fumatoriAttesa++;
		printf("Fumatori sospesi (sala1 liberi: %d - sala2 liberi: %d)\n",N1-s->occupatiS1_nf,N2-(s->occupatiS2_nf+s->occupatiS2_f));		
		pthread_cond_wait(&(s->fumatori), &(s->mutex));
		// Risveglio
		s->fumatoriAttesa--;
		printf("Fumatori risvegliato (sala1 liberi: %d - sala2 liberi: %d)\n",N1-s->occupatiS1_nf,N2-(s->occupatiS2_nf+s->occupatiS2_f));	
	}
	// Gruppo entrato
	s->occupatiS2_f++;	
	printf("Fumatori entrati in S2 (sala1 liberi: %d - sala2 liberi: %d)\n",N1-s->occupatiS1_nf,N2-(s->occupatiS2_nf+s->occupatiS2_f));	
	
	// Se sono entrato io sicuramente non ci sono non fumatori in attesa, quindi risveglio solo un fumatore 		
	if (s->fumatoriAttesa > 0)
		pthread_cond_signal (&(s->fumatori));
	 
 	pthread_mutex_unlock(&s->mutex);
}

void fumatori_esce(sala *s)
{	
	int i;	
	pthread_mutex_lock(&s->mutex);
	
	// Gruppo uscito
	s->occupatiS2_f--;
	printf("Fumatori usciti da S2 (sala1 liberi: %d - sala2 liberi: %d)\n",N1-s->occupatiS1_nf,N2-(s->occupatiS2_nf+s->occupatiS2_f));	
	
	// In uscita risveglio prima UN non fumatore (se c'è), altrimenti risveglio UN fumatore
	if (s->nfumatoriAttesa > 0)
		pthread_cond_signal (&(s->nfumatori));
	else
		if (s->fumatoriAttesa > 0)
			pthread_cond_signal (&(s->fumatori));
			
	pthread_mutex_unlock(&s->mutex);
}

void *thread_nfumatori(void * arg)
{	
	int sala; // I non fumatori dobbiamo sapere in quale sala sono entrati
	sleep(1);
	//printf("Gruppo non fumatori in ingresso\n");
	nfumatori_entra(&S, &sala);
	sleep(1); 
	nfumatori_esce(&S, sala);
	pthread_exit(0);
}
void *thread_fumatori(void * arg)
{	
	sleep(1);
	//printf("Gruppo fumatori in ingresso\n");
	fumatori_entra(&S);
	sleep(1); 
	fumatori_esce(&S);
	pthread_exit(0);
}

int main ()
{  	
	pthread_t fumatori[MAX_F], nfumatori[MAX_NF];
	int i;

  	/* inizializzazione S: */
	memset (&S, 0, sizeof(sala) );

	pthread_mutex_init(&S.mutex, NULL);
	pthread_cond_init (&S.fumatori,NULL);
	pthread_cond_init (&S.nfumatori,NULL);
	
	/* Creazione thread: */
	for (i = 0; i < MAX_F; i++)
		pthread_create (&fumatori[i], NULL, thread_fumatori, NULL);

	for (i = 0; i < MAX_NF; i++)
		pthread_create (&nfumatori[i], NULL, thread_nfumatori, NULL);
	
	/* Join thread: */
   	for (i = 0; i < MAX_F; i++)
		pthread_join (fumatori[i], NULL);
		
	for (i = 0; i < MAX_NF; i++)
		pthread_join (nfumatori[i], NULL);

	return 0;
}

