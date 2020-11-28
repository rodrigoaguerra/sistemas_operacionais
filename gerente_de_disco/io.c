#include "diskdriver.h"
#include "harddisk.h"

void disk_handler (int signum){
     disk_d.sinal = 1;
     queue_append((queue_t **) &t_queue,(queue_t *) &disk_meneger);// acorda o gerente de disco (põe na fila de prontas)
}

// inicializacao do driver de disco
// retorna -1 em erro ou 0 em sucesso
// numBlocks: tamanho do disco, em blocos
// blockSize: tamanho de cada bloco do disco, em bytes
int diskdriver_init (int *numBlocks, int *blockSize){


     if (disk_cmd (DISK_CMD_INIT, 0, 0) < 0) exit(1);// inicializando disco
     disk_d.numB = disk_cmd (DISK_CMD_DISKSIZE, 0, 0);
     disk_d.blockS =  disk_cmd (DISK_CMD_BLOCKSIZE, 0, 0);

    *numBlocks = disk_d.numB;
    *blockSize = disk_d.blockS;

    disk_d.sinal = 0;   //flag do sinal
    disk_d.status = 0;  //flag de status
    disk_d.queue_req = NULL;

    sem_create (&disk_d.s_buffer, 1); // controla o acesso ao disco (inicia em 1)

    task_create(&disk_meneger, diskDriverBody, "Gerenciador de Disco");// tarefa de gerenciamento de disco

    queue_remove((queue_t**) &t_queue, (queue_t*) &disk_meneger);
    user_task--;

    return 0;
}


void diskDriverBody (void * args)
{

   while (1)
   {
      if(sem_down (&disk_d.s_buffer) < 0 ) return -1; // obtém o semáforo de acesso ao disco

      //se foi acordado devido a um sinal do disco
      if (disk_d.sinal == 1) //disco gerou um sinal
      {
         disk_d.sinal = 0;
         disk_d.status = 0;// disco esta livre
         // acorda a tarefa cujo pedido foi atendido
         queue_remove((queue_t **) &disk_d.queue_req, (queue_t *) disk_d.acorda);
         queue_append((queue_t **) &t_queue,  (queue_t *) disk_d.acorda);
         disk_d.acorda->status = 0;
      }

      //se o disco estiver livre e houver pedidos de E/S na fila
      if ((disk_d.status == 0) &&  (disk_d.queue_req != NULL)) //fila de pedidos de acesso ao disco não está vazia
      {
          disk_d.status = 1;// disco nao esta livre
          disk_d.acorda = disk_d.queue_req;
          disk_d.queue_req = disk_d.queue_req->next; // escolhe na fila o pedido a ser atendido, usando FCFS
          if(disk_d.acorda->tstatus == 1) // leitura de bloco do disco
             disk_cmd(DISK_CMD_READ, disk_d.acorda->bloco, disk_d.acorda->buffer);// solicita ao disco a operação de E/S, usando disk_cmd()
          if(disk_d.acorda->tstatus == 2) // leitura de bloco do disco
             disk_cmd(DISK_CMD_WRITE, disk_d.acorda->bloco, disk_d.acorda->buffer);// solicita ao disco a operação de E/S, usando disk_cmd()

      }

      sem_up(&disk_d.s_buffer); // libera o semáforo de acesso ao disco

      //suspende a tarefa corrente
      queue_remove((queue_t **) &t_queue,(queue_t *) &disk_meneger);

      task_switch(&dispatcher); //(retorna ao dispatcher)
   }
}

int disk_block_read (int block, void *buffer)
{
   if(block >= (disk_d.blockS * disk_d.numB)) return -1;

   if(sem_down (&disk_d.s_buffer) < 0 ) return -1;// obtém o semáforo de acesso ao disco

   atual->bloco = block;
   atual->buffer = buffer;
   atual->tstatus  = 1;

   //inclui o pedido na fila de pedidos de acesso ao disco
   queue_remove((queue_t **) &t_queue,(queue_t *) atual);
   queue_append((queue_t **) &disk_d.queue_req,(queue_t *) atual);

   if ( disk_meneger.next == NULL)//gerente de disco está dormindo
   {
      queue_append((queue_t **) &t_queue, &disk_meneger);// acorda o gerente de disco (põe na fila de prontas)
   }

   sem_up(&disk_d.s_buffer);// libera semáforo de acesso ao disco

   // suspende a tarefa corrente (retorna ao dispatcher)
   task_switch(&dispatcher);

   return 0;
}

int disk_block_write(int block, void *buffer)
{
   if(block > (disk_d.blockS * disk_d.numB)) return -1;

   if(sem_down (&disk_d.s_buffer) < 0 ) return -1;// obtém o semáforo de acesso ao disco

   //inclui o pedido na fila de pedidos de acesso ao disco
   queue_remove((queue_t **) &t_queue,(queue_t *) atual);
   atual->bloco = block;
   atual->buffer = buffer;
   atual->tstatus = 2;

   queue_append((queue_t **) &disk_d.queue_req,(queue_t *) atual);

   if ( disk_meneger.next == NULL )//gerente de disco está dormindo
   {
      queue_append((queue_t **) &t_queue,(queue_t *) &disk_meneger);// acorda o gerente de disco (põe na fila de prontas)
   }

   sem_up(&disk_d.s_buffer);// libera semáforo de acesso ao disco

   // suspende a tarefa corrente (retorna ao dispatcher)
   task_switch(&dispatcher);
   return 0;
}
