#include "datatypes.h"
#define SCHEDFIFO

void task_yield (){
    task_switch (&dispatcher);
}

task_t* scheduler(){
    //se fila_prontas vazia, retornar NULL
    if(t_queue == NULL)
       return NULL;

#ifdef SCHEDFIFO
    task_t *task_run = (task_t *)t_queue;
    t_queue = t_queue->next ;
#else // SCHEDFIFO

    //encontrar a tarefa com a mínima prioridade dinâmica na fila_prontas
    task_t *aux = (task_t *)t_queue;
    task_t *task_run = (task_t *)t_queue;

    int total = queue_size(t_queue);
    int i;
    for(i = 0; i < total; i++){
        if(aux->prio_dinamic < task_run->prio_dinamic){
            task_run = aux;
        }
        aux = aux->next;
    }

    // envelhecer a prioridade dinâmica de todas as tarefas na fila_prontas
    aux = (task_t *)t_queue;

    for(i = 0; i < total; i++){
        aux->prio_dinamic--;
        aux = aux->next;
    }

    // reiniciar valor da prio_dinamica da tarefa encontrada no loop1 (fazer ser igual à sua prio_estática)
    task_run->prio_dinamic = task_run->prio_static;
#endif // SCHEDFIFO

    //retornar ponteiro para a tarefa encontrada no loop 1*/
    return (task_t*) task_run;
}

void dispatcher_body (){ // dispatcher é uma tarefa
    task_t* next;
    task_t * aux;
    int total = 0;
    int i = 0;

    while ( user_task  > 1 ){
        aux = (task_t *) sleeping_queue;
        total = queue_size(sleeping_queue);
        if( total > 0){
            for(i = 0; i < total; i++){
                  if(systime() >= aux->t_awakening){
                      sleeping_queue =  sleeping_queue->next;//proximo se torna o cabeça da fila
                      queue_remove((queue_t **) &sleeping_queue, (queue_t *) aux); //remove o cabeca da fila de dormindo
                      queue_append((queue_t **) &t_queue, (queue_t *) aux); //coloca o cabeca na fila de prontos
                      aux = (task_t *) sleeping_queue;
                  }else{
                      aux = aux->next;
                  }
            }
         }
        next = scheduler() ;  // scheduler é uma função
        if (next){
            // ações antes de lançar a tarefa "next", se houverem
            quantum = 20;
            task_switch (next) ; // transfere controle para a tarefa "next"
            // ações após retornar da tarefa "next", se houverem
        }
    }
    task_exit(0) ; // encerra a tarefa dispatcher
}
