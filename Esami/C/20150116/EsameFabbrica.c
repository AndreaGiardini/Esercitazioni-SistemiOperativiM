#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#define MAXCONTENITORE 5
#define FERRARI 0
#define PORSCHE 1
#define LAMBORG 2

typedef struct {
	sem_t modelliniProdotti;
	sem_t modelliniDaProdurre;
} ContenitoreModellini;

typedef struct {
    sem_t montaggio;
	sem_t produzione;
} CatenaMontaggio;

CatenaMontaggio *catena[3];
ContenitoreModellini *contenitore[3];

void init(CatenaMontaggio *catena, ContenitoreModellini *contenitore) {

	sem_init(&catena->montaggio, 0, 0);
	sem_init(&catena->produzione, 0, 0);
	
	sem_init(&contenitore->modelliniProdotti, 0, 0);
	sem_init(&contenitore->modelliniDaProdurre, 0, MAXCONTENITORE);
}

void *produzione(void *t) {
    long result = 0;
	int tipo = (int)t;
	
	while (1) {
		printf("[PRODUZIONE %i]: Inizio produzione\n", tipo);
		sleep(tipo + 1);
		printf("[PRODUZIONE %i]: Fine produzione\n", tipo);
		
		sem_post(&catena[tipo]->produzione);
		sem_wait(&catena[tipo]->montaggio);
		
		printf("[PRODUZIONE %i]: Parti consegnate alla catena di montaggio\n", tipo);
	}

    pthread_exit((void*) result);
}

void *montaggio(void *t) {
    long result = 0;
	int tipo = (int)t;
	
	while (1) {
		sem_wait(&catena[tipo]->produzione);
		sem_post(&catena[tipo]->montaggio);
		
		printf("[MONTAGGIO %i]: Inizio montaggio\n", tipo);
		sleep(tipo + 2);
		printf("[MONTAGGIO %i]: Fine montaggio\n", tipo);
		
		sem_wait(&contenitore[tipo]->modelliniDaProdurre);
		sem_post(&contenitore[tipo]->modelliniProdotti);
		
		printf("[MONTAGGIO %i]: Modellino messo nel contenitore\n", tipo);
	}

    pthread_exit((void*) result);
}

void *confezionamento(void *t) {
    long result = 0;
	
	while (1) {
		sem_wait(&contenitore[FERRARI]->modelliniProdotti);
		sem_post(&contenitore[FERRARI]->modelliniDaProdurre);
		printf("[CONFEZIONAMENTO]: Raccolto modellino Ferrari\n");
		
		sem_wait(&contenitore[PORSCHE]->modelliniProdotti);
		sem_post(&contenitore[PORSCHE]->modelliniDaProdurre);
		printf("[CONFEZIONAMENTO]: Raccolto modellino Porsche\n");
		
		sem_wait(&contenitore[LAMBORG]->modelliniProdotti);
		sem_post(&contenitore[LAMBORG]->modelliniDaProdurre);
		printf("[CONFEZIONAMENTO]: Raccolto modellino Lamborghini\n");
		
		printf("[CONFEZIONAMENTO]: Nuova confezione realizzata\n");
	}

    pthread_exit((void*) result);
}

int main (int argc, char *argv[]) {
    pthread_t threadProduzione[3], threadMontaggio[3], threadConfezionamento;
    int rc, i;

    void *status;

	for (i = FERRARI; i <= LAMBORG; i++) {
		ContenitoreModellini _contenitore;
		CatenaMontaggio _catena;
		
		contenitore[i] = &_contenitore;
		catena[i] = &_catena;
		
		init(catena[i], contenitore[i]);
	}
    
    for (i = FERRARI; i <= LAMBORG; i++) {
        rc = pthread_create(&threadProduzione[i], NULL, produzione, (void *)i);
        if (rc) {
            printf("ERRORE: %d\n", rc);
            exit(-1);
        }
    }
	
	for (i = FERRARI; i <= LAMBORG; i++) {
        rc = pthread_create(&threadMontaggio[i], NULL, montaggio, (void *)i);
        if (rc) {
            printf("ERRORE: %d\n", rc);
            exit(-1);
        }
    }
	
    rc = pthread_create(&threadConfezionamento, NULL, confezionamento, NULL);
	if (rc) {
		printf("ERRORE: %d\n", rc);
		exit(-1);
	}

    for (i = FERRARI; i <= LAMBORG; i++) {
        rc = pthread_join(threadProduzione[i], &status);
        if (rc) {
            printf("ERRORE join thread produzione %d codice %d\n", i, rc);
        }
    }

    for (i = FERRARI; i <= LAMBORG; i++) {
        rc = pthread_join(threadMontaggio[i], &status);
        if (rc) {
            printf("ERRORE join thread montaggio %d codice %d\n", i, rc);
        }
    }

    rc = pthread_join(threadConfezionamento, &status);
    if (rc) {
        printf("ERRORE join thread confezionamento codice %d\n", rc);
    }

    return 0;
}
