#include "fthread.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/*  */

/*************************************/
DEFINE_AUTOMATON(autom_behav)
{
  BEGIN_AUTOMATON
     
     STATE_STAY(0,10)
        fprintf (stdout, "exit!\n");
	exit(0);

   END_AUTOMATON
}

/*************************************/
void controler (void *args)
{
   ft_thread_t autom = (ft_thread_t)args;
   ft_thread_cooperate_n (3);
   fprintf (stdout, "suspend autom! ");
   ft_scheduler_suspend (autom);
   ft_thread_cooperate_n (15);
   fprintf (stdout, "resume autom! ");
   ft_scheduler_resume (autom);   
}

/*************************************/
void traceInstants (void *args)
{
   int i = 0;
   for (i=0;;i++) {
      fprintf(stdout,"\n>>>>>>>>>>> instant %d: ",i);
      ft_thread_cooperate ();
   }
}

int main(void)
{
   ft_thread_t autom;
   ft_scheduler_t sched = ft_scheduler_create ();
   
   autom = ft_automaton_create (sched,autom_behav,NULL,NULL);   
   ft_thread_create (sched,traceInstants,NULL,NULL);
   ft_thread_create (sched,controler,NULL,autom); 
   
   ft_scheduler_start (sched);
   ft_exit ();
   return 0;
}


/* result

>>>>>>>>>>> instant 0:
>>>>>>>>>>> instant 1:
>>>>>>>>>>> instant 2:
>>>>>>>>>>> instant 3: suspend autom!
>>>>>>>>>>> instant 4:
>>>>>>>>>>> instant 5:
>>>>>>>>>>> instant 6:
>>>>>>>>>>> instant 7:
>>>>>>>>>>> instant 8:
>>>>>>>>>>> instant 9:
>>>>>>>>>>> instant 10:
>>>>>>>>>>> instant 11:
>>>>>>>>>>> instant 12:
>>>>>>>>>>> instant 13:
>>>>>>>>>>> instant 14:
>>>>>>>>>>> instant 15:
>>>>>>>>>>> instant 16:
>>>>>>>>>>> instant 17:
>>>>>>>>>>> instant 18: resume autom!
>>>>>>>>>>> instant 19:
>>>>>>>>>>> instant 20:
>>>>>>>>>>> instant 21:
>>>>>>>>>>> instant 22:
>>>>>>>>>>> instant 23:
>>>>>>>>>>> instant 24: exit!
end result */

