#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N_QUARTOS 5
#define N_HOSPEDES 10
#define N_CAMAREIRAS 2

void *hospedes (void *arg);
void *camareiras (void *arg);


// Variaveis de sincronizacao
pthread_mutex_t mutex_quartos = PTHREAD_MUTEX_INITIALIZER;
sem_t semaforo_camareiras;


// Variaveis globais
int quartos[N_QUARTOS];
int camareira_quarto[N_QUARTOS];
int hospede_quarto[N_HOSPEDES];

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

    // Inicializacao dos semaforos
    sem_init(&semaforo_camareiras, 0, 0);


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
    // pthread_t entregador[N_ENTREGADORES];
    // for (i = 0; i < N_ENTREGADORES; i++) {
    //     id = (int *) malloc(sizeof(int));
    //     *id = i;
    //     pthread_create(&entregador[i], NULL, entregadores, (void*) (id));
    // }

    pthread_join(camareira[0],NULL);
	return 0;
}

int numAleatorio() {
    // Define a semente para a função rand() usando o tempo atual
    srand(time(NULL));
    
    // Gera um número aleatório entre 1 e 100
    return rand() % 100 + 1;
}


void * hospedes (void *arg) {
    int id_hospede = *((int *) arg);

    while (1) {
        pthread_mutex_lock(&mutex_quartos);
        
            for (int numero_quarto = 0; numero_quarto < N_QUARTOS; numero_quarto++) {
                if (quartos[numero_quarto] == -1) {
                    quartos[numero_quarto] = id_hospede;
                    hospede_quarto[id_hospede] = numero_quarto;
                    printf("HOSPEDE %d: pegou o quarto %d\n", id_hospede, numero_quarto);
                    sleep(2);
            pthread_mutex_unlock(&mutex_quartos);
                    break; // sai do loop
                }
        pthread_mutex_unlock(&mutex_quartos);
        }

        int quer_sair = numAleatorio();
        
        if (hospede_quarto[id_hospede] != -1 && quer_sair >= 70) {
            printf("HOSPEDE %d: quer sair do quarto %d\n", id_hospede, hospede_quarto[id_hospede]);
            pthread_mutex_lock(&mutex_quartos);

                quartos[hospede_quarto[id_hospede]] = -2;
                printf("HOSPEDE %d: saiu do quarto %d\n", id_hospede, hospede_quarto[id_hospede]);
                hospede_quarto[id_hospede] = -1;
                sem_post(&semaforo_camareiras);
            pthread_mutex_unlock(&mutex_quartos);
        } 
        else if (hospede_quarto[id_hospede] != -1 && quer_sair < 70) {
            printf("HOSPEDE %d: está dormindo...\n", id_hospede);
            sleep(5);
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