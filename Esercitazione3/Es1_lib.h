#ifndef ES1_LIB_H
#define ES1_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//Numero istruttori
#define NI 5
//Numero massimo di persone nella pista
#define MAX 20
//Numero massimo di persone nel gruppo
#define maxG 5

typedef enum { false, true } boolean;

typedef struct{
	pista ps;
	int num;
} args;

typedef struct{
	int PinPista;
	int EinPista;
	pthread_mutex_t lock;
	pthread_cond_t CodaE_IN[maxG];
	pthread_cond_t CodaP_IN[maxG];
	pthread_cond_t CodaP_OUT[maxG];
	int sospCodaE_IN[maxG];
	int sospCodaP_IN[maxG];
	int sospCodaP_OUT[maxG];
} pista;

void init(pista *ps);

#endif
