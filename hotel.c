
/*
Hospedes: N?
Quartos: 10
Entregador: ??
Camareira: ??

Descrição do problema:
Pessoas estão que estão no quarto podem solicitar pedidos. Pessoas quando liberam os quartos, a camareira deve passar e limpar antes de um novo hospede entrar.

HOSPEDE:
- Pode escolher um quarto
- Pode realizar um pedido para o Serviço do Hotel
    - Água
    - Alimento
        - Suco de Uva
        - Barrinha de Cereal
        - Chocolate
        - Saguadin
    - Ir em bora

SERVIÇO DO HOTEL:
- Alocar um hospede em um quarto
- Realizar as demandas dos hospedes:
    - Entregar os pedidos
    - Desalocar um hospede do quarto e solicitar para a camareira limpar o quarto

CAMAREIRA:
- Quando um quarto for liberado por um cliente, ela deve ir e limpar o quarto.
- Após limpar o quarto, deve avisar o serviço do hotel que o quarto está disponível

*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N_HOSPEDES 5

void *hospedes (void *arg);
void *camareira (void *arg);

int main () {
	int i;
	int *id;

	// Criacao das threads de Hospedes
    pthread_t r[N_HOSPEDES];

    for (i = 0; i < N_HOSPEDES; i++) {
	    id = (int *) malloc(sizeof(int));
	    *id = i;
	    pthread_create(&r[i], NULL, hospedes, (void*) (id));
	}

    pthread_join(r[0],NULL);
	return 0;
}

void * hospedes (void *arg) {
	
    int id_hospede = *((int *) arg);

	while(1) {
		// Hospede quer um quarto
		printf("Hospede %d quer reservar um quarto...\n", id_hospede);
		sleep(5);
	} 
}