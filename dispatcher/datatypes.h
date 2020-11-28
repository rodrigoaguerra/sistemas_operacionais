// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Estruturas de dados internas do sistema operacional
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "queue.h"
#include "pingpong.h"


#ifndef __DATATYPES__
#define __DATATYPES__

#define STACKSIZE 32768		/* tamanho de pilha das threads */

// Estrutura que define uma tarefa
typedef struct task_t{
    struct task_t *prev, *next ;   // para usar com a biblioteca de filas (cast)
    int tid;                      // ID da tarefa
    ucontext_t context;
    int prio_static;
    int prio_dinamic;

} task_t ;

// estrutura que define um semáforo
typedef struct{
  // preencher quando necessário
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
  // preencher quando necessário
} mqueue_t ;

#endif
//declaração de funções
void dispatcher_body ();

//declaração de variaveis
queue_t *queue;
task_t dispatcher;
int user_task;
