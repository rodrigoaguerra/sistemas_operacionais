#include "datatypes.h"
//#define DEBUG

void ticks (int signum){
        p_clock++;
        atual->task_processor++;
        if(atual->task_nivel != 0 && preempt == 1){
            quantum--;
            if(quantum <= 0){
                task_switch(&dispatcher);
            }
        }
}

//Inicializa o sistema
void pingpong_init (){
     /* desativa o buffer da saida padrao (stdout), usado pela função printf */
     setvbuf (stdout, 0, _IONBF, 0);

     cont_task = 0;
     user_task = 0;
     quantum = 20;
     preempt = 1;
     atual = NULL;
     t_queue = NULL;
     sleeping_queue = NULL;

     //criando a tarefa dispatcher
     task_create(&maintask, NULL, "Tarefa Main");

     #ifdef DEBUG
        printf ("pingpong_init: criou tarefa %d\n", maintask.tid) ;
     #endif

     atual = &maintask;

     //criando a tarefa dispatcher
     task_create(&dispatcher, dispatcher_body, "Despachante de tarefas");

     //remove dispatcher da fila de prontas
     queue_remove((queue_t **) &t_queue, (queue_t *) &dispatcher);


     dispatcher.task_nivel = 0;

     #ifdef DEBUG
        printf ("pingpong_init: criou o dispatcher %d\n", dispatcher.tid) ;
     #endif
    // temporizador
     struct itimerval timer;
     p_clock = 0;
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

     //tratador disck_ meneger
     if (signal (SIGUSR1, disk_handler) == SIG_ERR){
         perror ("Erro em signal: ") ;
         exit (1) ;
     }

     #ifdef DEBUG
        printf ("pingpong_init: criou o temporizador. \n") ;
     #endif
}

//tempo de execução das  tarefas
unsigned int systime(){
    return p_clock;
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
    task->tid = cont_task;
    task->prio_static = 0;
    task->prio_dinamic = 0;
    task->task_nivel = 1;
    task->task_start = systime();
    task->task_processor = 0;
    task->task_activations = 0;
    cont_task++;
    user_task++;
    task->status = 0;
    task->t_join = NULL;
    task->t_awakening = 0;

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
    atual->task_activations++;// ativacoes da tarefa atual
    swapcontext (&old->context, &task->context);

    return 0;
}

//termina a tarefa corrente
void task_exit (int exit_code){
       #ifdef DEBUG
            queue_print("t_queue no task_exit.:", t_queue, print_elem);
            printf ("task_exit: foi removida a tarefa %d com a prioridade %d \n", atual->tid, atual->prio_static) ;
        #endif
        printf("Task %d exit: execution time %4d ms, processor time %4d ms, %4d  activations \n", atual->tid, (systime() - atual->task_start), atual->task_processor, atual->task_activations);
        //removendo tarefa atual da fila;
        queue_remove ((queue_t**) &t_queue, (queue_t*) atual);
        user_task--;
        if(atual->t_join != NULL){
           task_t* aux_join = (task_t*) atual->t_join;
           int total = queue_size(atual->t_join);
           int i;
           for(i = 0; i < total; i++){
                queue_t* wait_join =  queue_remove((queue_t**)&atual->t_join, (queue_t*) aux_join);
                queue_append((queue_t**) &t_queue, (queue_t*) wait_join);
                aux_join = aux_join->next;
           }
        }
        atual->status = -1;
        atual->num_exit = exit_code;
        //(atual->tid == 0)
            //user_task--;
            //exit (1) ;
        task_yield ();
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

int task_join (task_t *task){
    if(task == NULL){
         #ifdef DEBUG
            printf ("task_join: a tarefa não existe.") ;
         #endif
        return -1;
    }

    if(task->status < 0){
         #ifdef DEBUG
            printf ("task_join: a tarefa já foi terminada.") ;
         #endif
         return task->num_exit;
    }

    task_t* wait = (task_t*) queue_remove((queue_t**)&t_queue, (queue_t*) atual);
    queue_append((queue_t**) &task->t_join, (queue_t*)wait);
    #ifdef DEBUG
         printf ("task_join: a tarefa.: %d espera a tarefa.: \n", wait->tid, task->tid ) ;
    #endif
    task_yield ();
    return task->num_exit;

}

void task_sleep (int t){
     if( t > 0){
         task_t* sleep = (task_t*) queue_remove((queue_t**)&t_queue, (queue_t*) atual);// retirando da fila de prontos
         sleep->t_awakening =  systime() + t*1000;// setando o tempo
         #ifdef DEBUG
            printf ("task_sleep: a tarefa.: %d vai dormir. \n", atual->tid ) ;
         #endif
         queue_append((queue_t**) &sleeping_queue, (queue_t*) sleep);// colocando o na fila de adormecidos
         task_yield();// voltando ao dispatcher
     }
}
