#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "prodcons.h"

/*************************************/
typedef struct cell
{
   int value;
   struct cell *next;
}
*cell;

typedef struct list
{
   int length;
   cell first;
   cell last;
}
*list;

list add (int v, list l)
{
   cell c = (cell) malloc (sizeof (struct cell));
   c->value = v;
   c->next = NULL;

   if (l == NULL){
      l = (list) malloc (sizeof (struct list));
      l->length = 0;
      l->first = c;
   }else{
      l->last->next = c;
   }
   l->length++;
   l->last = c;
   return l;
}

void put (int v,list *l)
{
  (*l) = add (v,*l);
}

int get (list *l)
{
  int res;
  list file = *l;
  if (l == NULL) {
    fprintf (stderr, "get error!\n");
    return 0;
  }
  res = file->first->value;
  file->length--;
  if (file->last == file->first){
    file = NULL;
  }else{
    file->first = file->first->next;
  }
  return res;
}

int size (list l)
{
   if (l==NULL) return 0;
   return l->length;
}
/*************************************/
list in = NULL, out = NULL;
pthread_mutex_t producer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t consumer_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t new_input  = PTHREAD_COND_INITIALIZER;
pthread_cond_t new_output = PTHREAD_COND_INITIALIZER;

/*************************************/
void process_value (int v)
{
  int i,j;
  for (i=0;i<PROCESSING;i++) j++;
  pthread_mutex_lock(&consumer_mutex);
  put(-v,&out);
  pthread_cond_signal (&new_output);
  pthread_mutex_unlock(&consumer_mutex);
}

void* process (void *args)
{
  pthread_mutex_lock(&producer_mutex);
  while (1) {
    if (size(in) > 0){
      int v = get(&in);
      pthread_mutex_unlock(&producer_mutex);
      process_value(v);
      pthread_mutex_lock(&producer_mutex);
    }else{
      pthread_cond_wait (&new_input,&producer_mutex);
    }
  }
  return NULL;
}

void* produce (void *args)
{
  int v = 0;
  while (v < PRODUCED) {
    pthread_mutex_lock(&producer_mutex);
    if (size(in) < FILE_SIZE){
      put (v,&in);
      PRINT("%d produced\n",v);
      pthread_cond_signal (&new_input);
      v++;
      pthread_mutex_unlock (&producer_mutex);
    }else{
      pthread_mutex_unlock(&producer_mutex);
      sched_yield();
    }
  }
  return NULL;
}

void* consume (void *args)
{
  int v = 0;
  while (v < PRODUCED) {
    pthread_mutex_lock(&consumer_mutex);
    if (size(out) > 0){
      int res = get (&out);
      PRINT("consume %d\n",res);
      v++;
    }else{
      pthread_cond_wait (&new_output,&consumer_mutex);
    }
    pthread_mutex_unlock(&consumer_mutex);     
  }
  exit (0);
  return NULL;
}

/*************************************/
int main(void)
{
   int i;
   pthread_t producer,consumer;
   pthread_t thread_array[MAX_THREADS];

   for (i=0; i<MAX_THREADS; i++){
      pthread_create (&thread_array[i],NULL,process,NULL);
   }

   pthread_create (&producer,NULL,produce,NULL);
   pthread_create (&consumer,NULL,consume,NULL);
   
   pthread_exit (0);
   return 0;
}

