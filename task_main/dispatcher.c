#include "datatypes.h"

void task_yield (){
    task_switch (&dispatcher);
}

task_t* scheduler(){
    task_t *aux = (task_t *)t_queue->next;
    task_t *task_run = (task_t *)t_queue->next;
    while(aux != (task_t *)t_queue){
        if(aux->prio_dinamic <= task_run->prio_dinamic){
            task_run = aux;
            aux->prio_dinamic--;
        }else{
            aux->prio_dinamic--;
        }
        aux = aux->next;
    }
    task_run->prio_dinamic = task_run->prio_static;

    return (task_t*) task_run;
}

void dispatcher_body () // dispatcher é uma tarefa
{
    task_t* next;
    while ( user_task  > 1 )
    {
        next = scheduler() ;  // scheduler é uma função
        if (next)
        {
            // ações antes de lançar a tarefa "next", se houverem
            task_switch (next) ; // transfere controle para a tarefa "next"
            // ações após retornar da tarefa "next", se houverem
        }
    }
    task_exit(1) ; // encerra a tarefa dispatcher
}
