#include "fthread_internal.h"

struct _order_st
{
   int         kind;
   ft_thread_t thread;
};

/**********************************************/
order_t _make_order (int kind,ft_thread_t thread)
{
   order_t newO = (order_t)mymalloc (sizeof (struct _order_st));
   if (newO == NULL) return NULL;
   newO->kind   = kind;
   newO->thread = thread;
   return newO;
}

int _order_kind (order_t order)
{
   return order->kind;
}

ft_thread_t _order_thread (order_t order)
{
   return order->thread;
}

/**********************************************/
