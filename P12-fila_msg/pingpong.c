
#include"datatypes.h"


//  #define DEBUG // funcionalidade que habilita debug via printf das funcoes             ****   DEBUG   ****


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
// Insere um elemento no final da fila.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - o elemento deve existir
// - o elemento nao deve estar em outra fila
void queue_append (queue_t **queue, queue_t *elem) {
    preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
    if(queue == NULL){ // fila sem memoria alocada
        printf("queue_append: A fila nao existe! \n");
        //exit (1);
        //return NULL;
    }else if(elem == NULL){ // elemento sem memoria alocada
        printf("queue_append: O elemento nao existe! \n");
        //exit (1);
        //return NULL;
    }else if((elem->next != NULL) || (elem->prev != NULL)){ // verifica se "elem" pertence a alguma fila
        printf("queue_append: O elemento ja pertence a uma fila! \n");
        //exit (1);
        //return NULL;
    }else if(*queue == NULL){ // verifica se o conteudo de "queue" eh vazio - fila valida mas sem elementos
        *queue = elem; // atribui "elem" ao conteudo de "queue" - "queue" aponta para "elem" e nao mais para NULL
        elem->next = elem; // prepara elemento para ser retornado apontando para NULL
        elem->prev = elem; // prepara elemento para ser retornado apontando para NULL
        #ifdef DEBUG
        task_t* enfileirada = (task_t*) elem;
        printf ("queue_append: adicionou tarefa %d na fila como primeira tarefa de usuario\n", enfileirada->id) ;
        #endif
    }else{ // condicao de "queue" valida possuido elementos, considerando sempre "queue" como primeiro elemento
        elem->next = (*queue);
        elem->prev = (*queue)->prev;
        (*queue)->prev->next = elem;
        (*queue)->prev = elem;
        #ifdef DEBUG
        task_t* enfileirada = (task_t*) elem;
        printf ("queue_append: adicionou tarefa %d na fila, sendo que ja haviam tarefas de usuario na fila\n", enfileirada->id) ;
        #endif
    }
    preempcao = 1; // habilita trocas de contexto apos finalizar funcao
}

//------------------------------------------------------------------------------
// Remove o elemento indicado da fila, sem o destruir.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - a fila nao deve estar vazia
// - o elemento deve existir
// - o elemento deve pertencer a fila indicada
// Retorno: apontador para o elemento removido, ou NULL se erro
queue_t *queue_remove (queue_t **queue, queue_t *elem) {
    preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
    if(queue == NULL){ // fila sem memoria alocada
        printf("queue_remove: A fila nao existe! \n");
        //exit (1);
        preempcao = 1; // habilita trocas de contexto apos finalizar funcao
        return NULL;
    }else if(*queue == NULL){ // fila sem elementos - aponta para NULL
        printf("queue_remove: A fila esta vazia! \n");
        //exit (1);
        preempcao = 1; // habilita trocas de contexto apos finalizar funcao
        return NULL;
    }else if(elem == NULL){ // elemento sem memoria alocada
        printf("queue_remove: O elemento nao existe! \n");
        //exit (1);
        preempcao = 1; // habilita trocas de contexto apos finalizar funcao
        return NULL;
    }else if((*queue == elem) && ((*queue)->next == *queue)){ // fila com apenas 1 elemento
        elem->next = NULL;
        elem->prev = NULL;
        *queue = NULL;
        #ifdef DEBUG
        task_t* desenfileirada = (task_t*) elem;
        printf ("queue_remove: removeu tarefa %d da fila, sendo que era a ultima tarefa de usuario da fila\n", desenfileirada->id) ;
        #endif
        preempcao = 1; // habilita trocas de contexto apos finalizar funcao
        return elem;
    }else if(*queue == elem){ // fila com mais de 1 elemento, sendo "elem" = "queue" , ou seja, o primeiro elemento
        *queue = elem->next;
        elem->prev->next = elem->next;
        elem->next->prev = elem->prev;
        elem->next = NULL;
        elem->prev = NULL;
        #ifdef DEBUG
        task_t* desenfileirada = (task_t*) elem;
        printf ("queue_remove: removeu tarefa %d da fila, sendo que era a primeira tarefa de usuario na fila\n", desenfileirada->id) ;
        #endif
        preempcao = 1; // habilita trocas de contexto apos finalizar funcao
        return elem;
    }else{ // percorre a fila ate encontrar elemento ou acabar a fila
        queue_t *auxiliar = *queue;
        auxiliar = auxiliar->next;
        while((auxiliar != *queue) && (auxiliar != elem)){ // percorre a fila
            auxiliar = auxiliar->next;
        }
        if(auxiliar == elem){ // caso encontre o elemento na fila faz a remocao
            elem->prev->next = elem->next;
            elem->next->prev = elem->prev;
            elem->next = NULL;
            elem->prev = NULL;
            #ifdef DEBUG
            task_t* desenfileirada = (task_t*) elem;
            printf ("queue_remove: removeu tarefa %d da fila,  sendo que existem mais tarefas de usuario na fila\n", desenfileirada->id) ;
            #endif
            preempcao = 1; // habilita trocas de contexto apos finalizar funcao
            return elem;
        }
        else{
            printf("queue_remove: O elemento nao pertence a esta fila! \n");
            //exit (1);
            preempcao = 1; // habilita trocas de contexto apos finalizar funcao
            return NULL;
        }
    }
    preempcao = 1; // habilita trocas de contexto apos finalizar funcao
}

//------------------------------------------------------------------------------
// Conta o numero de elementos na fila
// Retorno: numero de elementos na fila
int queue_size (queue_t *queue){
    preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
    if(queue == NULL || queue->next == NULL){ // fila valida mas sem elementos
        preempcao = 1; // habilita trocas de contexto apos finalizar funcao
        return 0;
    }else if(queue->next == queue){ // fila com apenas 1 elemento
        preempcao = 1; // habilita trocas de contexto apos finalizar funcao
        return 1;
    }else{ // pecorre a fila ate acabar realizando a contagem
        queue_t *auxiliar = queue;
        int i = 1;
        while(auxiliar->next != queue){
            auxiliar = auxiliar->next;
            i++;
        }
        preempcao = 1; // habilita trocas de contexto apos finalizar funcao
        return i;
    }
}

//------------------------------------------------------------------------------
// Percorre a fila e imprime na tela seu conteúdo. A impressão de cada
// elemento é feita por uma função externa, definida pelo programa que
// usa a biblioteca. Essa função deve ter o seguinte protótipo:
//
// void print_elem (void *ptr) ; // ptr aponta para o elemento a imprimir
void queue_print (char *name, queue_t *queue, void print_elem (void*) ) {
    if(queue == NULL){ // fila vazia
        printf("%s: [] \n", name);
        //printf("A fila %s esta vazia!", name);
        //exit (1);
    }else if((queue->next == queue) && (queue->prev == queue)){ // fila com apenas 1 elemento
        printf("%s: [", name);
        print_elem(queue);
        printf("] \n");
    }else{ // percorre a fila ate acabar realizando as impressoes
        printf("%s: [", name);
        queue_t *auxiliar = queue;
        while(auxiliar->next != queue){
            print_elem(auxiliar);
            auxiliar = auxiliar->next;
            printf(" ");
        }
        print_elem(auxiliar);
        printf("] \n");
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inicializa o sistema operacional; deve ser chamada no inicio do main()
void pingpong_init (){
    setvbuf (stdout, 0, _IONBF, 0);/* desativa o buffer da saida padrao (stdout), usado pela função printf */
    relogio = 0;  // zera variavel utilizada como relogio global
    struct itimerval timer; // estrutura de inicialização to timer
    if (signal (SIGALRM, conta_ticks) == SIG_ERR)
    {
        perror ("Erro em siganal: ") ;
        exit (1) ;
    }
    // ajusta valores do temporizador
    timer.it_value.tv_usec = 1000 ;      // primeiro disparo, em micro-segundos
    timer.it_value.tv_sec  = 0 ;      // primeiro disparo, em segundos
    timer.it_interval.tv_usec = 1000 ;   // disparos subsequentes, em micro-segundos
    timer.it_interval.tv_sec  = 0 ;   // disparos subsequentes, em segundos
    if (setitimer (ITIMER_REAL, &timer, 0) < 0) // arma o temporizador ITIMER_REAL (vide man setitimer)
    {
        perror ("Erro em setitimer: ") ;
        exit (1) ;
    }
    contador_tarefas = 0; // utilizado para definir o id das tarefas
    user_task = -1; //  utilizado para definir a quantidade de tarefas criadas, descontando a tarefa dispatcher
    queue_task = NULL; // inicializa ponteiro para fila de tarefas prontas para executar com endereco nulo
    task_create (&task_main, (void*) main, "Main"); // cria efetivamente a tarefa Main
    task_atual = &task_main; // tarefa atual aponta para Main neste momento
    task_create (&dispatcher, dispatcher_body, " *** DISPATCHER *** "); // cria efetivamente a tarefa dispatcher
    envelhecimento = -1; // define a taxa de envelhecimento das tarefas - fator de ajuste da prioridade
	quantum = 20; // definicao do quantum padrao
	preempcao = 1; // ativa preempcao entre tarefas;
	#ifdef DEBUG
    printf ("pingpong_init: inicializou variaveis \n");
    #endif
    queue_task = (queue_t*) &dispatcher; // devido a insercao da tarefa main na fila de prontas eh necessario fazer queue_task apontar para dispatcher
    task_yield(); // transfere para dispatcher a partir de pingpong init
}


// gerência de tarefas =========================================================

// Cria uma nova tarefa. Retorna um ID> 0 ou erro.
int task_create (task_t *task,			        // descritor da nova tarefa
                 void (*start_func)(void *),	// funcao corpo da tarefa
                 void *arg) {                   // argumentos para a tarefa

    preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
    getcontext(&(task->context)); // salva contexto atual na task criada
    char *stack ;
    stack = malloc (STACKSIZE); // criacao da pilha para inicializar variaveis para utilizacao de context
    if (stack)
    {
        task->context.uc_stack.ss_sp = stack ;
        task->context.uc_stack.ss_size = STACKSIZE;
        task->context.uc_stack.ss_flags = 0;
        task->context.uc_link = 0;
    }
    else
    {
      perror ("Erro na criação da pilha: ");
      return (-1);
    }
    makecontext (&(task->context), (void*)(*start_func), 1,(char*)arg);
    task->id = contador_tarefas; // adiciona id da tarefa
    contador_tarefas++; // incrementa contador de tarefas
    if(task->id == 1){ // verifica se a tarefa corresponde ao dispatcher: id = 1 sempre
        task->tarefa_sistema = 1; // define status da tarefa como tarefa de sistema
    }else{
        task->tarefa_sistema = 0; // define status da tarefa como de usuario
    }
    task_setprio(task, 0); // prioridade inicial de todas as tarefas vale 0
    task->quantum = quantum; // quantum inicial de todas as tarefas = 20
    queue_append((queue_t**) &queue_task, ((queue_t*) task)); // adiciona tarefa criada na fila de tarefas de usuario prontas para executar
    user_task++; // incrementa o contador de tarefas criadas e prontas para executar
    task->inicio = systime(); // inicializa tempo em ms correspondente a criacao da tarefa
    task->fim = 0; // inicializa tempo em ms correspondente ao termino da tarefa
    task->ativacoes = 0; // inicializa o numero de ativacoes da tarefa
    task->processamento = 0; // inicializa o tempo de processamento efetivo da tarefa
    task->codigo_encerramento = 0; // inicializa codigo de encerramento da tarefa com valor = 0
    task->status = 1; // status datarefa = ativa
    task->queue_espera = NULL; // inicializa ponteiro da fila de espera como null
    task->despertador = 0; // inicializa despertador da tarefa com valor 0 - tarefa nao dorme
    task->erro = 0; // tarefa criada sem erro
    #ifdef DEBUG
    printf ("\n task_create: criou tarefa %d: %s como uma tarefa: %d \n", task->id, (char*) arg, task->tarefa_sistema) ;
    printf(" ativacoes = %d \n codigo_encerramento = %d \n despertador = %d \n fim = %d \n id = %d \n inicio = %d \n prio_dinamica = %d \n prio_estatica = %d \n processamento = %d \n quantum = %d \n queue_espera = %d \n stack = %d \n status = %d \n tarefa_sistema = %d \n \n",
                task->ativacoes, task->codigo_encerramento, task->despertador, task->fim, task->id, task->inicio, task->prio_dinamica, task->prio_estatica, task->processamento, task->quantum, task->queue_espera, task->stack, task->status, task->tarefa_sistema);
    #endif
    preempcao = 1; // habilita trocas de contexto apos finalizar funcao
    return task->id;
}

// Termina a tarefa corrente, indicando um valor de status de encerramento
void task_exit (int exitCode) {
    preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
    #ifdef DEBUG
    printf ("task_exit: task_atual->id = %d sendo encerrada com exitCode = %d \n", task_atual->id, exitCode) ;
    #endif
    task_atual->codigo_encerramento = exitCode;  // redefine o codigo de encerramento da tarefa atual
    task_atual->status = 0; // tarefa encerrada
    task_t* task_removida = (task_t*) task_atual; // variavel criada para ser usada como parametro elem na funcao queue_remove
    queue_remove((queue_t**) &queue_task, (queue_t*)task_removida); // remove a tarefa atual da fila de tarefas prontas para executar
    user_task--; // decrementa o contador de tarefas prontas para executar
    task_atual->fim = systime();  // determina o tempo em ms do termino da tarefa
    printf("Task %d exit: execution time %d ms, processor time %d ms, %d activations \n",
            task_atual->id, (task_atual->fim-task_atual->inicio), task_atual->processamento, task_atual->ativacoes);
    task_t* task_auxiliar;
    int tarefas_esperando = queue_size(task_removida->queue_espera);
    while(tarefas_esperando > 0){
        #ifdef DEBUG
        printf("task_exit: %d tarefas esperando encerramento de task->id = %d \n", tarefas_esperando, task_atual->id);
        #endif // DEBUG
        task_auxiliar = (task_t*) task_removida->queue_espera;
        #ifdef DEBUG
        printf("task_exit: task->id = %d sera removida da fila de espera \n", task_auxiliar->id);
        #endif // DEBUG
        queue_remove((queue_t**) &(task_removida->queue_espera), (queue_t*) task_auxiliar);
        tarefas_esperando = queue_size(task_removida->queue_espera);
        #ifdef DEBUG
        printf("task_exit: task->id = %d removida da fila de espera \n", task_auxiliar->id);
        printf("task_exit: %d tarefas esperando encerramento de task->id = %d \n", tarefas_esperando, task_atual->id);
        #endif // DEBUG
        queue_append((queue_t**) &queue_task, (queue_t*) task_auxiliar);
        #ifdef DEBUG
        printf("task_exit: task->id = %d inserida na fila de prontas \n", task_auxiliar->id);
        #endif // DEBUG
    }
    preempcao = 1; // habilita trocas de contexto apos finalizar funcao
    task_yield(); // passa controle para o dispatcher
}

// alterna a execução para a tarefa indicada
int task_switch (task_t *task){
	preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
	if(task_atual == task){ // verifica condicao de alternancia entre apenas uma tarefa e a tarefa main
        #ifdef DEBUG
        printf ("task_switch: trocando contexto entre a mesma tarefa: %d -> %d: volta para Main \n", task_atual->id, task->id);
        #endif
        preempcao = 1;
        setcontext(&task_main.context); // retorna ao contexto da tarefa main
    }
    //task->ativacoes++; // considerando as ativacoes neste ponto contabiliza uma ativacao a mais para o dispatcher
    task_t *task_auxiliar = task_atual; // conceito de janela deslizante para alternar entre mais de 2 tarefas e a main
    task_atual = task;
    #ifdef DEBUG
    printf ("task_switch: trocando contexto tarefas diferentes: %d -> %d \n", task_auxiliar->id, task->id);
    #endif
    preempcao = 1; // habilita trocas de contexto apos finalizar funcao
    swapcontext (&task_auxiliar->context, &task->context);
    return 0;
}

// retorna o identificador da tarefa corrente (main eh 0)
int task_id () {
    #ifdef DEBUG
    printf ("task_id: id da tarefa =  %d\n", task_atual->id) ;
    #endif
    return task_atual->id;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
task_t* scheduler(){
/* CONSIDERACOES PARA COMPREENSAO DA FUNCAO */
/*
                    -----------------------------------------------
                   ||     *QUEUE                                   |
                   ||        ||                                    |
                   \/        \/                                    |
                   _         __              __             __     |
                  | |------>|  |----------->|  |---------->|  |----
                  | |       |  |            |  |           |  |
                  |_|<------|__|<-----------|__|<----------|__|<---
                  MAIN   DISPATCHER                                |
                 TASK 0    TASK 1          TASK 2          TASK 3  |
                    |                                              |
                    |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ |

*/
    preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
    task_t* task_prioridade = (task_t*) queue_task->next; // task prioritaria = task 2 (primeira tarefa) - se houver
    queue_t* auxiliar = queue_task->next; // variavel auxiliar para percorrer a fila buscando a tarefa de prioridade maior
    task_t* task_auxiliar; // variavel criada para realizar "cast" do elemento da fila e verificar sua prioridade
    while( (auxiliar->next != queue_task) // verifica se proximo elemento = inicio da fila - fila com apenas 1 tarefa
            && (auxiliar->next != (queue_t*) &dispatcher) ){ // verifica se proximo elemento = dispatcher: final da fila
        task_auxiliar = (task_t*) auxiliar->next; // task_auxiliar = primeira tarefa(se houver) desconsiderando dispatcher
        if(task_auxiliar->prio_dinamica < task_prioridade->prio_dinamica){  // verifica se task_auxiliar tem mais prioridade que task_prioridade
            task_prioridade->prio_dinamica += envelhecimento; // envelhece tarefa de menor prioridade, caso esteja ativa
            task_prioridade = task_auxiliar; // atualiza qual a tarefa de maior prioridade
        }else{
            task_auxiliar->prio_dinamica += envelhecimento; // envelhece tarefa de menor prioridade, caso esteja ativa
        }
        auxiliar = auxiliar->next; // vai para proximo elemento a verificar prioridade - se houver
    }
    #ifdef DEBUG
    //printf ("scheduler: id da tarefa prioritaria =  %d, com prioridade dinamica = %d \n", task_prioridade->id, task_prioridade->prio_dinamica) ;
    #endif
    task_prioridade->prio_dinamica = task_prioridade->prio_estatica; // valor da prio_dinamica = prio_estatica da tarefa a executar
    preempcao = 1; // habilita trocas de contexto apos finalizar funcao
    return task_prioridade; // retorna tarefa de prioridade mais elevada (lembrando que no LINUX eh a de valor mais baixo)
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void dispatcher_body (void *arg) // dispatcher é uma tarefa
{
    task_t* next; // proxima tarefa a executar
    while ( user_task > 0 ) // verifica se existem tarefas na fila de tarefas prontas para executar
    {
        int adormecidas = queue_size(queue_sleep);
        #ifdef DEBUG
        //printf("dispatcher: tarefas adormecidas = %d, tarefas prontas = %d \n", queue_size(queue_sleep), queue_size(queue_task));
        #endif // DEBUG
        task_t* task_auxiliar = (task_t*) queue_sleep;
        if(adormecidas > 0){
            int i;
            for(i = 0; i < adormecidas; i++){
                int tempo_atual = systime();
                #ifdef DEBUG
                //printf("dispatcher: task_auxiliar->id = %d e despertador = %d ms, tempo_atual = %d ms \n", task_auxiliar->id, task_auxiliar->despertador, tempo_atual);
                #endif // DEBUG
                if(tempo_atual > task_auxiliar->despertador){
                    queue_remove((queue_t**) &queue_sleep, (queue_t*) task_auxiliar);
                    #ifdef DEBUG
                    //printf("dispatcher: removeu task->id = %d da fila de adormecidas \n", task_auxiliar->id);
                    #endif // DEBUG
                    queue_append((queue_t**) &queue_task, (queue_t*) task_auxiliar);
                    #ifdef DEBUG
                    //printf("dispatcher: inseriu task->id = %d na fila de prontas \n", task_auxiliar->id);
                    //printf("dispatcher: tarefas adormecidas = %d, tarefas prontas = %d \n", queue_size(queue_sleep), queue_size(queue_task));
                    #endif // DEBUG
                    task_auxiliar = (task_t*) queue_sleep;
                }else{
                    queue_sleep = queue_sleep->next;
                    task_auxiliar = (task_t*) queue_sleep;
                }
            }
        }
        next = scheduler() ;  // scheduler é uma função que retorna a tarefa mais prioritaria para executar
        if (next != &dispatcher)
        {
            dispatcher.ativacoes++; // incrementa o contador de ativacoes da tarefa dispatcher
            next->ativacoes++; // incrementa o contador de ativacoes da tarefa atual
            #ifdef DEBUG
            printf("dispatcher: next->id = %d, next->ativacoes = %d, dispatcher->ativacoes = %d \n", next->id, next->ativacoes, dispatcher.ativacoes);
            #endif // DEBUG
            task_switch (next) ; // transfere controle para a tarefa "next"

        }
    }
    #ifdef DEBUG
    printf("dispatcher: user_task = %d \n", user_task);
    #endif // DEBUG
    task_exit(0) ; // encerra a tarefa dispatcher
}

// transfere processador para tarefa dispatcher
void task_yield (){
    #ifdef DEBUG
    printf("task_yeld: trocando contexto da task->id = %d para dispatcher \n", task_atual->id);
    #endif // DEBUG
    task_switch(&dispatcher);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void task_setprio (task_t *task, int prio){ // define priodirade estatica e dinamica da tarefa
    if(prio > 19 || prio < -20){ // valores utilizados pelo sistema LINUX
        printf("Prioridade invalida para o sistema LINUX - vai de (-20)=mais prioritaria, ate (+19)=menos prioritaria");
        exit(1);
    }
    if(task == NULL){ // se task = NULL seta valor da prioridade para tarefa atual
        task_atual->prio_estatica = prio;
        task_atual->prio_dinamica = task_atual->prio_estatica;
    }else{
        task->prio_estatica = prio;
        task->prio_dinamica = task->prio_estatica;
    }
}

int task_getprio (task_t *task){ // retorna a prioridade estatica da tarefa
    if(task == NULL){
        return task_atual->prio_estatica;
    }else{
        return task->prio_estatica;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void conta_ticks(int sinal){ // funcao ativada a cada ms
    relogio++; // incrementa o relogio  global, em ms
    task_atual->processamento++; // incrementa tempo de processamento da tarefa atual, incremento em ms
    if((task_atual->tarefa_sistema == 0) && (preempcao == 1)){
        task_atual->quantum--; // decrementa quantum da tarefa atual
        if(task_atual->quantum <= 0){
            task_atual->quantum = quantum; // fornece novo quantum para a tarefa atual
            task_yield(); // troca contexto para dispatcher
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int systime (){ // funcao que retorna o tempo atual do relogio global, em ms
    return relogio;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int task_join (task_t *task){ // funcao que faz a tarefa atual - que chamou a funcao task_join - ficar aguardando o temino da tarefa task
    preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
    if(task_atual == task){
        printf("task_join: Tarefa nao pode aguardar conclusao de si mesma \n");
        exit(1);
        return -1;
    }else if(task == NULL){
        printf("task_join: Tarefa nao existe! \n");
        preempcao = 1; // habilita trocas de contexto apos finalizar funcao
        return -1;
    }else if(task->status == 0){
        //printf("task_join: Tarefa ja encerrada! \n");
        preempcao = 1; // habilita trocas de contexto apos finalizar funcao
        return task->codigo_encerramento;
    }else {
        task_atual->status = 2; // tarefa esperando conclusao de task
        task_t* task_esperando = (task_t*) queue_remove((queue_t**) &queue_task, (queue_t*) task_atual); // remove task_atual da fila de prontas
        #ifdef DEBUG
        printf("task_join: task->id = %d removeu task->id = %d da fila de prontas \n", task->id, task_esperando->id);
        #endif // DEBUG
        queue_append((queue_t**) &(task->queue_espera), (queue_t*) task_esperando); // insere task_atual na fila de espera
        #ifdef DEBUG
        printf("task_join: task->id = %d inserida na fila de espera da task->id = %d \n", task_esperando->id, task->id);
        #endif // DEBUG
        task_yield(); // passa controle para dispatcher
        task_atual->status = 1; // tarefa atual ja pode executar
        #ifdef DEBUG
        printf("task_join: task_atual->id = %d pode executar: task_atual->status = %d \n", task_atual->id, task_atual->status);
        #endif // DEBUG
        preempcao = 1; // habilita trocas de contexto apos finalizar funcao
        return task->codigo_encerramento;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void task_sleep (int t){
    if(t > 0){
        preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
        task_atual->status = 3; // tarefa dormindo
        task_atual->despertador = systime() + t*1000; // tempo do despertador em ms
        task_t* task_dormindo = (task_t*) queue_remove((queue_t**) &queue_task, (queue_t*) task_atual); // remove task_atual da fila de prontas
        #ifdef DEBUG
        printf("task_sleep: removeu task->id = %d da fila de prontas \n", task_dormindo->id);
        #endif // DEBUG
        queue_append((queue_t**) &queue_sleep, (queue_t*) task_dormindo); // insere task_atual na fila de adormecidas
        #ifdef DEBUG
        printf("task_sleep: task->id = %d inserida na fila de adormecidas \n", task_dormindo->id);
        #endif // DEBUG
        preempcao = 1; // habilita trocas de contexto apos finalizar funcao
        task_yield(); // passa controle para dispatcher
        task_atual->status = 1; // tarefa atual ja pode executar
        #ifdef DEBUG
        printf("task_sleep: task_atual->id = %d pode executar: task_atual->status = %d \n", task_atual->id, task_atual->status);
        #endif // DEBUG
    }
    preempcao = 1; // habilita trocas de contexto apos finalizar funcao
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inicializa um semáforo apontado por s com o valor inicial value e uma fila vazia.
int sem_create (semaphore_t *s, int value){
    preempcao = 0; // evita as trocas de contexto dentro da funcao
    s->vagas = value;
    s->queue_semaforo = NULL;
    s->destruido = 0;
    if((s->vagas != value) || (s->queue_semaforo != NULL)){
        #ifdef DEBUG
        printf("sem_create: Erro ao tentar criar semaforo. \n");
        #endif // DEBUG
        preempcao = 1; // habilita novamente as trocas de contexto antes de sair da funcao
        return -1;
    }else{
        #ifdef DEBUG
        printf("sem_create: Semaforo criado com sucesso. s->vagas = %d, queue_size(s->queue_semaforo) = %d \n",
                    s->vagas, queue_size((s->queue_semaforo)));
        #endif // DEBUG
        preempcao = 1; // habilita novamente as trocas de contexto antes de sair da funcao
        return 0;
    }
}

int sem_down (semaphore_t *s){
    while(s->destruido == 0 && s != NULL){
        preempcao = 0; // evita as trocas de contexto dentro da funcao
        if(s->vagas > 0){
            s->vagas--; // usa uma vaga
            //printf("sem_down: task->id = %d conseguiu semaforo. s->vagas = %d. \n", task_atual->id, s->vagas);
            preempcao = 1; // habilita novamente as trocas de contexto antes de sair da funcao
            return 0;
        }else{
            task_atual->status = 4; // tarefa aguardando vaga no semaforo
            task_t* task_semaforo = (task_t*) queue_remove((queue_t**) &queue_task, (queue_t*) task_atual);
            queue_append((queue_t**) &(s->queue_semaforo), (queue_t*) task_semaforo);
            //printf("sem_down: task->id = %d nao conseguiu semaforo e foi inserida na fila de s. queue_size(s->queue) = %d \n", task_atual->id, queue_size(s->queue_semaforo));
            preempcao = 1; // habilita novamente as trocas de contexto antes de sair da funcao
            task_yield(); // apos falhar ao tentar uma vaga no semaforo sai da fila de prontas e volta para dispatcher
        }
    }
    preempcao = 1; // habilita novamente as trocas de contexto antes de sair da funcao
    return -1;
}

int sem_up (semaphore_t *s){
    preempcao = 0; // evita as trocas de contexto dentro da funcao
    if(s->destruido == 1 || s == NULL){
        preempcao = 1; // habilita novamente as trocas de contexto antes de sair da funcao
        return -1;
    }
    s->vagas++;
    //printf("sem_up: task->id = %d liberou vaga em s. s->vagas = %d \n", task_atual->id, s->vagas);
    if(s->vagas >= 0){
        if(queue_size(s->queue_semaforo) > 0){
            task_t* task_auxiliar = (task_t*) s->queue_semaforo;
            queue_remove((queue_t**) &(s->queue_semaforo), (queue_t*) task_auxiliar);
            queue_append((queue_t**) &queue_task, (queue_t*) task_auxiliar);
            task_auxiliar->status = 1;
            //printf("sem_up: task->id = %d removida de s->queue.\n", task_auxiliar->id);
        }
    }
    preempcao = 1; // habilita novamente as trocas de contexto antes de sair da funcao
    return 0;
}

int sem_destroy (semaphore_t *s){
    preempcao = 0; // evita as trocas de contexto dentro da funcao
    if(s == NULL){
        printf("sem_destroy: Erro: semaforo nao existe ou ja foi destruido.\n");
        preempcao = 1; // habilita novamente as trocas de contexto antes de sair da funcao
        return -1;
    }
    int tarefas_suspensas = queue_size(s->queue_semaforo);
    while(tarefas_suspensas > 0){
        task_t* task_auxiliar = (task_t*) s->queue_semaforo;
        queue_remove((queue_t**) &(s->queue_semaforo), (queue_t*) task_auxiliar);
        task_auxiliar->erro = -1;
        tarefas_suspensas = queue_size(s->queue_semaforo);
        queue_append((queue_t**) &queue_task, (queue_t*) task_auxiliar);
    }
    s->destruido = 1;
    preempcao = 1; // habilita novamente as trocas de contexto antes de sair da funcao
    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int mqueue_create (mqueue_t *queue, int max, int size){
    preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
    queue->capacidade = max; // numero maximo de mensagens na fila
    queue->buffer = (void*) malloc(sizeof(max * size)); // aloca memoria para a fila de mensagens
    queue->tamanho = size; // tamanho maximo de cada mensagem na fila de mensagens
    queue->pos_c = 0; // posicao inicial do apontador do produtor na fila
    queue->pos_p = 0; // ṕosicao inicial do apontador do consumidor na fila
    queue->destruido = 0; // condicao de fila de mensagens ativa: destruido = 0;
    sem_create (&queue->s_buffer, 1); // controla o acesso ao buffer (inicia em 1)
    sem_create (&queue->s_item, 0); // número de itens no buffer (inicia em 0)
    sem_create (&queue->s_vaga, max); // número de vagas livres no buffer
    if((queue->capacidade == max) && (queue->tamanho == size)){
        preempcao = 1; // habilita trocas de contexto apos finalizar funcao
        return 0;
    }else{
        preempcao = 1; // habilita trocas de contexto apos finalizar funcao
        return -1;
    }
}

int mqueue_send (mqueue_t *queue, void *msg){
    while (1){
        preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
        if(queue->destruido != 1){
            sem_down (&queue->s_vaga);  // aguarda uma vaga no buffer
            preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
            sem_down (&queue->s_buffer); // aguarda acesso exclusivo ao buffer
            preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
            if(queue->destruido != 1){
                bcopy(msg, &queue->buffer[queue->pos_p], queue->tamanho); // copia a mensagem para a fila de mensagens
                queue->pos_p++; // avanca uma posicao no apontador de posicoes do produtor
                queue->pos_p = queue->pos_p % queue->capacidade; // quando posicao for igual ao fim da fila faz posicao = 0
                sem_up (&queue->s_buffer); // libera o acesso ao buffer
                sem_up (&queue->s_item); // indica a presença de um novo item no buffer
                preempcao = 1; // habilita trocas de contexto apos finalizar funcao
                return 0;
            }else{
                preempcao = 1; // habilita trocas de contexto apos finalizar funcao
                return -1;
            }
        }else{
            preempcao = 1; // habilita trocas de contexto apos finalizar funcao
            return -1;
        }
    }
}

int mqueue_recv (mqueue_t *queue, void *msg){
    while (1){
        preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
        if(queue->destruido != 1){
            sem_down(&queue->s_item);  // aguarda um novo item no buffer
            preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
            sem_down(&queue->s_buffer); // aguarda acesso exclusivo ao buffer
            preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
            if(queue->destruido != 1){
                bcopy(&queue->buffer[queue->pos_c], msg, queue->tamanho); // copia o conteudo da fila de mensagens para o ponteiro msg
                queue->pos_c++; // avanca uma posicao no apontador de posicao do consumidor
                queue->pos_c = queue->pos_c % queue->capacidade; // quando posicao for igual ao fim da fila faz posicao = 0
                sem_up (&queue->s_buffer); // libera o acesso ao buffer
                sem_up (&queue->s_vaga); // indica a liberação de uma vaga no buffer
                preempcao = 1; // habilita trocas de contexto apos finalizar funcao
                return 0;
            }else{
                preempcao = 1; // habilita trocas de contexto apos finalizar funcao
                return -1;
            }
        }else{
            preempcao = 1; // habilita trocas de contexto apos finalizar funcao
            return -1;
        }
    }
}

int mqueue_destroy (mqueue_t *queue){
    preempcao = 0; // desabilita preempcao entre tarefas: impede troca de contexto no meio da funcao
    sem_destroy(&queue->s_buffer); // destroi estrutura do semaforo
    sem_destroy(&queue->s_item); // destroi estrutura do semaforo
    sem_destroy(&queue->s_vaga); // destroi estrutura do semaforo
    queue->buffer = NULL; // faz ponteiro de buffer = null
    queue->destruido = 1; // estabelece condicao de mqueue destruido
    queue = NULL;
    preempcao = 1; // habilita trocas de contexto apos finalizar funcao
    return 0;
}

int mqueue_msgs (mqueue_t *queue){
    if(queue->pos_p >= queue->pos_c){
        return (queue->pos_p - queue->pos_c); // retorna o numero de mensagens presente na fila
    }else{
        return (queue->pos_p + (queue->capacidade - queue->pos_c)); // retorna o numero de mensagens presente na fila
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


