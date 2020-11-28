#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

// operating system check
#if defined(_WIN32) || (!defined(__unix__) && !defined(__unix) && (!defined(__APPLE__) || !defined(__MACH__)))
#warning Este codigo foi planejado para ambientes UNIX (LInux, *BSD, MacOS). A compilacao e execucao em outros ambientes e responsabilidade do usuario.
#endif

/* função que tratará os sinais recebidos */
void tratador (int signum)
{
   printf ("Recebi o sinal %d\n", signum) ;
}

int main (void)
{
   /* registra ação para o sinal SIGINT (^C do teclado) */
   if (signal (SIGINT, tratador) == SIG_ERR)
   {
      perror ("Erro em signal: ") ;
      exit (1) ;
   }

   /* laço vazio */
   while (1) ;
}
