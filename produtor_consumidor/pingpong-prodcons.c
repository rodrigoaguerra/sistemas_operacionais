#include <stdio.h>
#include <stdlib.h>
#include "pingpong.h"

#define NUM 5

//valor inteiro entre 0 e 99
int item;
//fila de inteiros com capacidade para até 5 elementos, inicialmente vazia, acessada com política FIFO.
//Pode ser implementado usando um vetor de inteiros ou a biblioteca de filas já desenvolvida.
int buffer[NUM];

int pos_p;
int pos_c;

//fila de tarefas
task_t   p1, p2, p3, c1, c2;

//semáforos, devidamente inicializados
semaphore_t s_buffer, s_item, s_vaga;

void produtor(void * arg){
   while (1){
      task_sleep(1);// atual dorme
      item = rand() % 99;// produz um item
      sem_down (&s_vaga);  // aguarda uma vaga no buffer
      sem_down (&s_buffer); // aguarda acesso exclusivo ao buffer
      buffer[pos_p] = item; // deposita o item no buffer
      pos_p++;
      if(pos_p == 5)
         pos_p = 0;
      sem_up (&s_buffer); // libera o acesso ao buffer
      sem_up (&s_item); // indica a presença de um novo item no buffer
      printf ("%s produziu %d \n", (char *) arg, item) ;
   }
}

void consumidor(void * arg){
   while (1){
      int valor;
      sem_down(&s_item);  // aguarda um novo item no buffer
      sem_down(&s_buffer); // aguarda acesso exclusivo ao buffer
      valor = buffer[pos_c];// retira o item do buffer
      pos_c++;
      if(pos_c == 5)
         pos_c = 0;
      sem_up (&s_buffer); // libera o acesso ao buffer
      sem_up (&s_vaga); // indica a liberação de uma vaga no buffer
      printf ("%s consumiu %d \n", (char *) arg, valor) ; // consome o item retirado do buffer
      task_sleep (1); // atual dorme 1 segundo
   }
}

int main (int argc, char *argv[]){

   int i;
   pos_p = 0;
   pos_c = 0;

   for(i = 0; i < NUM; i++)
     buffer[i] = 0;

   pingpong_init () ;

   sem_create (&s_buffer, 1); // controla o acesso ao buffer (inicia em 1)

   sem_create (&s_item, 0); // número de itens no buffer (inicia em 0)

   sem_create (&s_vaga, NUM); // número de vagas livres no buffer (inicia em N)


   task_create (&p1, produtor, "p1") ;
   task_create (&p2, produtor, "p2") ;
   task_create (&p3, produtor, "p3") ;
   task_create (&c1, consumidor, "                  c1") ;
   task_create (&c2, consumidor, "                  c2") ;

   task_join (&p1) ;

   sem_destroy (&s_buffer);

   sem_destroy (&s_item);

   sem_destroy (&s_vaga);

   task_join (&p2) ;
   task_join (&p3) ;
   task_join (&c1) ;
   task_join (&c2) ;


   task_exit (0) ;

   exit (0) ;
}
