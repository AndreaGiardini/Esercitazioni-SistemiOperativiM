#include "Lib.h"

int winner = 0;

/*
* Inizializzazione variabili
*/
void init(){
	/*
	* Inizializzazione Mutex e voti
    */
    int i, j;
    for(i=0; i < K; i++){
        pthread_mutex_init (&sondaggi[i].lock, NULL);
        for(j=0; j < NUM_THREADS; j++){
            sondaggi[i].voti[j]=0;
        }
    }
    pthread_mutex_init (&vediFilm, NULL);

    /*
    * Barriera Init
    */
    sem_init(&B.mb,0,1);
    sem_init(&B.sb,0,0);
    B.arrivati=0;
    
}

/*
* Sincronizzazione barriera
*/

void sync_barriera(){
    sem_wait(&B.mb);
    B.arrivati++; 
    if (B.arrivati==NUM_THREADS){
        /*
        * Prima di sbloccare calcolo il film vincitore
        */
        stampaVoti();
        sem_post(&B.sb);
    }
    sem_post(&B.mb);
    sem_wait(&B.sb); 
    sem_post(&B.sb);
    return;
}

void stampaVoti(){
    int i,j;
    float avg[K];
    /*
    * Stampa voti e media
    */
    float sum;
    for(i=0; i < K; i++){
        sum=0;
        printf("\nFilm con titolo: %d\n", i);
        printf("voti: ");
        for(j=0; j < NUM_THREADS; j++){
            printf("\t%d", sondaggi[i].voti[j]);
            sum += sondaggi[i].voti[j];
        }
        printf("\tAverage: %.2f\n", sum / NUM_THREADS);
        avg[i] = sum / NUM_THREADS;
    }

    /*
    * Decreto il vincitore
    */
    int index = -1;
    int max = 0;
    for(i=0; i < K; i++){
        if(avg[i] > max){
            index = i;
            max = avg[i];
        }
    }
    winner = index;
}

