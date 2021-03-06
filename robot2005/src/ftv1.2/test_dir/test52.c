#include "fthread.h"
#include <stdio.h>
#include <stdlib.h>

/* mutual stops */

#define MAX 10
ft_scheduler_t sched;
ft_thread_t *thread_table;
ft_event_t  *event_table;

void cleanup (void *args)
{
   int num = (int)args;
   ft_scheduler_stop (thread_table[(1+num)%MAX]);
   fprintf (stdout,"- stopping thread %d\n",(1+num)%MAX);      
}


void run (void *args)
{
   int num = (int)args;
   fprintf (stdout,"launching thread %d\n",num);   
   ft_thread_await (event_table[num]);
   fprintf (stdout,"event %d received\n",num);   
   ft_scheduler_stop (thread_table[num]);
   fprintf (stdout,"stopping thread %d\n",num);
   while (1) ft_thread_cooperate ();
}

void gen (void *args)
{
   ft_thread_cooperate_n (100);
   ft_thread_generate (event_table[5]);
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
   int i;
   for (i=0; i<MAX; i++) ft_thread_join (thread_table[i]);
   fprintf (stdout,"exit\n");
   exit (0);
}

int main (void)
{
  int i;
  sched = ft_scheduler_create ();
  thread_table = malloc (MAX*sizeof (ft_thread_t));
  event_table  = malloc (MAX*sizeof (ft_event_t));
  
  for (i=0;i<MAX;i++){
     thread_table[i] = ft_thread_create (sched,run,cleanup,(void**)i);
     event_table[i]  = ft_event_create (sched);
  }

  //ft_thread_create (sched,traceInstants,NULL,NULL);
  
  ft_thread_create (sched,gen,NULL,NULL);  
  ft_thread_create (sched,joiner,NULL,NULL);
   
  ft_scheduler_start (sched);

  ft_exit ();
  return 0;
}

/* result
launching thread 0
launching thread 1
launching thread 2
launching thread 3
launching thread 4
launching thread 5
launching thread 6
launching thread 7
launching thread 8
launching thread 9
event 5 received
stopping thread 5
- stopping thread 6
- stopping thread 7
- stopping thread 8
- stopping thread 9
- stopping thread 0
- stopping thread 1
- stopping thread 2
- stopping thread 3
- stopping thread 4
- stopping thread 5
exit
end result */
