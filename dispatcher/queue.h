//------------------------------------------------------------------------------
// Defini��o e opera��es em uma fila gen�rica.
// by Carlos Maziero, DAINF/UTFPR, maio/2013
//------------------------------------------------------------------------------

#ifndef __QUEUE__
#define __QUEUE__

#ifndef NULL
#define NULL ((void *) 0)
#endif

//------------------------------------------------------------------------------
// estrutura de uma fila gen�rica, sem conte�do definido.
// Veja um exemplo de uso desta estrutura em testafila.c

typedef struct queue_t
{
   struct queue_t *prev ;  // aponta para o elemento anterior na fila
   struct queue_t *next ;  // aponta para o elemento seguinte na fila
} queue_t ;

//------------------------------------------------------------------------------
// Insere um elemento no final da fila.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - o elemento deve existir
// - o elemento nao deve estar em outra fila

void queue_append (queue_t **queue, queue_t *elem) ;

//------------------------------------------------------------------------------
// Remove o elemento indicado da fila, sem o destruir.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - a fila nao deve estar vazia
// - o elemento deve existir
// - o elemento deve pertencer a fila indicada
// Retorno: apontador para o elemento removido, ou NULL se erro

queue_t *queue_remove (queue_t **queue, queue_t *elem) ;

//------------------------------------------------------------------------------
// Conta o numero de elementos na fila
// Retorno: numero de elementos na fila

int queue_size (queue_t *queue) ;

//------------------------------------------------------------------------------
// Percorre a fila e imprime na tela seu conte�do. A impress�o de cada
// elemento � feita por uma fun��o externa, definida pelo programa que
// usa a biblioteca. Essa fun��o deve ter o seguinte prot�tipo:
//
// void print_elem (void *ptr) ; // ptr aponta para o elemento a imprimir

void queue_print (char *name, queue_t *queue, void print_elem (void*) ) ;

#endif

