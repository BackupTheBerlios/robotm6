#include "fthread.h"
#include <stdio.h>

void print (void *txt)
{
   while (1) {
      printf ((char*)txt);
      ft_thread_cooperate ();
   }
}

int main(void)
{
  ft_scheduler_t sched = ft_scheduler_create ();
  ft_thread_create (sched,print,NULL,"Hello");
  ft_thread_create (sched,print,NULL," World!\n");
  ft_scheduler_start (sched);
  ft_exit ();
  return 0;
}
