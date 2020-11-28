#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

// operating system check
#if defined(_WIN32) || (!defined(__unix__) && !defined(__unix) && (!defined(__APPLE__) || !defined(__MACH__)))
#warning Este codigo foi planejado para ambientes UNIX (LInux, *BSD, MacOS). A compilacao e execucao em outros ambientes e responsabilidade do usuario.
#endif

// tratador do sinal
void tratador (int signum)
{
   printf ("Recebi o sinal %d\n", signum) ;
}

int main ()
{
   struct itimerval timer;

   // define a ação para o sinal de timer
   if (signal (SIGALRM, tratador) == SIG_ERR)
   {
      perror ("Erro em signal: ") ;
      exit (1) ;
   }

   // ajusta valores do temporizador
   timer.it_value.tv_usec = 0 ;      // primeiro disparo, em micro-segundos
   timer.it_value.tv_sec  = 3 ;      // primeiro disparo, em segundos
   timer.it_interval.tv_usec = 0 ;   // disparos subsequentes, em micro-segundos
   timer.it_interval.tv_sec  = 1 ;   // disparos subsequentes, em segundos

   // arma o temporizador ITIMER_REAL (vide man setitimer)
   if (setitimer (ITIMER_REAL, &timer, 0) < 0)
   {
      perror ("Erro em setitimer: ") ;
      exit (1) ;
   }

   // laco vazio
   while (1) ;
}
