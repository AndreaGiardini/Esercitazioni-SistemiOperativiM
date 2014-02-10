#include "Lib.h"
/*
int piuPrio(int matt, int tipo, int dim){
    int i;
    if (dim == 1){
        return -1;
    }
    for(i = dim-2; i >= 0; i--){
        if(F.sosp[matt][tipo][i] > 0){
            return i;
        }
    }
    return -1;
}
*/

int piuPrio(int matt, int tipo, int dim){
    int i;
    if (dim == 0){
        return -1;
    }
    for(i = 0; i < dim-1; i++){
        if(F.sosp[matt][tipo][i] > 0){
            return i;
        }
    }
    return -1;
}

void segnala(){
    int ret = piuPrio(T2, STANDARD, MaxG);
    if(ret != -1){
        printf("LOL1 - ret : %d\n", ret);
        pthread_cond_signal(&F.coda[T2][STANDARD][ret]); return;
    }
    ret = piuPrio(T2, SPECIALI, MaxG);
    if(ret != -1){
        printf("LOL2 - ret : %d\n - Temperatura: %d - InForno: %d\n", ret, F.temperatura, F.inForno);
        pthread_cond_signal(&F.coda[T2][SPECIALI][ret]); return;
    }
    ret = piuPrio(T1, STANDARD, MaxG);
    if(ret != -1){
        printf("LOL3 - ret : %d\n", ret);
        pthread_cond_signal(&F.coda[T1][STANDARD][ret]); return;
    }
    ret = piuPrio(T1, SPECIALI, MaxG);
    if(ret != -1){
        printf("LOL4 - ret : %d\n", ret);
        pthread_cond_signal(&F.coda[T1][SPECIALI][ret]); return;
    }
}

boolean fornoPieno(){
    if (F.inForno == MAXF){
        return true;
    } else {
        return false;
    }
}

boolean fornoVuoto(){
    return !fornoPieno();
}

boolean inAttesa(int mattonelle, int tipo){
    int i;
    for(i = 0; i < MaxG; i++){
        if(F.sosp[mattonelle][tipo][i] > 0){
            //pthread_cond_signal(&F.coda[mattonelle][tipo][i]);
            return true;
        }
    }
    return false;
}

void thread_T1(int tid, int tipo, int dim){
    pthread_mutex_lock(&F.lock);
    if(tipo == STANDARD){
        /*
        * MATTONELLE T1 STANDARD - Sospendono se:
        * - Ci sono T2 (standard/speciali) in attesa
        * - La temperatura è bassa ma non c'è posto
        * - La temperatura è alta e c'è almeno una piastrella (se è vuoto posso cambiare temperatura)
        * - Ci sono gruppi più prioritari
        */
        while(inAttesa(T2, STANDARD) || inAttesa(T2, SPECIALI) || (F.temperatura == TEMPBASSA && F.inForno + dim > MAXF) || (F.temperatura == TEMPALTA && F.inForno > 0) || piuPrio(T1, tipo, dim) != -1){
            F.sosp[T1][STANDARD][dim-1]++;
            segnala();
            pthread_cond_wait(&F.coda[T1][STANDARD][dim-1], &F.lock);
            F.sosp[T1][STANDARD][dim-1]--;
        }

        //Entrata forno
        F.temperatura = TEMPBASSA;
        F.inForno += dim;
        printf("Tid %d - %d mattonelle T1 STANDARD - ENTRATA\n", tid, dim);
        segnala();
        pthread_mutex_unlock(&F.lock);

        //Cottura per un periodo random di secondi
        sleep((rand() % 5) + 1);

        //Uscita forno
        pthread_mutex_lock(&F.lock);
        F.inForno -= dim;
        printf("Tid %d - %d mattonelle T1 STANDARD - USCITA\n", tid, dim);
        segnala();
        //pthread_mutex_unlock(&F.lock);
    } else {
        //printf("Tid %d - %d mattonelle T1 SPECIALI", tid, dim);

        /*
        * MATTONELLE T1 SPECIALI - Sospendono se:
        * - Ci sono T2 (standard/speciali) in attesa
        * - Ci sono T1 standard in attesa
        * - La temperatura è bassa ma non c'è posto
        * - La temperatura è alta e c'è almeno una piastrella (se è vuoto posso cambiare temperatura)
        * - Ci sono gruppi più prioritari
        */
        while(inAttesa(T2, STANDARD) || inAttesa(T2, SPECIALI) ||  inAttesa(T1, STANDARD) || (F.temperatura == TEMPBASSA && F.inForno + dim > MAXF) || (F.temperatura == TEMPALTA && F.inForno > 0) || piuPrio(T1, tipo, dim) != -1){
            F.sosp[T1][SPECIALI][dim-1]++;
            segnala();
            pthread_cond_wait(&F.coda[T1][STANDARD][dim-1], &F.lock);
            F.sosp[T1][SPECIALI][dim-1]--;
        }

        //Entrata forno
        F.temperatura = TEMPBASSA;
        F.inForno += dim;
        printf("Tid %d - %d mattonelle T1 SPECIALI - ENTRATA\n", tid, dim);
        segnala();
        pthread_mutex_unlock(&F.lock);

        //Cottura per un periodo random di secondi
        sleep((rand() % 5) + 1);

        //Uscita forno
        pthread_mutex_lock(&F.lock);
        F.inForno -= dim;
        printf("Tid %d - %d mattonelle T1 SPECIALI - USCITA\n", tid, dim);
        segnala();
        //pthread_mutex_unlock(&F.lock);

    }

    pthread_mutex_unlock(&F.lock);
}

void thread_T2(int tid, int tipo, int dim){
    pthread_mutex_lock(&F.lock);
        if(tipo == STANDARD){
        /*
        * MATTONELLE T2 STANDARD - Sospendono se:
        * - La temperatura è alta ma non c'è posto
        * - La temperatura è bassa e c'è almeno una piastrella (se è vuoto posso cambiare temperatura)
        * - Ci sono gruppi più prioritari
        */
        while((F.temperatura == TEMPALTA && F.inForno + dim > MAXF) || (F.temperatura == TEMPBASSA && F.inForno > 0) || piuPrio(T2, tipo, dim) != -1){
            F.sosp[T2][STANDARD][dim-1]++;
            printf("YUPPP T2 STANDARD - %d - %d - %d - %d\n", (F.temperatura == TEMPALTA && F.inForno + dim > MAXF), (F.temperatura == TEMPBASSA && F.inForno > 0), piuPrio(T2, tipo, dim), dim);
            segnala();
            pthread_cond_wait(&F.coda[T2][STANDARD][dim-1], &F.lock);
            F.sosp[T2][STANDARD][dim-1]--;
        }

        //Entrata forno
        F.temperatura = TEMPALTA;
        F.inForno += dim;
        printf("Tid %d - %d mattonelle T2 STANDARD - ENTRATA\n", tid, dim);
        segnala();
        pthread_mutex_unlock(&F.lock);

        //Cottura per un periodo random di secondi
        sleep((rand() % 5) + 1);

        //Uscita forno
        pthread_mutex_lock(&F.lock);
        F.inForno -= dim;
        printf("Tid %d - %d mattonelle T2 STANDARD - USCITA\n", tid, dim);
        segnala();
        //pthread_mutex_unlock(&F.lock);
    } else {
        /*
        * MATTONELLE T2 SPECIALI - Sospendono se:
        * - Ci sono T2 standard in attesa
        * - La temperatura è alta ma non c'è posto
        * - La temperatura è bassa e c'è almeno una piastrella (se è vuoto posso cambiare temperatura)
        * - Ci sono gruppi più prioritari
        */
        while(inAttesa(T2, STANDARD) || (F.temperatura == TEMPALTA && F.inForno + dim > MAXF) || (F.temperatura == TEMPBASSA && F.inForno > 0) || piuPrio(T1, tipo, dim) != -1){
            F.sosp[T2][SPECIALI][dim-1]++;
            printf("YUPPP T2 SPECIALI - %d - %d - %d - %d - %d\n", inAttesa(T2, STANDARD), (F.temperatura == TEMPALTA && F.inForno + dim > MAXF), (F.temperatura == TEMPBASSA && F.inForno > 0), (piuPrio(T1, tipo, dim) != -1), dim);
            segnala();
            pthread_cond_wait(&F.coda[T2][STANDARD][dim-1], &F.lock);
            F.sosp[T2][SPECIALI][dim-1]--;
        }

        //Entrata forno
        F.temperatura = TEMPALTA;
        F.inForno += dim;
        printf("Tid %d - %d mattonelle T2 SPECIALI - ENTRATA\n", tid, dim);
        if(F.temperatura == TEMPALTA){
            printf("Forno - Mattonelle %d - Temperatura alta", F.inForno);
        } else {
            printf("Forno - Mattonelle %d - Temperatura bassa", F.inForno);
        }
        segnala();
        pthread_mutex_unlock(&F.lock);

        //Cottura per un periodo random di secondi
        sleep((rand() % 5) + 1);

        //Uscita forno
        pthread_mutex_lock(&F.lock);
        F.inForno -= dim;
        printf("Tid %d - %d mattonelle T2 SPECIALI - USCITA\n", tid, dim);
        segnala();
        //pthread_mutex_unlock(&F.lock);

    }
    pthread_mutex_unlock(&F.lock);
}

void* thread_work(void* t){
    int tid = (intptr_t) t;

    /*
    * Esecuzione thread
    */
    srand(time(NULL)+tid);
    if(rand() % 2){
        thread_T1(tid, rand() % 2, (rand() % MaxG) +1);
    } else {
        thread_T2(tid, rand() % 2, (rand() % MaxG) +1);
    }

    pthread_exit((void*)(intptr_t) tid);
}

int main(int argc, char * argv[]){

    int i, ret_code;

    void* status;
    pthread_t thread[NUM_THREADS];

    /*
    * Inizializzazione risorse
    */
    init();

    /*
    * Creazione threads
    */
    for(i=0; i < NUM_THREADS; i++){
        ret_code=pthread_create(&thread[i], NULL, thread_work, (void *) (intptr_t) i);
        if (ret_code) { 
            printf("ERRORE: %d\n", ret_code); 
            exit(EXIT_FAILURE);
        }
    }

    /*
    * Join threads
    */
    for(i=0; i < NUM_THREADS; i++){
        ret_code=pthread_join(thread[i], &status);
        if (ret_code){
            printf("ERRORE join thread %d codice %d\n", i, ret_code); 
        }
        //printf("Finito thread con ris. %d\n",(int)(intptr_t)status);
    }


    return EXIT_SUCCESS;
}
