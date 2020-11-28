#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

//------------------------------------------------------------------------------
// Insere um elemento no final da fila.
// Condicoes a verificar, gerando msgs de erro:
// - o elemento nao deve estar em outra fila
void queue_append (queue_t **queue, queue_t *elem){
    // - a fila deve existir
    if(queue == NULL){
	printf("fila nao existe!\n");
    	return;
    }
     

    // - o elemento deve existir
    if(elem == NULL){
	printf("elemento nao existe!\n");
	return; 
    }	
    

    // - o elemento nao deve estar em outra fila
    if(elem->next != NULL || elem->prev != NULL){
    	printf("elemento ja esta em outra fila! \n");
	return;
    }

    if(*queue != NULL){
        //fila com elemento(s) inseridos
        queue_t *aux = *queue;
        while(aux->next != *queue && aux != elem){
              aux = aux->next;
        }
        //O primeiro e o ultimo apontam para o elemento
        (*queue)->prev = elem;
        aux->next = elem;
        //O elemento aponta para o primeiro e para o ultimo
        elem->next = *queue;
        elem->prev = aux;
    }else{
       //uma fila vazia e um elemento isolado
       elem->next = elem;
       elem->prev = elem;
       *queue = elem;
    }
}

//------------------------------------------------------------------------------
// Remove o elemento indicado da fila, sem o destruir.
// Condicoes a verificar, gerando msgs de erro:


queue_t *queue_remove (queue_t **queue, queue_t *elem){
    // - a fila deve existir
    // - o elemento deve existir
    if(queue != NULL && elem != NULL){
       // - a fila nao deve estar vazia
       if(*queue != NULL){
           queue_t *aux = *queue;
           // - o elemento deve pertencer a fila indicada
           int pertence = 0;
           while(aux->next != *queue && pertence == 0){
                 if(aux == elem){
                    pertence++;
                 }
                 aux = aux->next;
           }
           if(aux == elem){
                pertence++;
           }
           if(pertence == 1){
              aux = *queue;
              if(aux->next != *queue){
                while(aux->next != elem){
                      aux = aux->next;
                }

                aux->next = elem->next;
                aux->next->prev = elem->prev;

                if(elem == *queue){
                    *queue = aux->next;
                }


                // Retorno: apontador para o elemento removido
                elem->next = NULL;
                elem->prev = NULL;

                return elem;
              }else{
                *queue = NULL;
                // Retorno: apontador para o elemento removido
                elem->next = NULL;
                elem->prev = NULL;

                return elem;
              }
           }
       }
	}

    //NULL se erro
    return NULL;
}

//------------------------------------------------------------------------------
// Conta o numero de elementos na fila
// Retorno: numero de elementos na fila
int queue_size (queue_t *queue){
    int i = 0;
    if(queue != NULL){
        queue_t *aux = queue;
        while(aux->next != queue){
            i++;
            aux = aux->next;
        }
        i++;
    }
    return i;
}

//------------------------------------------------------------------------------
// Percorre a fila e imprime na tela seu conteúdo. A impressão de cada
// elemento é feita por uma função externa, definida pelo programa que
// usa a biblioteca. Essa função deve ter o seguinte protótipo:
//
// void print_elem (void *ptr) ; // ptr aponta para o elemento a imprimir
void queue_print (char *name, queue_t *queue, void print_elem (void*) ) {
       if(queue != NULL){
        printf("%s: [", name);
        queue_t *aux = queue;
        while(aux->next != queue){
             print_elem(aux);
             printf(" ");
             aux = aux->next;
        }
        print_elem(aux);
        printf("] \n");
      }else{
        printf("%s: [] \n", name);
      }
}

