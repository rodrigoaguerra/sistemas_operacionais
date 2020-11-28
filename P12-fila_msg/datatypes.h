// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Estruturas de dados internas do sistema operacional

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "pingpong.h"
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#ifndef __DATATYPES__
#define __DATATYPES__
#define STACKSIZE 32768		/* tamanho de pilha das threads */

// ESTRUTURAS CRIADAS NA ATIVIDADE P0-FILA GENERICA
typedef struct queue_t
{
   struct queue_t *prev ;  // aponta para o elemento anterior na fila
   struct queue_t *next ;  // aponta para o elemento seguinte na fila
} queue_t ;

// Estrutura que define uma tarefa
typedef struct task_t
{
    struct task_ *prev, *next; // ponteiros paratarefas proxima e anterior
    int id; // id da tarefa
    ucontext_t context; // contexto da tarefa
    void *stack;
    int prio_estatica; // prioridade estatica da tarefa
    int prio_dinamica; // prioridade dinamica da tarefa
    int tarefa_sistema; // se for = 1 -> tarefa de sistema; se for = 0 -> tarefa de usuario
    int quantum; // quantum da tarefa
    int inicio; // variavel que armazena o tempo em ms da criacao da tarefa
    int fim; // variavel que armazena o tempo em ms da finalizacao da tarefa
    int processamento; // variavel que armazena o tempo em processador da tarefa, em ms
    int ativacoes; // variavel que armazena o numero de ativacoes da tarefa
    int codigo_encerramento; // variavel criada para armazenar o codigo de encerramento da tarefa, especificado na chamada da task_exit
    int status; // variavel criada para armazenar status da tarefa: 0 = encerrada, 1 = ativa, 2 = esperando, 3 = dormindo
    queue_t* queue_espera; // fila de tarefas em espera
    int despertador; // armazena o tempo, em ms, que a tarefa pode sair da fila de adormecidas
    int erro; // codigo de erro retornado pela tarefa quando estava em alguma fila que foi destruida
  // preencher quando necessário
} task_t ;

// estrutura que define um semáforo
typedef struct
{
    int vagas; // vagas disponiveis no semaforo
    queue_t* queue_semaforo; // fila de tarefas que falharam ao tentar s_down - estao aguardando vaga no semaforo
    int destruido; // codigo utilizado para condicao de semaforo destruido ( 1 = destruido; 0 = ativo)
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
    int capacidade; // numero maximo de mensagens
    int* buffer; // fila ou vetor com tamanho definido pela capacidade maxima de msgs
    int tamanho; // tamanho maximo de cada msg
    semaphore_t s_buffer; // controla o acesso ao buffer
    semaphore_t s_item; // controla o acesso ao item e indica existencia de algum
    semaphore_t s_vaga; // controla acesso a uma vaga no semaforo
    int pos_p, pos_c; // indica posicao na fila tanto no produtor quanto no consumidor
    int destruido; // codigo utilizado para condicao de mqueue destruido ( 1 = destruido; 0 = ativo)
  // preencher quando necessário
} mqueue_t ;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ESTRUTURAS CRIADAS NA ATIVIDADE P0-FILA GENERICA
void queue_append (queue_t **queue, queue_t *elem) ; // Insere um elemento no final da fila.

queue_t *queue_remove (queue_t **queue, queue_t *elem) ; // Remove o elemento indicado da fila, sem o destruir.

int queue_size (queue_t *queue) ; // Conta o numero de elementos na fila

void queue_print (char *name, queue_t *queue, void print_elem (void*) ) ; // Percorre a fila e imprime na tela seu conteúdo.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ESTRUTURAS CRIADAS NA ATIVIDADE P2-GESTAO DE TAREFAS
void pingpong_init (); // inicializa paremetros do sistema

int task_create (task_t *task,			        // descritor da nova tarefa
                 void (*start_func)(void *),	// funcao corpo da tarefa
                 void *arg);

void task_exit (int exitCode); // encerra tarefa corrente

int task_switch (task_t *task); // troca para task

int task_id (); // retorna o identificador da tarefa corrente (main eh 0)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ESTRUTURAS CRIADAS NA ATIVIDADE P3-DESPACHANTE DE TAREFAS
task_t* scheduler();  // funcao que define a tarefa a ser executada

void dispatcher_body (void *arg); // tarefa que gerenciona fila de tarefas prontas para executar

void task_yield (); // transfere processador para tarefa dispatcher
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ESTRUTURAS CRIADAS NA ATIVIDADE P4-ESCALONADOR POR PRIORIDADE
void task_setprio (task_t *task, int prio); // define prioridade estatica da tarefa

int task_getprio (task_t *task); // retorna prioridade estatica da tarefa
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCOES CRIADAS NA ATIVIDADE P5-PREEMPCAO E COMPARTILHAMENTO DE TEMPO
void conta_ticks(int sinal); // realiza o controle de tempo/quantum das tarefas assim como do relogio global
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCAO CRIADA NA ATIVIDADE P6-CONTABILIZACAO DE TAREFAS
unsigned int systime () ; // funcao que retorna o tempo atual, que fica sempre atualizado pelo conta_ticks
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCAO CRIADA NA ATIVIDADE P7-TAREFA MAIN
int main (int argc, char *argv[]); // funcao geralmente fornecida pelo programa de teste do professor
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCAO CRIADA NA ATIVIDADE P8-OPERADOR JOIN
int task_join (task_t *task); // funcao que faz a tarefa que realizou a chamada aguardar o termino de task
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCAO CRIADA NA ATIVIDADE P9-TASK_SLEEP
void task_sleep (int t); // funcao que faz a tarefa atual dormir t milisegundos
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCAO CRIADA NA ATIVIDADE P10- SEMAFOROS
int sem_create (semaphore_t *s, int value); // cria um semaforo com numero de vagas = value
int sem_down (semaphore_t *s); // tenta acesso no semaforo, entrando em fila de espera caso nao consiga vaga
int sem_up (semaphore_t *s); // libera uma vaga no semaforo, liberando uma tarefa da fila de espera, caso exista
int sem_destroy (semaphore_t *s); // destroi o semaforo, liberando as tarefas que estavam esperando vaga com um codigo de erro
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCAO CRIADA NAS ATIVIDADES P11/12-USO DE SEMAFOROS E FILA DE MENSAGENS
int mqueue_create (mqueue_t *queue, int max, int size); // cria fila de msgs, com "max" msgs de tamanho = "size" cada
int mqueue_send (mqueue_t *queue, void *msg); // envia uma msg ou fica aguardando vaga para enviar : produtor
int mqueue_recv (mqueue_t *queue, void *msg); // retira uma msg ou fica aguardando o envio de uma : consumidor
int mqueue_destroy (mqueue_t *queue); // destroi a fila de msgs, assim como as estruturas utilizadas por ela
int mqueue_msgs (mqueue_t *queue); // retorna o numero de msgs na fila de msgs
/////////////////////////////////////////////////////////////////////////////////////////////////////////////



int contador_tarefas; // variavel utilizada para definir a id das tarefas
task_t task_main; // tarefa que armazena o contexo da main
task_t *task_atual; // ponteiro utilizado para verificar tarefa corrente
task_t dispatcher; // tarefa que realizara o gerenciamento da fila de tarefas
queue_t* queue_task; // fila de tarefas prontas para executar
queue_t* queue_sleep; // fila de tarefas adormecidas
int user_task; // variavel utilizada para contabilizar as tarefas da fila de tarefas prontas pra executar
int envelhecimento; // task aging - envelhecimento de tarefas - no LINUX a prioridade maior é - 20; prioridade menor +19;
int relogio;  // variavel global criada para armazenar o tempo do sistema em ms
int quantum; // estabelece o quantum da tarefa
int preempcao; // variavel utilizada para habilitar ou desabilitar a preempcao entre tarefas


#endif
