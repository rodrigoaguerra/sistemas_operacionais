#include "datatypes.h"
//#define DEBUG

task_t *atual;
task_t maintask;
int cont_task;
int quantum;
unsigned int clock;

#ifdef DEBUG
void print_elem (void *ptr){
   task_t *elem = ptr ;

   if (!elem)
      return ;

   elem->prev ? printf ("%d", elem->prev->tid) : printf ("*") ;
   printf ("<%d>", elem->tid) ;
   elem->next ? printf ("%d", elem->next->tid) : printf ("*") ;
}
#endif

//tempo de execução das  tarefas
unsigned int systime(){
    return clock;
}

void ticks (int signum){
     if(atual->task_nivel != 0){
        quantum--;
        if(quantum == 0){
             quantum = 20;
             atual->task_activations++;
             #ifdef DEBUG
                printf("Task %d exit: execution time %4d ms, processor time %4d ms, %4d  activations \n", atual->tid, systime(), atual->task_processor, atual->task_activations);
             #endif
            //remove tarefa do processador
            queue_remove ((queue_t**) &t_queue, (queue_t*) atual);
            //retorna a tarefa para a fila de prontas
            queue_append((queue_t**) &t_queue, (queue_t*) atual);
            dispatcher.task_activations++;
            task_switch (&dispatcher);
        }
        clock++;
        atual->task_processor++;
     }
     atual->task_processor++;
     clock++;
}

//Inicializa o sistema
void pingpong_init (){
     /* desativa o buffer da saida padrao (stdout), usado pela função printf */
     setvbuf (stdout, 0, _IONBF, 0);

     // temporizador
     struct itimerval timer;
     // define a ação para o sinal de timer
     if (signal (SIGALRM, ticks) == SIG_ERR){
         perror ("Erro em signal: ") ;
         exit (1) ;
     }

     // ajusta valores do temporizador
     timer.it_value.tv_usec = 1000;      // primeiro disparo, em micro-segundos
     timer.it_value.tv_sec  = 0 ;      // primeiro disparo, em segundos
     timer.it_interval.tv_usec = 1000;   // disparos subsequentes, em micro-segundos
     timer.it_interval.tv_sec  = 0 ;   // disparos subsequentes, em segundos

     // arma o temporizador ITIMER_REAL (vide man setitimer)
     if (setitimer (ITIMER_REAL, &timer, 0) < 0){
          perror ("Erro em setitimer: ") ;
          exit (1) ;
     }
     #ifdef DEBUG
        printf ("pingpong_init: criou o temporizador. \n") ;
     #endif
     cont_task = 0;
     user_task = 0;
     quantum = 20;
     clock = 0;
     getcontext (&dispatcher.context);
     maintask.next =  NULL;
     maintask.prev =  NULL;
     dispatcher.next = NULL;
     dispatcher.prev = NULL;
     #ifdef DEBUG
        printf ("pingpong_init: criou tarefa %d\n", maintask.tid) ;
     #endif

     //criando a tarefa dispatcher
     task_create(&dispatcher, dispatcher_body, "Despachante de tarefas");

     //criando a tarefa dispatcher
     task_create(&maintask, dispatcher_body, "Despachante de tarefas");

     atual = &maintask;

     #ifdef DEBUG
        printf ("pingpong_init: criou a fila e o dispatcher %d\n", dispatcher.tid) ;
     #endif


}

//cria uma nova tarefa
int task_create (task_t *task, void (*start_routine)(void *),  void *arg){

    char *stack ;

    getcontext (&task->context);

    stack = malloc (STACKSIZE) ;
    if (stack)
    {
        task->context.uc_stack.ss_sp = stack ;
        task->context.uc_stack.ss_size = STACKSIZE;
        task->context.uc_stack.ss_flags = 0;
        task->context.uc_link = 0;
    }
    else
    {
        return -1;
    }

    makecontext (&task->context, (void*)(*start_routine), 1, arg);
    task->next = NULL;
    task->prev = NULL;
    if(task == &maintask){
       task->tid = 0;
    }else{
       task->tid = cont_task;
    }
    task->prio_static = 0;
    task->prio_dinamic = 0;
    if(task == &dispatcher){
      task->task_nivel = 0;
      task->tid = 1;
    }else{
      task->task_nivel = 1;
    }


    task->task_start = systime();
    task->task_processor = 0;
    task->task_activations = 0;
    task->quantum = 20;
    cont_task++;
    user_task++;

    #ifdef DEBUG
        printf ("task_create: criou tarefa %d\n", task->tid) ;
    #endif

    queue_append((queue_t**) &t_queue, (queue_t*)task);

    #ifdef DEBUG
        printf ("task_create: colocou a  tarefa  %d ,  na fila\n", task->tid) ;
    #endif

    return task->tid;
}


//transfere o processador para outra tarefa
int task_switch (task_t *task){

    task_t *old = atual;
    atual = task;

    #ifdef DEBUG
        printf ("task_switch: trocou a tarefa %d pela a tarefa %d\n", old->tid, task->tid) ;
    #endif

    swapcontext (&old->context, &task->context);

    return 0;
}

//termina a tarefa corrente
void task_exit (int exit_code){
     if(atual != &dispatcher){

        #ifdef DEBUG
            queue_print("t_queue no task_exit.:", t_queue, print_elem);
            printf ("task_exit: foi removida a tarefa %d com a prioridade %d \n", atual->tid, atual->prio_static) ;
        #endif

        printf("Task %d exit: execution time %4d ms, processor time %4d ms, %4d  activations \n", atual->tid, (systime() - atual->task_start), atual->task_processor, atual->task_activations);

        //removendo tarefa atual da fila;
        queue_remove ((queue_t**) &t_queue, (queue_t*) atual);

        user_task--;

        task_switch (&dispatcher);
     }else{
        #ifdef DEBUG
            queue_print("t_queue no task_exit.:", t_queue, print_elem);
            printf ("task_exit: a tarefa %d e o dispacher com a prioridade %d \n", atual->tid, atual->prio_static) ;
        #endif

        printf("Task %d exit: execution time %4d ms, processor time %4d ms, %4d  activations \n", atual->tid, (systime() - atual->task_start), atual->task_processor, atual->task_activations);

        task_switch (&maintask);
     }
}

//informa o identificador da tarefa corrente
int task_id (){
    return atual->tid;
}

//seta prioridade na tarefa;
void task_setprio (task_t *task, int prio){
     if(prio < 20 && prio > -20){
        if(task != NULL){
            task->prio_static = prio;
            task->prio_dinamic = prio;
        }else{
            atual->prio_static = prio;
            atual->prio_dinamic = prio;
        }
     }else{
        #ifdef DEBUG
            printf ("task_setprio: A tarefa %d está com a prioridade maior que 20 ou menor que -20 valor prio %d \n", task->tid, prio) ;
        #endif
     }
}

//pega prioridade da tarefa;
int task_getprio (task_t *task){
    if(task == NULL)
       return atual->prio_static;
    else
       return task->prio_static;
}


