#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

static long sum = 0;
static long numcalled = 0;
int malloc_status = 0;

void *mymalloc (int size)
{
   void *res;
   sum += size;
   numcalled++;  
   res = malloc (size);
   if (NULL == res) { malloc_status = 1; }
   return res;
}

void *myrealloc (void *ptr,int size)
{
   void *res;
   sum += size;
   numcalled++;
   res = realloc (ptr,size);
   if (NULL == res) { malloc_status = 1; }
   return res;
}

void myexit (int n)
{
   fprintf (stderr,"malloc: %ld (called %ld times)\n",sum,numcalled);
   exit (n);
}

void myfree (void *p)
{
   free (p);
}
