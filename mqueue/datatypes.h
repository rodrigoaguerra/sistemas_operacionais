// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Estruturas de dados internas do sistema operacional
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>
#include "queue.h"

#ifndef __DATATYPES__
#define __DATATYPES__

#define STACKSIZE 32768		/* tamanho de pilha das threads */

// Estrutura que define uma tarefa
typedef struct task_t{
    struct task_t *prev, *next ;  // para usar com a biblioteca de filas (cast)
    int tid;                      // ID da tarefa
    ucontext_t context;           // contexto da CPU na tarefa
    int prio_static;              // valor da prioridade setada
    int prio_dinamic;             // valor de decremento da prioridade
    int task_nivel;               // indentificador do nível da tarefa;
    unsigned int task_start;      // tempo que a tarefa começa;
    unsigned int task_end;        // tempo que a tarefa termina;
    int task_processor;           // tempo que a tarefa vai receber no processador;
    int task_activations;         // numero de ativaçoes da tarefa;
    int status;                   // numero do status da tarefa funcionando ou terminada;
    int num_exit;                 // numero de retorno de task exit;
    queue_t* t_join;              // tarefas depedentes;
    unsigned int t_awakening;	  // tempo para a tarefa despertar;
} task_t ;

// estrutura que define um semáforo
typedef struct{
  int s_count;
  queue_t* s_queue;
  int s_lock;
} semaphore_t ;

// estrutura que define um mutex
typedef struct
{
  // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct
{
  // preencher quando necessário
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct
{

  int max; //tamanho da fila
  int size; //tamanho size bytes
  int fim;  //fim da fila
  int ini;  //incio da fila
  int n_msg; //numero de mensagens na fila
  int tam_buffer; //tamanho de bytes do buffer;
  char* mqueue;  // fila de mensagens
  semaphore_t s_buffer; // semaforo para fila
  semaphore_t s_item;   // semaforo para itens
  semaphore_t s_vaga;   //semaforo para vagas

} mqueue_t ;

#endif
//declaração de funções
void dispatcher_body ();

//declaração de variaveis
task_t maintask;
int cont_task;
unsigned int p_clock;
queue_t *t_queue;//fila de tarefas prontas
queue_t *sleeping_queue;//fila de tarefas adormecidas
task_t dispatcher;
task_t *atual;
int user_task;
int preempt;
int quantum;
