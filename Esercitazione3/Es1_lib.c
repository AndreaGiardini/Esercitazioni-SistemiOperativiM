#include <Es1_lib.h>
#include <stddef.h>

void init(pista *ps){
	int i;
	ps->PinPista=0;
	ps->EinPista=0;
	pthread_mutex_init (ps->lock, NULL);
	for (i=0; i < maxG; i++){
		pthread_cond_init (ps->CodaE_IN[i], NULL);
		pthread_cond_init (ps->CodaP_IN[i], NULL);
		pthread_cond_init (ps->CodaP_OUT[i], NULL);
		ps->sospCodaE_IN[i] = 0;
		ps->sospCodaP_IN[i] = 0;
		ps->sospCodaP_OUT[i] = 0;
	}
}
