/*
* Si calcoli il massimo in un insieme di valori interi 
* di N elementi, memorizzati in un vettore V. 
* Si vuole affidare la ricerca del massimo a un 
* insieme di M thread concorrenti, ognuno dei quali 
* si dovrà occupare della ricerca del massimo in una 
* porzione del vettore di dimensione K data (M=N/
* K). 
* Il thread iniziale dovrà quindi: 
* –  Inizializzare il vettore V con N valori casuali; 
* –  Creare gli M thread concorrenti; 
* –  Ricercare il massimo tra gli M risultati ottenuti 
* dai thread e stamparne il valore.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_RND_NUM 50  //Massimo numero random
#define NUM_EL 10       //Numero di elementi
#define NUM_TH 5        //Numero di Thread

int el_num = NUM_EL / NUM_TH;

void* thread_work(void* t){
    int* elements = (int*) t;
    int i;
    long result=0;

    for(i=0; i < el_num; i++){
        if(elements[i] > result){
            result = elements[i];
        }
    }

    pthread_exit((void*) result); 
}

void main (int argc, char* argv[]){

    int i, elements[NUM_EL], rc;
    void* status;
    pthread_t thread[NUM_TH];

    printf("Elements number: %d\nMax random number: %d\nThreads number: %d\n", NUM_EL, MAX_RND_NUM, NUM_TH);

    //Inizializzazione del vettore
    printf("\nElements List:\n");
    srand(time(NULL));
    for(i=0; i < NUM_EL; i++){
        elements[i]= rand() % MAX_RND_NUM;
        printf("%d\t", elements[i]);
    }

    //Creazione Threads
    for(i=0; i < NUM_TH ; i++){
        rc = pthread_create(&thread[i], NULL, thread_work, (void *)elements + sizeof(int)*el_num*i); 
        if (rc) { 
            printf("ERRORE: %d\n", rc); 
            exit(-1);
        }
    }

    printf("\n\n");
    //Join Threads and calculate maximum
    long max=0;
    for(i=0; i < NUM_TH ; i++){
        rc = pthread_join(thread[i], &status); 
        if (rc){
            printf("ERRORE join thread %d codice %d\n", i, rc); 
        }else{
            printf("Finito thread con ris. %ld\n",(long)status);
            if((long)status > max){
                max = (long)status;
            }
        }
    }

    //Result
    printf("\nMaximum is: %lu", max);

}
