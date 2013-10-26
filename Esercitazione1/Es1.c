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

void* thread_work(void* t){
    printf("LOL");
}

int main (int argc, char* argv[]){
    int i, elements[NUM_EL];
    
    printf("Elements number: %d\nMax random number: %d\nThreads number: %d\n", NUM_EL, MAX_RND_NUM, NUM_TH);
    
    //Inizializzazione del vettore
    printf("\nElements List:\n");
    srand(time(NULL));
    for(i=0; i < NUM_EL; i++){
        elements[i]= rand() % MAX_RND_NUM;
        printf("%d\t", elements[i]);
    }
    
    printf("\n");
    
}
