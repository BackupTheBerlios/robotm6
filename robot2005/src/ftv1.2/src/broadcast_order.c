#include "fthread_internal.h"

struct _broadcast_order_st
{
  ft_event_t  event;
  int         pure;
  void       *value;
};

/************************************************/
broadcast_order_t _make_broadcast_order (ft_event_t event,
					 int pure,
					 void *value)
{
   broadcast_order_t newB = (broadcast_order_t)mymalloc (sizeof (struct _broadcast_order_st));
   if (newB == NULL) return NULL;
   newB->event  = event;
   newB->pure   = pure;
   newB->value  = value;
   return newB;
}

ft_event_t _broadcast_order_event (broadcast_order_t broadcast)
{
   return broadcast->event;
}

int _broadcast_order_is_pure (broadcast_order_t broadcast)
{
   return broadcast->pure;
}

void* _broadcast_order_value (broadcast_order_t broadcast)
{
   return broadcast->value;
}
