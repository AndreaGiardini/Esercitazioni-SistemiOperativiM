#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
//Numero persone
#define N 10
//Numero films
#define K 5

/*
* Un sondaggio è composto da:
*   - Un id (num) del film
*   - Un campo di voto per ognuno degli N utenti
*/
typedef struct sondaggio{
    int num;
    int voti[N];
} sondaggio;

//Inizializzo un sondaggio per ogni film
sondaggio sondaggi[K];
//Inizializzo un semaforo per ogni sondaggio
static sem_t semph[K];
static sem_t barriera;

void * user(void * t){
    int i, k;

    //Per ogni film
    for(i=0; i < K; i++){
        //Attendo che il semaforo sia libero
        sem_wait(&semph[i]);
        for(k=0; k < N; k++){
            //Scrivo un voto random nella prima cella libera
            if(sondaggi[i].voti[k] == -1){
                sondaggi[i].voti[k] = rand() % 10 + 1;
                //printf("Thread %d - Voto: %d - con i = %d e K = %d\n", pthread_self(), sondaggi[i].voti[k], i , k);
                break;
            }
        }
        sem_post(&semph[i]);
    }

    sem_post(barriera);
    //Attendo fino a quando tutti i thread non hanno valutato il film

    pthread_exit (0); 

}

int main (int argc, char * argv[]){

    int i, j;
    pthread_t thread[N];

    srand(time(NULL));

    /*
    * Inizializzo i semafori ad 1 (libero)
    */
    for(i=0; i < K; i++){
        sem_init(&semph[i], 0, 1);
    }
    //Semaforo di barriera con il numero di utenti
    sem_init(barriera, 0, N);

    //Inizializzo i sondaggi
    for(i=0; i < K; i++){
        sondaggi[i].num=i;
        for(j=0; j < N; j++){
            sondaggi[i].voti[j]=-1;
        }
    }

    //Per ogni utente avvio un processo
    for(i=0; i < N; i++){
        pthread_create(&thread[i],NULL,user, NULL);
    }

    //Attendo la fine dei processi
    for(i=0; i < N; i++){
        pthread_join(thread[i], NULL);
    }

    //Stampo i risultati
    for(i=0; i < K; i++){
        float sum=0;
        printf("Media del film numero %d:\n", sondaggi[i].num);
        printf("Voti: ");
        for(j=0; j < N; j++){
            sum += sondaggi[i].voti[j];
            printf("\t%d", sondaggi[i].voti[j]);
        }
        sum /= N;
        printf("\nMedia: %.2f\n\n", sum);
    }
}
