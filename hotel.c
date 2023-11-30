#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N_QUARTOS 5
#define N_HOSPEDES 10
#define N_CAMAREIRAS 2
#define N_ENTREGADORES 2

void *hospedes (void *arg);
void *camareiras (void *arg);
void *entregadores (void *arg);


// Variaveis de sincronizacao
pthread_mutex_t mutex_quartos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_entregador = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_pedido = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_entregador = PTHREAD_COND_INITIALIZER;

sem_t semaforo_camareiras;
sem_t semaforo_hospedes;


// Variaveis globais
int quartos[N_QUARTOS];
int camareira_quarto[N_QUARTOS];
int hospede_quarto[N_HOSPEDES];
int hospede_pedido[N_HOSPEDES];
int totalPedidos = 0;

// Funcao principal para executar o programa
int main () {
	int i;
	int *id;

    // Inicializacao do array hospede quartos
    for (int i = 0; i < N_HOSPEDES; i++) {
        hospede_quarto[i] = -1;
    }

    // Inicializacao do array de quartos
    for (int i = 0; i < N_QUARTOS; i++) {
        quartos[i] = -1;
    }

    // Inicializacao do array de camareiras
    for (int i = 0; i < N_QUARTOS; i++) {
        camareira_quarto[i] = -1;
    }

    // Inicializacao do array de pedidos
    for (int i = 0; i < N_HOSPEDES; i++) {
        hospede_pedido[i] = -1;
    }

    // Inicializacao dos semaforos
    sem_init(&semaforo_camareiras, 0, 0);
    sem_init(&semaforo_hospedes, 0, 0);


    // Criacao das threads de Camareiras
    pthread_t camareira[N_CAMAREIRAS];
    for (i = 0; i < N_CAMAREIRAS; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&camareira[i], NULL, camareiras, (void*) (id));
    }


	// Criacao das threads de Hospedes
    pthread_t hospede[N_HOSPEDES];
    for (i = 1; i < N_HOSPEDES; i++) {
	    id = (int *) malloc(sizeof(int));
	    *id = i;
	    pthread_create(&hospede[i], NULL, hospedes, (void*) (id));
	}


    // Criacao da thread de Entregador
    pthread_t entregador[N_ENTREGADORES];
    for (i = 0; i < N_ENTREGADORES; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&entregador[i], NULL, entregadores, (void*) (id));
    }

    pthread_join(camareira[0],NULL);
	return 0;
}

int numAleatorio(int range) {
    // Define a semente para a função rand() usando o tempo atual
    srand(time(NULL));
    
    // Gera um número aleatório entre 0 e range
    return rand() % range + 1;
}


void * hospedes (void *arg) {
    int id_hospede = *((int *) arg);

    while (1) {
            if (hospede_quarto[id_hospede] == -1) {
                pthread_mutex_lock(&mutex_quartos);
                for (int numero_quarto = 0; numero_quarto < N_QUARTOS; numero_quarto++) {
                    if (quartos[numero_quarto] == -1) {
                        quartos[numero_quarto] = id_hospede;
                        hospede_quarto[id_hospede] = numero_quarto;
                pthread_mutex_unlock(&mutex_quartos);
                        printf("HOSPEDE %d: pegou o quarto %d\n", id_hospede, numero_quarto);
                        sleep(2);
                        break; // sai do loop
                    }
            }
                pthread_mutex_unlock(&mutex_quartos);
        }

        int quer_sair = numAleatorio(100);
        int pedido = 0;
        
        // SAI DO QUARTO COM 20% DE CHANCE
        if (hospede_quarto[id_hospede] != -1 && quer_sair >= 70) {
            printf("HOSPEDE %d: no quarto %d está solicitando check-out.\n", id_hospede, hospede_quarto[id_hospede]);
            sleep(2);

            pthread_mutex_lock(&mutex_quartos);
                quartos[hospede_quarto[id_hospede]] = -2;
                printf("HOSPEDE %d: liberou o quarto %d\n", id_hospede, hospede_quarto[id_hospede]);
                hospede_quarto[id_hospede] = -1;
                sem_post(&semaforo_camareiras);
            pthread_mutex_unlock(&mutex_quartos);
        }

        // REALIZA UM PEDIDO COM 20% DE
        else if (hospede_quarto[id_hospede] != -1 && (quer_sair >=60 && quer_sair < 70)) {

            pedido = numAleatorio(3);
            int num_quarto = hospede_quarto[id_hospede];

            if(pedido == 0) {

                printf("HOSPEDE %d: está pedindo comida no quarto %d\n", id_hospede, num_quarto);

                pthread_mutex_lock(&mutex_pedido);
                    totalPedidos++;
                    hospede_pedido[id_hospede] = 0;
                pthread_mutex_unlock(&mutex_pedido);
                pthread_cond_signal(&cond_entregador);

                sem_wait(&semaforo_hospedes);
                sleep(2);

            }

            if(pedido == 1) {
                    
                printf("HOSPEDE %d: está pedindo bebida no quarto %d\n", id_hospede, num_quarto);
                sleep(2);

                pthread_mutex_lock(&mutex_pedido);
                    totalPedidos++;
                    hospede_pedido[id_hospede] = 1;
                pthread_mutex_unlock(&mutex_pedido);
                pthread_cond_signal(&cond_entregador);

                sem_wait(&semaforo_hospedes);

            }

            if(pedido == 2) {
                        
                printf("HOSPEDE %d: está pedindo toalhas no quarto %d\n", id_hospede, num_quarto);
                sleep(2);

                pthread_mutex_lock(&mutex_pedido);
                    totalPedidos++;
                    hospede_pedido[id_hospede] = 2;
                pthread_mutex_unlock(&mutex_pedido);
                pthread_cond_signal(&cond_entregador);

                sem_wait(&semaforo_hospedes);

            }
        } else if (hospede_quarto[id_hospede] != -1) {
            printf("HOSPEDE %d: está dormindo....\n", id_hospede);
            sleep(3);
        } else {
            sleep(1);
        }

    }

}

void * camareiras (void *arg) {
    int id_camareira = *((int *) arg);

    while (1) {
        printf("CAMAREIRA %d: está esperando um quarto para limpar...\n", id_camareira);
        sem_wait(&semaforo_camareiras);
                    
        pthread_mutex_lock(&mutex_quartos);
            for (int numero_quarto = 0; numero_quarto < N_QUARTOS; numero_quarto++) {
                if (quartos[numero_quarto] == -2 && camareira_quarto[numero_quarto] == -1) {
                    camareira_quarto[numero_quarto] = id_camareira;
                    printf("CAMAREIRA %d: está limpando o quarto %d\n", id_camareira, numero_quarto);
                    sleep(2);
                    quartos[numero_quarto] = -1;
                    printf("CAMAREIRA %d: terminou de limpar o quarto %d\n", id_camareira, numero_quarto);
                    camareira_quarto[id_camareira] = -1;
            pthread_mutex_unlock(&mutex_quartos);
                    break;
                }
            }
        pthread_mutex_unlock(&mutex_quartos);
    }
}

void * entregadores (void *arg) {
    int id_entregador = *((int *) arg);

    while (1) {
        pthread_mutex_lock(&mutex_entregador);
            while (totalPedidos == 0) {
                printf("ENTREGADOR %d: está esperando um pedido...\n", id_entregador);
                pthread_cond_wait(&cond_entregador, &mutex_entregador);
            }

            pthread_mutex_lock(&mutex_pedido);
                totalPedidos--;
            pthread_mutex_unlock(&mutex_pedido);

            for (int i = 0; i < N_HOSPEDES; i++) {
                if (hospede_pedido[i] != -1) {

                    if(hospede_pedido[i] == 0) {
                        printf("ENTREGADOR %d: está entregando a comida para o hospede %d\n", id_entregador, i);
                    }

                    if(hospede_pedido[i] == 1) {
                        printf("ENTREGADOR %d: está entregando a bebida para o hospede %d\n", id_entregador, i);
                    }

                    if(hospede_pedido[i] == 2) {
                        printf("ENTREGADOR %d: está entregando as toalhas para o hospede %d\n", id_entregador, i);
                    }

                    pthread_mutex_lock(&mutex_pedido);
                        hospede_pedido[i] = -1;
                    pthread_mutex_unlock(&mutex_pedido);
                    sem_post(&semaforo_hospedes);
                    break;
                }
            }

        pthread_mutex_unlock(&mutex_entregador);
        sleep(2);
        
    }
}