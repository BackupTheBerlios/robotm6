#include "fthread.h"
#include <stdio.h>

DEFINE_AUTOMATON (print)
{
  BEGIN_AUTOMATON
     STATE (0) {
	printf ((char*)ARGS);
        GOTO(0);
     }
  END_AUTOMATON
}

int main(void)
{
  ft_scheduler_t sched = ft_scheduler_create ();
  ft_automaton_create (sched,print,NULL,"Hello");
  ft_automaton_create (sched,print,NULL," World!\n");
  ft_scheduler_start (sched);
  ft_exit ();
  return 0;
}
