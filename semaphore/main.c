#include <stdio.h>
#include <stdlib.h>
#include "pingpong.h"
/*

produtor(){
   while (true){
      task_sleep (1);
      item = random (0..99)

      down (s_vaga)

      down (s_buffer)
      insere item no buffer
      up (s_buffer)

      up (s_item)
   }
}

consumidor
{
   while (true)
   {
      down (s_item)

      down (s_buffer)
      retira item do buffer
      up (s_buffer)

      up (s_vaga)

      print item
      task_sleep (1)
   }
}

*/

