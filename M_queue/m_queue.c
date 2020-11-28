#include "pingpong.h"

//Observações:
//Filas de mensagens são na verdade buffers limitados acessados por tarefas que se comportam como produtoras e consumidoras de mensagens.
//A fila de mensagens em si é uma região crítica que deve ser protegida de eventuais condições de disputa. Para isso, você deve utilizar os semáforos desenvolvidos no módulo anterior em sua implementação.

//Inicializa a fila de mensagens apontada por queue,
//com capacidade para receber até max mensagens de tamanho size bytes cada,
//inicialmente vazia. Retorna 0 em caso de sucesso e -1 em caso de erro.
int mqueue_create (mqueue_t *queue, int max, int size){

    if(queue == NULL)return -1;//se fila vazia

    if(max <= 0 || size <= 0) return -1;//se valor maximo menor ou igual a zero


    //aloca memoria para a fila de mensagens

    queue->mqueue = (task_t *) malloc(max * size);
	if (queue->mqueue == NULL) {
        #ifdef DEGUG
            printf ("mqueue_create: malloc retornou NULL. \n");
        #endif // DEGUG
        return -1;
    }

    //inicializa semaphoros ~^_^~

    sem_create (&queue->s_buffer, 1); // controla o acesso ao buffer (inicia em 1)

    sem_create (&queue->s_item, 0); // número de itens no buffer (inicia em 0)

    sem_create (&queue->s_vaga, max); // número de vagas livres no buffer (inicia em N)

	queue->fim = 0;
	queue->ini = 0;
	queue->n_msg = 0;
	queue->max = max;
	queue->size = size;

    return 0;
}

//Envia a mensagem apontada por msg para o fim da fila queue; esta chamada é bloqueante:
//caso a fila esteja cheia, a tarefa corrente é suspensa até que o envio possa ser feito.
//O ponteiro msg aponta para um buffer contendo a mensagem a enviar, que deve ser copiada através de funções C como bcopy ou memcpy.
//Retorna 0 em caso de sucesso e -1 em caso de erro.
int mqueue_send (mqueue_t *queue, void *msg){

    if(queue == NULL)
        return -1;

    if(msg == NULL)
        return -1;
    while(queue->fim <= queue->max){

        sem_down (&queue->s_vaga);  // aguarda uma vaga no buffer
        sem_down (&queue->s_buffer); // aguarda acesso exclusivo ao buffer
        //guarda a messagem no buffer
        queue->mqueue = memcpy(&queue->mqueue[queue->fim], (char *) msg, strlen((char *) msg)+1);
        queue->fim += strlen((char *) msg)+1;
        queue->n_msg++;
        sem_up (&queue->s_buffer); // libera o acesso ao buffer
        sem_up (&queue->s_item); // indica a presença de um novo item no buffer
        //printf ("mqueue_send.: envia produziu a msg %s , fim está em %d \n", (char *) msg, queue->fim);
        #ifdef DEBUG
            printf (" mqueue_send.: envia produziu a msg %s , fim está em %d \n", (char *) msg, queue->fim) ;
        #endif // DEBUG
    }
    return 0;
}

//Recebe uma mensagem do início da fila queue e a deposita no buffer apontado por msg; esta chamada é bloqueante:
//caso a fila esteja vazia, a tarefa corrente é suspensa até que a recepção possa ser feita.
//O ponteiro msg aponta para um buffer que irá receber a mensagem, que deve ser copiada através de funções C como bcopy ou memcpy.
//Retorna 0 em caso de sucesso e -1 em caso de erro.
int mqueue_recv (mqueue_t *queue, void *msg){
    if(queue == NULL)
        return -1;

    if(msg == NULL)
        return -1;
    while(mqueue_msgs (queue) > 0){
        sem_down (&queue->s_vaga);  // aguarda uma vaga no buffer
        sem_down (&queue->s_buffer); // aguarda acesso exclusivo ao buffer
        //guarda a messagem no buffer
        msg = memcpy((char *) msg, &queue->mqueue[queue->ini], strlen((char *) &queue->mqueue[queue->ini])+1);
        queue->ini = strlen((char *) queue->mqueue)+2;
        queue->n_msg--;
        sem_up (&queue->s_buffer); // libera o acesso ao buffer
        sem_up (&queue->s_item); // indica a presença de um novo item no buffer
        //printf ("mqueue_recv.: recebeu a msg %s , e inicio está em %d \n", (char *) msg, queue->ini);
        #ifdef DEBUG
           printf ("mqueue_recv.: recebeu a msg %s , e inicio está em %d \n", (char *) msg, queue->ini);
        #endif // DEBUG
    }
}

//Encerra a fila de mensagens indicada por queue, destruindo seu conteúdo e liberando todas as tarefas que esperam mensagens dela
//(essas tarefas devem retornar das suas respectivas chamadas com valor de retorno -1). Retorna 0 em caso de sucesso e -1 em caso de erro.
int mqueue_destroy (mqueue_t *queue){
    if(queue == NULL)
        return -1;
    free(queue->mqueue);// destroi fila
    //destroi semaforos
    sem_destroy(&queue->s_buffer);
    sem_destroy(&queue->s_item);
    sem_destroy(&queue->s_vaga);

    return 0;
}

//Informa o número de mensagens presentes na fila indicada por queue.
//Retorna 0 ou +N em caso de sucesso e -1 em caso de erro.
int mqueue_msgs (mqueue_t *queue){
    if(queue == NULL)
        return -1;

    return queue->n_msg;
}

