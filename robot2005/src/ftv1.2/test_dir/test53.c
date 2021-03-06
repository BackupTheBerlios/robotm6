#include "fthread.h"
#include <stdio.h>
#include <stdlib.h>

/* completely deterministic behavior */

ft_scheduler_t sched;
ft_thread_t t1,t2;
ft_event_t  e1,e2;

void run1 (void *args)
{
   int i;
   ft_thread_await (e1);
   ft_scheduler_stop (t2);
   //ft_thread_cooperate ();
   for (i=0;i<1000;i++) {
      fprintf (stdout,"*");
      ft_thread_cooperate ();
   }
}

void run2 (void *args)
{
   int i;
   ft_thread_await (e2);
   ft_scheduler_stop (t1);
   //ft_thread_cooperate ();
   for (i=0;i<1000;i++) {
      fprintf (stdout,"#");
      ft_thread_cooperate ();
   }
}

void gen (void *args)
{
   ft_thread_cooperate_n (100);
   ft_thread_generate (e2);
   ft_thread_generate (e1);   
}

void traceInstants (void *args)
{
   int i = 0;
   while (1) {
      fprintf (stdout,"\n>>>>>>>>>>> instant %d ",i++);
      ft_thread_cooperate ();
   }
}

void joiner (void *args)
{
  ft_thread_join (t1);
  ft_thread_join (t2);  
  fprintf (stdout,"exit\n");
  exit (0);
}

int main (void)
{
  sched = ft_scheduler_create ();
  t1 = ft_thread_create (sched,run1,NULL,NULL);
  t2 = ft_thread_create (sched,run2,NULL,NULL);  
  e1 = ft_event_create (sched);
  e2 = ft_event_create (sched);  

  //ft_thread_create (sched,traceInstants,NULL,NULL);
  
  ft_thread_create (sched,gen,NULL,NULL);  
  ft_thread_create (sched,joiner,NULL,NULL);
  
  ft_scheduler_start (sched);

  ft_exit ();
  return 0;
}

/* result
*#exit
end result */
