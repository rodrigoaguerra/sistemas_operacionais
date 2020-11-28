#include "pingpong.h"
//#define DEBUG

int mqueue_create (mqueue_t *queue, int max, int size){

    if(queue == NULL) return -1;//se fila vazia

    if(max <= 0 || size <= 0) return -1;//se valor maximo menor ou igual a zero

    //inicializa semaphoros ~^_^~

    sem_create (&queue->s_buffer, 1); // controla o acesso ao buffer (inicia em 1)

    sem_create (&queue->s_item, 0); // número de itens no buffer (inicia em 0)

    sem_create (&queue->s_vaga, max); // número de vagas livres no buffer (inicia em N)

	queue->fim = 0; //final da ultima msg
	queue->ini = 0; //inicio da primeira msg
	queue->n_msg = 0; //numero de msgs
	queue->max = max; // numero maximo de mensagens
	queue->size = size; // tamanho da mensagem

    queue->tam_buffer =  max * size; // tamanho do buffer total

    //aloca memoria para a fila de mensagens
    queue->mqueue = (void*) malloc(queue->tam_buffer);

	if (queue->mqueue == NULL) {
        #ifdef DEGUG
            printf ("mqueue_create: malloc retornou NULL. \n");
        #endif // DEGUG
        return -1;
    }

    return 0;
}

int mqueue_send (mqueue_t *queue, void *msg){

    if(queue == NULL)
        return -1;

    if(msg == NULL)
        return -1;

    if(sem_down (&queue->s_vaga) < 0) return -1;  // aguarda uma vaga no buffer
    if(sem_down (&queue->s_buffer) < 0) return -1; // aguarda acesso exclusivo ao buffer

    bcopy(msg, &queue->mqueue[queue->fim], queue->size);//guarda a mensagem no final do buffer

    queue->fim = (queue->fim + queue->size) % queue->tam_buffer; //movimenta o indicador do fim para o novo final

    queue->n_msg++;//incrementa as quantidade de mensagens

    sem_up (&queue->s_buffer); // libera o acesso ao buffer
    sem_up (&queue->s_item); // indica a presença de um novo item no buffer

    #ifdef DEBUG
        printf (" mqueue_send.: Numero de msg: %d \n", mqueue_msgs(queue)) ;
        printf (" mqueue_send.: envia produziu a msg %d , fim está em %d \n", (int) msg, queue->fim) ;
    #endif // DEBUG

    return 0;
}

int mqueue_recv (mqueue_t *queue, void *msg){
    if(queue == NULL)
        return -1;

    if(msg == NULL)
        return -1;

    if(sem_down(&queue->s_item) < 0) return -1;  // aguarda um novo item no buffer
    if(sem_down(&queue->s_buffer) < 0) return -1; // aguarda acesso exclusivo ao buffer

    bcopy(&queue->mqueue[queue->ini], msg, queue->size);
    queue->ini = (queue->ini + queue->size) % queue->tam_buffer;

    queue->n_msg--;

    sem_up (&queue->s_buffer); // libera o acesso ao buffer
    sem_up (&queue->s_vaga); // indica a liberação de uma vaga no buffer

    #ifdef DEBUG
       printf (" mqueue_recv.: Numero de msg: %d \n", mqueue_msgs(queue)) ;
       printf ("mqueue_recv.: recebeu a msg %s , e inicio está em %d \n", (int *) msg, queue->ini);
    #endif // DEBUG

    return  0;
}

int mqueue_destroy (mqueue_t *queue){
    if(queue == NULL)
        return -1;

    //destroi semaforos
    sem_destroy(&queue->s_buffer);
    sem_destroy(&queue->s_item);
    sem_destroy(&queue->s_vaga);

    free(queue->mqueue);// destroi fila

    queue = NULL;

    return 0;
}

int mqueue_msgs (mqueue_t *queue){
    if(queue == NULL)
        return -1;

    return queue->n_msg;
}

