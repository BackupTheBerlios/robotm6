#include "fthread_internal.h"
#include <sched.h> // for yield!

static int direct_access  = 1; // direct access to the next executable thread
static int trace_instants = 0; // trace beginning of instants

/******************************************/
// the scheduler type
/******************************************/
struct ft_scheduler_st
{
   ft_thread_t         self;
   
   item_list_t         linked;       // list of running threads
   item_cell_t         current_cell; // current executed cell
   
   item_list_t         orders;
   
   item_list_t         to_broadcast;
   int                 active;      // number of threads to execute

   pthread_mutex_t     sleeping;
   pthread_cond_t      awake;

   item_list_t         to_finalize;

   int                 move;
   int                 eoi;
   long                instant;

   item_list_t         timer;

   int                 determinist;  
   
   int                 well_created; // == FT_MAGIC if thread is OK
};

void ft_scheduler_deallocate (ft_scheduler_t sched)
{
   _deallocate_item_list (sched->linked);
   _deallocate_item_list (sched->orders);
   _deallocate_item_list (sched->to_broadcast);
   _deallocate_item_list (sched->to_finalize);
   _deallocate_item_list (sched->timer);
   sched->well_created = 0;
   free (sched);
}

/******************************************/
/* to determine if a scheduler is correctly created, by
   ft_scheduler_create */
/******************************************/
int _scheduler_well_created (ft_scheduler_t sched)
{
   return sched != NULL && sched->well_created == FT_MAGIC;
}

/******************************************/
// accessors
/******************************************/
ft_thread_t _get_scheduler_self (ft_scheduler_t sched)
{
   return sched->self;
}

item_cell_t _get_scheduler_current_cell (ft_scheduler_t sched)
{
   return sched->current_cell;
}

int _get_scheduler_eoi (ft_scheduler_t sched)
{
   return sched->eoi;
}

void _set_scheduler_move (ft_scheduler_t sched)
{
   sched->move = 1;
}

long _get_scheduler_instant (ft_scheduler_t sched)
{
   return sched->instant;
}

/*****************************************************/
// creation
/*****************************************************/
ft_scheduler_t ft_scheduler_create (void)
{
  pthread_mutex_t sleeping;
  pthread_cond_t awake;
  
  ft_scheduler_t sched = (ft_scheduler_t)mymalloc (sizeof (struct ft_scheduler_st));
  if (sched == NULL) return NULL;
  
  sched->self = _make_thread (NULL,NULL);
  if (sched->self == NULL) return NULL;
  
  if (NULL == (sched->linked       = _make_item_list ())) return NULL;
  if (NULL == (sched->orders       = _make_item_list ())) return NULL;
  if (NULL == (sched->to_broadcast = _make_item_list ())) return NULL;
  if (NULL == (sched->timer        = _make_item_list ())) return NULL;
  if (NULL == (sched->to_finalize  = _make_item_list ())) return NULL;
  
  pthread_mutex_init (&sleeping,NULL);
  pthread_cond_init  (&awake,NULL);

  sched->move         = 0;
  sched->eoi          = 0;
  sched->instant      = 0;
  sched->sleeping     = sleeping;
  sched->awake        = awake;
  sched->active       = 0;

  sched->determinist  = 1;

  sched->well_created = FT_MAGIC;
  return sched;
}

ft_scheduler_t ft_scheduler_nd_create (void)
{
   ft_scheduler_t sched = ft_scheduler_create ();
   sched->determinist = 0;   
   return sched;   
}
   
/*****************************************************/
// trace instants
/*****************************************************/
void ft_trace_instants (char b)
{
   trace_instants = b;
}

static void _trace_instant (ft_scheduler_t sched)
{
   fprintf (stderr,"\n>>> instant %ld ",sched->instant);
   fprintf (stderr,"(%d)",_length_of_item_list (sched->linked));
   fprintf (stderr,": ");
}

/****************************************************/
// timer of the scheduler
/****************************************************/
void _register_in_timer (ft_scheduler_t sched)
{
   item_cell_t cell =
      _append_item_list (sched->timer,sched->current_cell);
   _set_thread_timer_cell (ft_thread_self (),cell);
}

void _extract_from_timer (ft_scheduler_t sched,item_cell_t cell)
{
   _extract (sched->timer,cell);
}

static int _process_deadline (void *item,void *arg)
{
   item_cell_t cell     = (item_cell_t)item;
   ft_thread_t thread   = (ft_thread_t)_content_of_cell (cell);
   ft_scheduler_t sched = (ft_scheduler_t)arg;

   if (_get_thread_is_suspended (thread)) return KEEP;

   _decr_thread_deadline (thread);
   
   if (_get_thread_deadline (thread) > 0) return KEEP;
   if (_get_thread_status (thread) == _WAIT) {
      _unmask (sched,cell);
      _unregister_in_event (thread);
   }
   return REMOVE;
}

static void _process_all_deadlines (ft_scheduler_t sched)
{
   _apply_on_items (sched->timer,_process_deadline,sched);
}

/****************************************************/
// scheduler reset
/****************************************************/
static void _reset_scheduler (ft_scheduler_t sched)
{
   sched->move = sched->eoi = 0;
   sched->instant++;
   _process_all_deadlines    (sched);
}

/******************************************************/
// no busy-waiting for schedulers
/******************************************************/
static int _something_to_do (ft_scheduler_t sched)
{
   return _length_of_item_list (sched->linked)       > 0
       || _length_of_item_list (sched->timer)        > 0
       || _length_of_item_list (sched->orders)       > 0
       || _length_of_item_list (sched->to_broadcast) > 0;
}

static void _sleep_if_nothing_to_do (ft_scheduler_t sched)
{
   _PTH_LOCK(sched->sleeping);
   while (!_something_to_do (sched)){
      //fprintf(stderr,"sched %ld falls asleep..\n",(long)sched->self);
      _PTH_WAIT(sched->awake,sched->sleeping);
      //fprintf(stderr,"sched %ld awakes..\n",(long)sched->self);
   }
   _PTH_UNLOCK(sched->sleeping);
}

static void _awake_scheduler (ft_scheduler_t sched)
{
   _PTH_LOCK(sched->sleeping);
   //fprintf (stderr,"awake sched thread %ld\n",(long)sched->self);
   _PTH_NOTIFY(sched->awake,sched->sleeping);
   _PTH_UNLOCK(sched->sleeping);  
}

/******************************************************/
// firing of threads linked to the scheduler
/******************************************************/
static int _fireable (ft_thread_t thread)
{
   return !_get_thread_is_suspended (thread)
       &&  _get_thread_status (thread) == _CONT;
}

static int _fire_one (item_cell_t cell,void *arg)
{
   ft_thread_t thread = (ft_thread_t)_content_of_cell (cell);
   ft_scheduler_t sched  = (ft_scheduler_t)arg;
   if (_fireable (thread)) {
      sched->current_cell = cell;
      if (_is_automaton (thread)) {
	 //fprintf(stderr,"sched fires automaton %ld\n",(long)thread);
	 _run_as_automaton (thread);
	 //fprintf(stderr,"returns from automaton %ld\n",(long)thread);
      } else {
	 //fprintf(stderr,"sched fires thread %ld\n",(long)thread);
	 _transmit_token_to_thread (thread);
	 //fprintf(stderr,"return from thread %ld\n",(long)thread);
      }
      if (_get_thread_status (thread) != _CONT) sched->active--;
      if (direct_access && _get_thread_status (thread) == _WAIT) {
	 return MASK;
      }
   }
   return KEEP;
}

static void _fire_all_threads (ft_scheduler_t sched)
{
   _apply_on_cells (sched->linked,_fire_one,sched);
}

/******************************************************/
// stability of the scheduler
/******************************************************/
static int _stable (ft_scheduler_t sched)
{
   int res;
   _lock_thread (sched->self);
   res = sched->active == 0;
   _unlock_thread (sched->self);
   return res;
}

/*****************************************************/
// finalization of threads (when stopped)
/*****************************************************/
static int finalize_one_thread (void *item,void *arg)
{
   _finalize ((ft_thread_t)item);
   return REMOVE;
}

static void _finalize_stopped_threads (ft_scheduler_t sched)
{
   if (_length_of_item_list (sched->to_finalize)) {
      _apply_on_items (sched->to_finalize,finalize_one_thread,NULL);
   }
}

/*******************************************************/
// transfer events
/*******************************************************/
/* Precondition: all events and scheduler targets are verified. This
is true because all broadcast orders are produced by
ft_scheduler_broadcast or ft_scheduler_broadcast_value */
static int transfer_event (void *item,void *arg)
{
   broadcast_order_t order = (broadcast_order_t)item;
   ft_event_t event = (ft_event_t)_broadcast_order_event (order);
   if (_broadcast_order_is_pure (order))
      _event_generate (event);
   else
      _event_generate_value (event,_broadcast_order_value (order));
   free (order);
   return REMOVE;
}

static void _transfer_all_events (ft_scheduler_t sched)
{
   _apply_on_items (sched->to_broadcast,transfer_event,NULL);
}

/*******************************************************/
// broadcast orders
/*******************************************************/
static int _send_broadcast_order (ft_event_t event,int pure,void *val){
   item_cell_t res;
   broadcast_order_t order;
   ft_scheduler_t sched;

   _VERIFY_EVENT (event);
   sched = _get_event_scheduler (event);
   _VERIFY_SCHEDULER (sched);
   
   if (NULL == (order = _make_broadcast_order (event,pure,val)))
      return EBADMEM;
   
   _lock_thread (sched->self);
   res = _append_item_list (sched->to_broadcast,order);
   _unlock_thread (sched->self);
   
   _awake_scheduler (sched); // awake possibly sleeping scheduler  
   return res != NULL ? OK : EBADMEM;
}

int ft_scheduler_broadcast (ft_event_t event){
   return _send_broadcast_order (event,1,NULL);
}

int ft_scheduler_broadcast_value (ft_event_t event,void *val){
   return _send_broadcast_order (event,0,val);
}

/*******************************************************/
// control orders
/*******************************************************/
static int _send_order (int kind,ft_thread_t thread)
{
   item_cell_t res;
   order_t order;
   ft_scheduler_t sched;
   
   _VERIFY_THREAD_LINKED (thread); 
   sched = _get_thread_scheduler (thread);
   _VERIFY_SCHEDULER (sched);

   if (NULL == (order = _make_order (kind,thread))) return EBADMEM;
   
   _lock_thread (sched->self);
   res = _append_item_list (sched->orders,order);
   _unlock_thread (sched->self);
   
   _awake_scheduler (sched); // awake scheduler which is possibly sleeping
   return res != NULL ? OK : EBADMEM;
}

/*******************************************************/
int ft_scheduler_stop (ft_thread_t thread)
{
   return _send_order (STOP_ORDER,thread);
}

int ft_scheduler_suspend (ft_thread_t thread)
{
   return _send_order (SUSPEND_ORDER,thread);
}
 
int ft_scheduler_resume (ft_thread_t thread)
{
   return _send_order (RESUME_ORDER,thread);
}

int _register_unlink_order (ft_thread_t thread)
{
   return _send_order (UNLINK_ORDER,thread);
}

// the thread is already linked
int _register_as_runnable (ft_thread_t thread)
{
   return _send_order (LINK_ORDER,thread);
}

/*******************************************************/
// processing the control orders
/*******************************************************/
static int process_order (void *item,void *arg)
{
   order_t order = (order_t)item;
   ft_scheduler_t sched = (ft_scheduler_t)arg;
   ft_thread_t thread = (ft_thread_t)_order_thread (order);
   switch (_order_kind (order)) {
      case LINK_ORDER:
	 _set_thread_status (thread,_CONT);
	 _link_thread_to (thread,sched);
	 _append_item_list (sched->linked,thread);
	 break;
      case UNLINK_ORDER:
	 _set_thread_is_linked (thread,0);
	 break;
      case SUSPEND_ORDER:
	 _set_thread_is_suspended (thread,1);
	 break;
      case RESUME_ORDER:
	 _set_thread_is_suspended (thread,0);
	 break;
      case STOP_ORDER:
	  if (_get_thread_status (thread) != _TERM) {
	    _append_item_list (sched->to_finalize,thread);
	  }
	 _set_thread_status (thread,_TERM);
	 break;
   }
   free (order);
   return REMOVE;
}

static void _process_all_orders (ft_scheduler_t sched)
{
   _apply_on_items (sched->orders,process_order,sched);
}

/*******************************************************/
// reset threads
/*******************************************************/
/* Perform unlinking and remove terminated and unlinked threads To
   unlink a native thread means to give it the token for the last
   time. As an automaton sould always be linked to a scheduler, when
   it is discovered unlinked, it is automatically re-linked. */

static void reset_thread_status (ft_scheduler_t sched,
				 ft_thread_t thread)
{
   if (_get_thread_is_suspended (thread)) return;
   if (_COOP == _get_thread_status (thread))
      _set_thread_status (thread,_CONT);
   if (_CONT == _get_thread_status (thread))
      sched->active++;
}

static int _reset_thread (void* item,void *arg)
{
   ft_thread_t thread = (ft_thread_t)item;
   ft_scheduler_t sched = (ft_scheduler_t)arg;

   if (_get_thread_status (thread) == _TERM) return REMOVE;
   
   if (_get_thread_is_linked (thread) == 0) {
      if (!_is_automaton (thread)) {
	 _give_token_to (thread);
      } else {
	 _set_thread_is_linked (thread,1);
	 _register_as_runnable (thread);
      }
      return REMOVE;
   }
   
   reset_thread_status (sched,thread);
   return KEEP;
}

static void _reset_all_threads (ft_scheduler_t sched)
{
   sched->active = 0;
   _apply_on_items (sched->linked,_reset_thread,sched);
}

/*****************************************************/
/* unmasking: WAIT -> CONT. pre-condition: the cell contains a thread
 with status WAIT */
/*****************************************************/
void _unmask (ft_scheduler_t sched,item_cell_t cell)
{
   ft_thread_t thread = (ft_thread_t)_content_of_cell (cell);
   if (direct_access)
      _reinsert_cell (sched->linked,cell,sched->determinist);
   if (_get_thread_is_suspended (thread)) return;
   _set_thread_status (thread,_CONT);
   sched->active++;
}

/*****************************************************/
// instants
/*****************************************************/
static void _start_instant (ft_scheduler_t sched)
{
   _reset_scheduler          (sched);
   
   // protected access to the scheduler
   _lock_thread              (sched->self);
   _process_all_orders       (sched);
   _transfer_all_events      (sched);
   _unlock_thread            (sched->self);
   // release access to the scheduler
   
   _reset_all_threads        (sched);
   _finalize_stopped_threads (sched);
}

static void _make_decision (ft_scheduler_t sched)
{
   if (sched->move) sched->move = 0; else sched->eoi = 1;
}


static void _instant (ft_scheduler_t sched)
{
   if (trace_instants) _trace_instant (sched);
   _start_instant (sched);
   while (1) {
      _fire_all_threads (sched);
      _make_decision (sched);
      if (_stable (sched)) return;
   }
}

/*****************************************************/
// scheduler start
/*****************************************************/
static void* _scheduler_behavior (void *arg)
{
   ft_scheduler_t sched = (ft_scheduler_t)arg;
   while (1) {
      _sleep_if_nothing_to_do (sched);
      _instant (sched);
      sched_yield (); // let others schedulers execute, if any
   } 
   return NULL;
}

int ft_scheduler_start (ft_scheduler_t sched)
{
   int res;
   pthread_t pth;
   _VERIFY_SCHEDULER (sched);
   res = pthread_create (&pth,NULL,_scheduler_behavior,sched);
   //if (res == 0) fprintf (stderr,"sched %d started\n",(int)sched->self);
   return res;
}

/*****************************************************/
// scheduler react
/*****************************************************/
int ft_scheduler_react (ft_scheduler_t sched,int k)
{
   switch (k) {
      case INIT_INSTANT: // open the instant
         if (trace_instants) _trace_instant (sched);
         _start_instant (sched);
	 return 0;
      	 
      case PROCEED_PHASE: // run the threads upto fixpoint
	 while (1) {
	    _lock_thread (sched->self);
	    _transfer_all_events (sched);
	    _unlock_thread (sched->self);
	    sched->move = 0;
	    _fire_all_threads (sched);
	    if (sched->move == 0) return 0;
	 }
      
      case NEED_NEW_PHASE: // is there any new event to broadcast ?
	 // no protection needed: all areas have reach their fixpoint
	 return _length_of_item_list (sched->to_broadcast);
      
      case CLOSE_INSTANT: // close the instant
	 sched->eoi = 1;
         if (!_stable (sched)) _fire_all_threads (sched);
   }
   return 0;
}

/*****************************************************/
// scheduler instant
/*****************************************************/
void ft_scheduler_instant (ft_scheduler_t sched)
{
   _instant (sched);
}

