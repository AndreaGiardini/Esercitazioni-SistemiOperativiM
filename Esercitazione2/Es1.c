#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define N 10
#define K 5

typedef struct sondaggio{
    int num; //Numero sondaggio (di K)
    int voti[N];
} sondaggio;

static sem_t semph[K];
sondaggio sondaggi[K];

void * user(void * t){

    int i, k;
    
    srand(time(NULL));
    //Per ogni film attendo il semaforo e scrivo un voto
    for(i=0; i < K; i++){
        sem_wait(&semph[i]);
        for(k=0; k < N; k++){
            if(sondaggi[i].voti[k] == -1){
                sondaggi[i].voti[k] = rand() % 10 + 1;
                //printf("Voto: %d - con i = %d e K = %d\n", sondaggi[i].voti[k], i , k);
                break;
            }
        }
        sem_post(&semph[i]);        
    }

    pthread_exit (0); 

}

int main (int argc, char * argv[]){

    int i, j;
    pthread_t thread[N];
    
    //Inizializzo i semafori
    for(i=0; i < K; i++){
        sem_init(&semph[i], 0, 1);
    }
    
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
    for(i=0; i < K; i++){
        pthread_join (thread[i], NULL);
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
