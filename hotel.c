
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
#define N_CAMAREIRAS 2
#define N_ENTREGADORES 1

void *hospedes (void *arg);
void *camareiras (void *arg);
void *entregadores (void *arg);
void *servico_hotel (void *arg);

// Variaveis de sincronizacao
pthread_mutex_t mutex_quartos = PTHREAD_MUTEX_INITIALIZER;

sem_t semaforo_servico_hotel;
sem_t semaforo_camareiras;
sem_t semaforo_entregadores;


// Variaveis globais
int quartos[N_HOSPEDES];
int pedidos[N_HOSPEDES];



// Funcao principal para executar o programa
int main () {
	int i;
	int *id;


    // Inicializacao dos semaforos
    sem_init(&semaforo_camareiras, 0, 0);
    // sem_init(&semaforo_servico_hotel, 0, 1);
    // sem_init(&semaforo_entregadores, 0, 0);


	// Criacao das threads de Hospedes
    pthread_t hospede[N_HOSPEDES];

    for (i = 0; i < N_HOSPEDES; i++) {
	    id = (int *) malloc(sizeof(int));
	    *id = i;
	    pthread_create(&hospede[i], NULL, hospedes, (void*) (id));
	}


    // Criacao das threads de Camareiras
    pthread_t camareira[N_CAMAREIRAS];
    for (i = 0; i < N_CAMAREIRAS; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&camareira[i], NULL, camareiras, (void*) (id));
    }


    // Criacao da thread de Entregador
    pthread_t entregador[N_ENTREGADORES];
    for (i = 0; i < N_ENTREGADORES; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&entregador[i], NULL, entregadores, (void*) (id));
    }


    pthread_join(hospede[0],NULL);
	return 0;
}


void realizar_pedido(int id_hospede, int num_pedido) {
    int pedido_realizado = 0;

}

void alocar_quarto(int id_hospede) {
    int num_quarto;
    int quarto_alocado = 0;

    printf("Vamos pesquisar se existe algum quarto disponível para o hospede %d. \n", id_hospede);

    // Procura um quarto vazio
    for (num_quarto = 0; num_quarto < N_HOSPEDES; num_quarto++) {
        if (quartos[num_quarto] == 0) {
            quartos[num_quarto] = id_hospede;
            quarto_alocado = 1;
            break;
        }
    }


    sleep(2);
    if (quarto_alocado) {
        printf("Hospede %d alocou o quarto %d\n", id_hospede, num_quarto);
    } else {
        printf("Hospede %d não conseguiu alocar um quarto\n", id_hospede);
    }
}


void * hospedes (void *arg) {
	
    int id_hospede = *((int *) arg);

	while(1) {

        // Hospede quer um quarto
        pthread_mutex_lock(&mutex_quartos);
            printf("Hospede %d quer reservar um quarto...\n", id_hospede);
            alocar_quarto(id_hospede);
        pthread_mutex_unlock(&mutex_quartos);

		sleep(5);
	} 
}

void * camareiras (void *arg) {
    
    int id_camareira = *((int *) arg);

    while(1) {
        printf("Camareira %d está esperando um quarto para limpar...\n", id_camareira);
        sem_wait(&semaforo_camareiras);

        printf("Camareira %d está limpando o quarto...\n", id_camareira);
        sleep(5);
    } 
}

void * entregadores (void *arg) {
    
    int id_entregador = *((int *) arg);

    while(1) {
        // Entregador quer entregar um pedido
        printf("Entregador %d quer entregar um pedido...\n", id_entregador);
        sleep(5);
    } 
}