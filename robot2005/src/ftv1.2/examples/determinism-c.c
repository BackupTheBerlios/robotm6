#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_t       t1,t2;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER,
                lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  e1    = PTHREAD_COND_INITIALIZER,
                e2    = PTHREAD_COND_INITIALIZER;


#define MAX 1000000
void* run1 (void *args)
{
   int i;
   pthread_mutex_lock (&lock1);
   pthread_cond_wait (&e1,&lock1);
   pthread_mutex_unlock (&lock1);

   pthread_cancel (t2);

   for (i=0;i<MAX;i++) {
      fprintf (stderr,"*");
   }
   fprintf (stderr,"t1 terminated\n");
   return NULL;
}


void* run2 (void *args)
{
   int i;
   pthread_mutex_lock (&lock2);
   pthread_cond_wait (&e2,&lock2);
   pthread_mutex_unlock (&lock2);

   pthread_cancel (t1);
 
   for (i=0;i<MAX;i++) {
      fprintf (stderr,"#");
   }
   return NULL;
}

void* gen (void *args)
{
   usleep (1);

   pthread_mutex_lock (&lock1);
   pthread_mutex_lock (&lock2);

   pthread_cond_broadcast (&e1);
   fprintf (stdout,"broadcast e1\n");  
   pthread_cond_broadcast (&e2);
   fprintf (stdout,"broadcast e2\n");
   
   pthread_mutex_unlock (&lock1);
   pthread_mutex_unlock (&lock2);
   
   return NULL;
}


int main (void)
{
  int c, *cell = &c;
  pthread_t g;

  //e2 = e1;
  
  pthread_create (&t1,NULL,run1,NULL);
  pthread_create (&t2,NULL,run2,NULL);  

  pthread_create (&g,NULL,gen,NULL);  

  pthread_join (t1,(void**)cell);
  pthread_join (t2,(void**)cell);
  
  fprintf (stdout,"exit\n");
  exit (0);

  return 0;
}

/* result
NON DETERMINISTIC ON A MULTIPROCESSOR MACHINE
end result */
