#include "datatypes.h"

void task_yield (){
     task_switch (&dispatcher);
}

task_t* scheduler(){
        queue = queue->next;
        return (task_t*) queue;
}

void dispatcher_body () // dispatcher é uma tarefa
{
   task_t* next;
   while ( user_task  > 1 ){
      next = scheduler() ;  // scheduler é uma função
      if (next){
          // ações antes de lançar a tarefa "next", se houverem
         task_switch (next) ; // transfere controle para a tarefa "next"

          // ações após retornar da tarefa "next", se houverem
      }
   }
   task_exit(1) ; // encerra a tarefa dispatcher
}
