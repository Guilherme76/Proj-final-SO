#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

sem_t empty, full, mutex;

int next_item[3] = {100, 200, 300}; // Pr�ximo item a ser produzido por cada produtor

void *produtor(void *param) {
    int producer_id = *((int *)param);

    while (1) {
        sem_wait(&empty); // Aguarda se o buffer est� vazio
        sem_wait(&mutex); // Aguarda acesso exclusivo ao buffer

        int item = next_item[producer_id - 1]; // Pega o pr�ximo item a ser produzido pelo produtor
        buffer[in] = item; // Coloca o item no buffer
        printf("p%d produziu %d\n", producer_id, item); // Exibe a produ��o do produtor
        in = (in + 1) % BUFFER_SIZE; // Atualiza o �ndice de entrada do buffer

        next_item[producer_id - 1]++; // Atualiza o pr�ximo item a ser produzido pelo produtor

        sem_post(&mutex); // Libera o acesso exclusivo ao buffer
        sem_post(&full); // Avisa que o buffer est� cheio para os consumidores

        sleep(1); // Para simular o tempo de produ��o
    }
}

void *consumidor(void *param) {
    int consumer_id = *((int *)param);

    while (1) {
        sem_wait(&full); // Aguarda se o buffer est� cheio
        sem_wait(&mutex); // Aguarda acesso exclusivo ao buffer

        int item = buffer[out]; // Pega o item do buffer
        printf("                   c%d consumiu %d\n", consumer_id, item); // Exibe o consumo do consumidor
        out = (out + 1) % BUFFER_SIZE; // Atualiza o �ndice de sa�da do buffer

        sem_post(&mutex); // Libera o acesso exclusivo ao buffer
        sem_post(&empty); // Avisa que o buffer est� vazio para os produtores

        sleep(1); // Para simular o tempo de consumo
    }
}

int main() {
    pthread_t produtores[3];
    pthread_t consumidores[2];
    int i, id[3], idc[2];

    sem_init(&empty, 0, BUFFER_SIZE); // Inicializa o sem�foro empty com o tamanho do buffer
    sem_init(&full, 0, 0); // Inicializa o sem�foro full com 0 (buffer vazio inicialmente)
    sem_init(&mutex, 0, 1); // Inicializa o sem�foro mutex com 1 (para garantir acesso exclusivo ao buffer)

    // Cria threads para produtores
    for (i = 0; i < 3; i++) {
        id[i] = i + 1;
        pthread_create(&produtores[i], NULL, produtor, &id[i]);
    }

    // Cria threads para consumidores
    for (i = 0; i < 2; i++) {
        idc[i] = i + 1;
        pthread_create(&consumidores[i], NULL, consumidor, &idc[i]);
    }

    // Aguarda as threads de produtores terminarem
    for (i = 0; i < 3; i++) {
        pthread_join(produtores[i], NULL);
    }

    // Aguarda as threads de consumidores terminarem
    for (i = 0; i < 2; i++) {
        pthread_join(consumidores[i], NULL);
    }

    sem_destroy(&empty); // Destr�i o sem�foro empty
    sem_destroy(&full); // Destr�i o sem�foro full
    sem_destroy(&mutex); // Destr�i o sem�foro mutex

    return 0;
}
