#include "datatypes.h"

int sem_create (semaphore_t *s, int value){
    if(s == NULL)// semaphore NULL
       return -1;

    s->s_count = value; //contador
    s->s_queue = NULL;
    s->s_lock = 0;

    return 0;
}


int sem_down (semaphore_t *s){
   if(s == NULL || s->s_lock == 1)
        return -1;
   preempt = 0;
   s->s_count--; //s.counter ← s.counter − 1
   if(s->s_count < 0){//if s.counter < 0 then
        //põe a tarefa corrente no final de s.queue
        queue_remove((queue_t**) &t_queue, (queue_t*) atual);
        queue_append((queue_t**) &s->s_queue, (queue_t*) atual);
        //suspende a tarefa corrente
        task_switch(&dispatcher);
        if(s->s_lock == 1){
            preempt = 1;
            return -1;
        }
    }
    preempt = 1;
    return 0;
}

int sem_up (semaphore_t *s){
    if(s == NULL || s->s_lock == 1)
        return -1;
    preempt = 0;
    s->s_count++;//s.counter ← s.counter + 1
    if(s->s_count <= 0){//if s.counter ≤ 0 then
      task_t* aux = (task_t*) s->s_queue;
      queue_remove((queue_t**) &s->s_queue, (queue_t*) aux);//retira a primeira tarefa t de s.queue
      queue_append((queue_t**) &t_queue, (queue_t*) aux);//devolve t à fila de tarefas prontas (ou seja, acorda t)
    }
    preempt = 1;
    return 0;
}

int sem_destroy (semaphore_t *s){
    if(s == NULL)
        return -1;
    preempt = 0;
    if(s->s_queue != NULL){
        int total = queue_size(s->s_queue);
        int i;
        for(i=0; i < total; i++){
            task_t* aux = (task_t*) s->s_queue;
            queue_remove((queue_t**) &s->s_queue, (queue_t*) aux);
            queue_append((queue_t**) &t_queue, (queue_t*) aux);
        }
        s->s_queue = NULL;
    }
    s->s_lock = 1;
    preempt = 1;
    return 0;
}
