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

#define MAX_RND_NUM 1050    /* Massimo numero random */
#define NUM_EL 100          /* Numero di elementi */
#define NUM_TH 20           /* Numero di Thread */

/*
* Lista elementi
*/
int elements[NUM_EL];

/*
* Numero di elementi per thread
*/
int el_num[NUM_TH];

void* thread_work(void* t){
    long tid = (long) t;
    int i, displ=0;
    long result=0;

    /* Displacement */
    for(i=0; i < tid; i++){
        displ += el_num[i];
    }

    for(i=0; i < el_num[tid]; i++){
        /* printf("Thread id: %ld - Displacement: %d - Number : %d\n", tid, displ, elements[displ+i]); */
        if(elements[displ+i] > result){
            result = elements[displ+i];
        }
    }

    pthread_exit((void*) result); 
}

int main (int argc, char* argv[]){

    /*
    * i - Indice
    * rc - Esito primitive
    * status - Codici di ritorno
    * thread - Lista thread
    */
    long i;
    int rc;
    void* status;
    pthread_t thread[NUM_TH];

    printf("Numero di elementi: %d\nNumero casuale massimo: %d\nNumero di thread: %d\n", NUM_EL, MAX_RND_NUM, NUM_TH);

    /* Inizializzazione del vettore e stampa */
    printf("\nElements List:\n");
    srand(time(NULL));
    for(i=0; i < NUM_EL; i++){
        elements[i]= rand() % MAX_RND_NUM;
        printf("%d\t", elements[i]);
    }
    printf("\n");

    /* Calcolo il numero di elementi che deve processare ogni thread */
    for(i=0; i < NUM_TH; i++){
        el_num[i] = NUM_EL / NUM_TH;
    }

    if (NUM_EL % NUM_TH != 0){
        for(i=0; i < NUM_EL % NUM_TH; i++){
            el_num[i]++;
        }
    }

    /* Creazione Threads */
    for(i=0; i < NUM_TH ; i++){
        //printf("LOL - %lu\n", i);
        /* Passo al thread il suo indice */
        rc = pthread_create(&thread[i], NULL, thread_work, (void*)i); 
        if (rc) { 
            printf("ERRORE: %d\n", rc); 
            exit(-1);
        }
    }

    printf("\n");

    /* Join e calcolo del massimo */
    long max=0;
    for(i=0; i < NUM_TH ; i++){
        rc = pthread_join(thread[i], &status); 
        if (rc){
            printf("ERRORE join thread %lu codice %d\n", i, rc); 
        }else{
            printf("Finito thread con ris. %ld\n",(long)status);
            if((long)status > max){
                max = (long)status;
            }
        }
    }

    /* Risultato */
    printf("\nMaximum is: %lu", max);

    return 0;

}
