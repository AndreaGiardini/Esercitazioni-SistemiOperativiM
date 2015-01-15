/*
 * Si consideri la piccola società di trasporto aereo low-cost AIRBOLO e il problema di
 * organizzare la partenza del volo charter Bologna-Zanzibar, per il quale sono registrati
 * N passeggeri.
 * Per minimizzare i costi, AIRBOLO ha affidato le attività pre-imbarco ad un solo
 * addetto.
 * Data la scarsità di personale a disposizione, AIRBOLO ha organizzato tali attività
 * presso il gate d’imbarco adottando i seguenti criteri.
 * In particolare, l’addetto di AIRBOLO:
 *
 * 1. Prima verifica i documenti (carta d’imbarco) di tutti gli N viaggiatori registrati
 *    per il volo;
 * 2. Una volta terminato il controllo documenti di tutti i viaggiatori, l’addetto di
 *    AIRBOLO deposita in un grande cesto tutti i kit di viaggio per i viaggiatori (il
 *    kit è una confezione omaggio contenente uno snack e una bibita), dal quale
 *    ogni viaggiatore preleverà il proprio. A questo proposito si assuma che:
 *    
 *    a. i kit vengano depositati nel cesto dall’addetto uno alla volta.
 *    b. Il cesto abbia una capacità limitata pari a MAX_C kit (MAX_C<N).
 *
 * 3. Successivamente l’addetto apre la porta che permette l’accesso dei viaggiatori
 *    all’aereo (uno alla volta).
 * 
 * Pertanto, ogni passeggero in partenza per Zanzibar si reca al gate dove osserva le
 * seguenti regole:
 *
 * 1. Si sottopone al controllo documenti;
 * 2. poi ritira (eventualmente) il proprio “kit di viaggio” dal cesto.
 * 3. Infine si imbarca sull’aereo.
 *
 * Si assuma che ogni passeggero acceda singolarmente al gate e che l’ordine con cui i
 * passeggeri si presentano al gate sia casuale.
 * Riguardo al precedente punto 2, si noti la possibilità che uno o più viaggiatori non
 * ritirino il proprio kit di viaggio: a questo proposito si assuma che il numero di
 * viaggiatori che rinunciano al kit sia sempre strettamente minore di MAX_C.
 * Si realizzi un’applicazione concorrente in c/linuxthreads che rappresenti addetto e
 * viaggiatori mediante thread concorrenti e che rispetti i vincoli dati mediante
 * un’opportuna politica di sincronizzazione realizzata tramite semafori.
 */

#include "20140626.h"

void * addetto(void * t)
{
    //per ogni passeggero
    for(int i = 0; i < N_PASSEGGERI; i++)
    {
        //aspetto nuovo passeggero
        sem_wait(&aeroporto.nuovo_passeggero_arrivato);
        
        pthread_mutex_lock(&aeroporto.lock);
        aeroporto.passeggeri_controllati++;
        pthread_mutex_unlock(&aeroporto.lock);
        
        printf("[Addetto] controllati dati passeggero %d - [Controllati in totale: %d]\n", i, aeroporto.passeggeri_controllati);
        sleep(rand()%3);
    
        sem_post(&aeroporto.nuovo_passeggero_servito);
    }
    
    printf("[Addetto] tutti i documenti controllati! Inizio a preparare le ceste\n");
    
    for(int i = 0; i < N_PASSEGGERI; i++)
    {
        while(aeroporto.cesta_n == MAX_CESTO)
            pthread_cond_wait(&aeroporto.cesta_piena, &aeroporto.lock);
        
        sem_wait(&aeroporto.cesta_da_ritirare);
        printf("[Addetto] preparo cesto %d\n", i);
        sleep(rand()%2);
        
        pthread_mutex_lock(&aeroporto.lock);
        aeroporto.cesta_n++;
        pthread_mutex_unlock(&aeroporto.lock);
        
        printf("[Addetto] cesta %d pronta\n", i);
        sem_post(&aeroporto.cesta_ritirata);
    }
    
    printf("[Addetto] finito di preparare le ceste! mi metto in attesa al gate\n");
    
    for(int i = 0; i < N_PASSEGGERI; i++)
    {
        sem_wait(&aeroporto.passeggero_deve_entrare);
        aeroporto.passeggeri_entrati++;
        sem_post(&aeroporto.passeggero_entrato);
    }
    pthread_exit(NULL);
}

void * passeggero(void * t)
{
    int tid = (int)(intptr_t) t;
    int cesta;
    
    sem_wait(&aeroporto.nuovo_passeggero_servito);
    printf("[Passeggero %d] arrivato al check-in\n", tid);
    sleep(rand()%2);
    sem_post(&aeroporto.nuovo_passeggero_arrivato);
    
    sleep(1);
    
    pthread_mutex_lock(&aeroporto.lock);
    if(aeroporto.passeggeri_controllati == N_PASSEGGERI)
    {
        //sono l'ultimo! sblocco la prossima fase
        printf("[Passeggero %d] sono l'ultimo passeggero ad effettuare il check-in! via alle ceste\n", tid);
        
        sem_post(&aeroporto.fase_ceste);
    }
    pthread_mutex_unlock(&aeroporto.lock);
    
    sem_wait(&aeroporto.fase_ceste);
    sem_post(&aeroporto.fase_ceste);
    
    sem_wait(&aeroporto.cesta_ritirata);
    
    //inizia la fase delle ceste
    cesta = rand()%2;
    if(cesta % 3 == 0)
    {
        aeroporto.cesta_n--;
        printf("[Passeggero %d] ho preso la cesta\n", tid);
        pthread_cond_signal(&aeroporto.cesta_piena);
    }
    else
    {
        printf("[Passeggero %d] non prendo la cesta\n", tid);
    }
    
    aeroporto.passeggeri_ceste++;
    sem_post(&aeroporto.cesta_da_ritirare);
    
    pthread_mutex_lock(&aeroporto.lock);
    if(aeroporto.passeggeri_ceste == N_PASSEGGERI)
    {
        //sono l'ultimo sblocco l'ultima fase
        printf("[Passeggero %d] sono l'ultima a prendere la cesta. Via all'ultima fase\n", tid);
        sem_post(&aeroporto.fase_finale);
    }
    pthread_mutex_unlock(&aeroporto.lock);
    
    sem_wait(&aeroporto.fase_finale);
    sem_post(&aeroporto.fase_finale);
    
    //fase finale
    sem_wait(&aeroporto.passeggero_entrato);
    printf("[Passeggero %d] entro nell'aereo\n", tid);
    sem_post(&aeroporto.passeggero_deve_entrare);
    
    pthread_exit(NULL);
}


void init(Aeroporto * aeroporto)
{
    sem_init(&aeroporto->nuovo_passeggero_servito, 0, 1);
    sem_init(&aeroporto->nuovo_passeggero_arrivato, 0, 0);
    sem_init(&aeroporto->passeggero_entrato, 0, 1);
    sem_init(&aeroporto->passeggero_deve_entrare, 0, 0);
    sem_init(&aeroporto->cesta_ritirata, 0, 1);
    sem_init(&aeroporto->cesta_da_ritirare, 0, 0);

    sem_init(&aeroporto->fase_ceste, 0, 0);
    sem_init(&aeroporto->fase_finale, 0, 0);
    
    pthread_mutex_init(&aeroporto->lock, NULL);
    pthread_mutex_init(&aeroporto->cesta, NULL);
    
    aeroporto->cesta_n = 0;
    aeroporto->passeggeri_controllati = 0;
    aeroporto->passeggeri_ceste = 0;
}


int main(int argc, char *  argv[])
{
    int result;
    void * status;
    
    init(&aeroporto);
    
    printf("Aeroporto avviato.\n");
    
    printf("[Aeroporto] %d passeggeri in arrivo...\n", N_PASSEGGERI);
    for(int i = 0; i < N_PASSEGGERI; i++)
    {
        result = pthread_create(&passeggeri_threads[i], NULL, passeggero, (void *)(intptr_t) i);
        if(result)
        {
            printf("Errore nella creazione del thread PASSEGGERO %d con stato %d\n", i, result);
            exit(-1);
        }
    }
    
    printf("[Aeroporto] Arriva l'addetto...\n");
    result = pthread_create(&addetto_thread, NULL, addetto, NULL);
    if(result)
    {
        printf("Errore nella creazione del thread ADDETTO con stato %d\n", result);
        exit(-1);
    }

    
    //join dei threads
    for(int i = 0; i < N_PASSEGGERI; i++)
    {
        result = pthread_join(passeggeri_threads[i], status);
        if(result)
        {
            printf("Errore nel join del thread %d con stato %d e risultato %d\n", i, result, (int)(intptr_t)status);
        }
    }
    
    result = pthread_join(addetto_thread, status);
    if(result)
    {
        printf("Errore nel join del thread con stato %d e risultato %d\n", result, (int)(intptr_t)status);
    }
    
    printf("Fine dell'Aeroporto. Buon viaggio!\n");
}