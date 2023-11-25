
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

#define N_QUARTOS 5
#define N_HOSPEDES 15
#define N_CAMAREIRAS 2
#define N_ENTREGADORES 1

void *hospedes (void *arg);
void *camareiras (void *arg);
void *entregadores (void *arg);
void *servico_hotel (void *arg);

// Variaveis de sincronizacao
pthread_mutex_t mutex_quartos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_camareira = PTHREAD_MUTEX_INITIALIZER;

sem_t semaforo_hospedes;
sem_t semaforo_servico_hotel;
sem_t semaforo_camareiras;
sem_t semaforo_entregadores;

pthread_barrier_t barreira_hospedes;


// Variaveis globais
int quartos[N_QUARTOS];
int hospede_quarto[N_HOSPEDES];
int camareira_quarto[N_QUARTOS];
int pedidos[N_QUARTOS];

int num_hospede_atual;



// Funcao principal para executar o programa
int main () {
	int i;
	int *id;


    // Inicializacao dos semaforos
    sem_init(&semaforo_camareiras, 0, N_CAMAREIRAS);
    sem_init(&semaforo_hospedes, 0, N_HOSPEDES);
    sem_init(&semaforo_servico_hotel, 0, 0);
    // sem_init(&semaforo_entregadores, 0, 0);


	// Criacao das threads de Hospedes
    pthread_t hospede[N_HOSPEDES];

    for (i = 1; i < N_HOSPEDES; i++) {
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


    // Criacao da thread de Servico do Hotel
    pthread_t t_servico_hotel;
    pthread_create(&t_servico_hotel, NULL, servico_hotel, NULL);


    pthread_join(camareira[0],NULL);
	return 0;
}


void linhasVazias(int n) {
    for (int i = 0; i < n; i++) {
        printf("\n");
    }
}


void realizar_pedido(int id_hospede, int num_pedido) {
    int pedido_realizado = 0;
    // preenche o array de pedidos com o pedido do cara
    pedidos[id_hospede] = num_pedido;

}

void alocar_quarto(int id_hospede) {
    int num_quarto;
    int quarto_alocado = 0;

    linhasVazias(1);
    printf("Vamos pesquisar se existe algum quarto disponível para o hospede %d. \n", id_hospede);
    linhasVazias(1);

    // Procura um quarto vazio
    for (num_quarto = 0; num_quarto < N_QUARTOS; num_quarto++) {
        if (quartos[num_quarto] == 0) {
            if (camareira_quarto[num_quarto] != -1) {
                quartos[num_quarto] = id_hospede;
                hospede_quarto[id_hospede] = num_quarto;
                quarto_alocado = 1;
                break;
            }
        }
    }

    sleep(2);
    if (quarto_alocado) {
        printf("Hospede %d alocou o quarto %d\n", id_hospede, num_quarto);

        // // Imprime os elementos do array usando um loop for
        // for (int i = 0; i < N_QUARTOS; i++) {
        //     printf("%d ", quartos[i]);
        // }
        // printf("\n"); // Adiciona uma nova linha no final

    } else {
        printf("Hospede %d não conseguiu alocar um quarto\n", id_hospede);
    }
}


int numAleatorio() {
    int numero_aleatorio;
    
    // Define a semente para a função rand() usando o tempo atual
    srand(time(NULL));
    
    // Gera um número aleatório entre 1 e 100
    numero_aleatorio = rand() % 100 + 1;

    return numero_aleatorio;
}


int estaNoArray(int elemento, int array[], int tamanho) {
    for (int i = 0; i < tamanho; ++i) {
        if (array[i] == elemento) {
            return 1; // Retorna 1 se o elemento estiver no array
        }
    }
    return 0; // Retorna 0 se o elemento não estiver no array
}


void * hospedes (void *arg) {
	
    int id_hospede = *((int *) arg);

    // Hospede quer um quarto
    pthread_mutex_lock(&mutex_quartos);
        printf("Hospede %d quer reservar um quarto...\n", id_hospede);
        num_hospede_atual = id_hospede;
        sem_post(&semaforo_servico_hotel);
    pthread_mutex_unlock(&mutex_quartos);
    
    pthread_barrier_wait(&barreira_hospedes);

    sleep(5);

    if (estaNoArray(id_hospede, hospede_quarto, N_HOSPEDES)) {
        printf("Hospede %d conseguiu reservar um quarto!\n", id_hospede);

        while (1) {
            int acao = numAleatorio();
            if (acao >= 70) {
                linhasVazias(2);
                printf("Hospede %d quer ir embora...\n", id_hospede);

                pthread_mutex_lock(&mutex_quartos);
                    int num_quarto = hospede_quarto[id_hospede];
                    quartos[num_quarto] = 0;

                    // Define o quarto que a camareira deve limpar
                    camareira_quarto[num_quarto] = -1;

                    sleep(2);

                    printf("Hospede %d liberou o quarto %d\n", id_hospede, id_hospede);
                    sem_post(&semaforo_camareiras);
                pthread_mutex_unlock(&mutex_quartos);

                break;
            } else {
                printf("Hospede %d quer realizar um pedido...\n", id_hospede);
                sleep(3);
                // linhasVazias(2);
                // printf("Hospede %d quer realizar um pedido...\n", id_hospede);
                // linhasVazias(2);

                // pthread_mutex_lock(&mutex_quartos);
                //     realizar_pedido(id_hospede, acao);
                //     printf("Hospede %d realizou o pedido %d\n", id_hospede, acao);
                // pthread_mutex_unlock(&mutex_quartos);  
            }
        }
    } else {
        printf("Hospede %d não conseguiu reservar um quarto!\n", id_hospede);
    }
    

    sleep(3);
    printf("Hospede %d foi embora...\n", id_hospede);
    pthread_exit(0);
}


void * camareiras (void *arg) {
    
    int id_camareira = *((int *) arg);

    while(1) {
    
        linhasVazias(2);
        printf("Camareira %d está esperando um quarto para limpar...\n", id_camareira);

        sem_wait(&semaforo_camareiras);

        for (int i = 0; i < N_QUARTOS; i++) {
            if (quartos[i] == 0) {
                linhasVazias(2);
                printf("Camareira %d está limpando o quarto %d...\n", id_camareira, i);
                sleep(5);
                printf("Camareira terminou de limpar o quarto %d...\n", i);
                camareira_quarto[i] = 0;
            }
            break;  
        }

        
    } 
}

void * entregadores (void *arg) {
    
    int id_entregador = *((int *) arg);

    while(1) {
        // Entregador quer entregar um pedido
        // printf("Entregador %d quer entregar um pedido...\n", id_entregador);
        sleep(5);
    } 
}

void * servico_hotel (void *arg) {
    
    while(1) {
        // Servico do Hotel quer atender um pedido
        linhasVazias(2);
            printf("Servico do Hotel está aguardando solicitações dos hospedes!\n");
            sem_wait(&semaforo_servico_hotel);

        // Servico do Hotel quer alocar um quarto
        linhasVazias(2);
            alocar_quarto(num_hospede_atual);
            sem_post(&semaforo_hospedes);
        sleep(5);
    } 
}