/**
 * @file   mthread.cpp
 * @author Alexis Nikichine
 * @date   Sat May 17 21:34:16 2003
 * 
 * @brief  
 * 
 */

#define LOG_CLASSID CLASS_MTHREAD

//#define LOG_DEBUG_ON
#include "mthread.h"
#include "log.h"
#include "classConfig.h"
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <map>

namespace {
    static const unsigned int MTHREAD_NAME_LENGTH = 80;
    typedef struct MthreadInfoStruct {
        MThreadId threadId;
        char name[MTHREAD_NAME_LENGTH];
    } MthreadInfoStruct;
    
    typedef std::map<MThreadId, MthreadInfoStruct> ThreadList;
    
    ThreadList* threadlist_=NULL;
    
    // ------------------------------------------------------------------------
    // addThreadToList
    // ------------------------------------------------------------------------
    void addThreadToList(MThreadId   threadId,
                         const char* name)
    {
        if (!threadlist_) threadlist_ = new ThreadList();
        (*threadlist_)[threadId].threadId = threadId;
        strncpy((*threadlist_)[threadId].name, name, MTHREAD_NAME_LENGTH);
        (*threadlist_)[threadId].name[MTHREAD_NAME_LENGTH-1]=0;
    }
    
    /*!
      A struct passed to mthread_wrapper_function.
      Must be allocated with new (as it will be freed by delete)
    */
    struct thread_wrapper_data_t
    {
        void* (*E_start_routine)(void*);
        void* E_arg;
    };

    // -----------------------------------------------------------------------
    // mthread_wrapperFunction
    // -----------------------------------------------------------------------
    // Do some background work to ensure that when a thread ends, it sends a
    // log
    // -----------------------------------------------------------------------
    extern "C" void* mthread_wrapperFunction(void* arg)
    {
        thread_wrapper_data_t* wrapper_args = (thread_wrapper_data_t*)arg;
        void* result;
        
        LOG_DEBUG("mthread_wrapperFunction\n");
	int ancien_mode;
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &ancien_mode);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &ancien_mode);
        result = wrapper_args->E_start_routine(wrapper_args->E_arg);
        LOG_OK("Thread %s terminated\n", (*threadlist_)[pthread_self()].name);
        delete wrapper_args;

        return result;
    }

} // namespace

// ----------------------------------------------------------------------------
// MTHREAD_LIST
// ----------------------------------------------------------------------------
void MTHREAD_LIST()
{
    ThreadList::iterator iter = threadlist_->begin();
    LOG_INFO("Thread list: (%d threads)\n", threadlist_->size());
    for(;iter != threadlist_->end(); iter++) {
        LOG_INFO("  %s %d\n", (*iter).second.name, 
		 (int)((*iter).second.threadId));
    }
}

// ----------------------------------------------------------------------------
// MTHREAD_KILL_ALL
// ----------------------------------------------------------------------------
void MTHREAD_KILL_ALL()
{
    ThreadList::iterator iter = threadlist_->begin();
    LOG_INFO("Thread list to kill: (%d threads)\n", threadlist_->size());
    for(;iter != threadlist_->end(); iter++) {
        MTHREAD_CANCEL((*iter).second.threadId);
	void* value;
	MTHREAD_JOIN((*iter).second.threadId, &value);
    }
    LOG_INFO("All thread are dead\n");
}

// ----------------------------------------------------------------------------
// MTHREAD_CREATE
// ----------------------------------------------------------------------------
int MTHREAD_CREATE( const char* thread_name,
		    MThreadId*  thread,
		    pthread_attr_t* attr,
		    void* (*start_routine)(void* ), 
		    void* arg )
{
    int result=0;
    thread_wrapper_data_t* wrapper_args = new thread_wrapper_data_t;
    wrapper_args->E_start_routine = start_routine;
    wrapper_args->E_arg = arg;
    
    LOG_DEBUG("pthread_create(%s)\n", thread_name);
    
    int VI_prems=1;
    
    do { 
        if (!VI_prems) {
            LOG_ERROR("pthread_create (%s)interrupted\n",thread_name);
        } else {
            VI_prems = 0;
        }
        result = pthread_create( thread,				
                                 attr,					
                                 mthread_wrapperFunction,
                                 wrapper_args ); 
        
    } while (result && errno == EINTR );
    
    if (result) {
        LOG_ERROR("Thread %s not created", thread_name);
    } else {
        LOG_OK("Thread %s (%d) created\n", thread_name, (int)(*thread));
        addThreadToList(*thread, thread_name);
    }
    return result;
}

// ----------------------------------------------------------------------------
// MTHREAD_JOIN
// ----------------------------------------------------------------------------
int MTHREAD_JOIN(MThreadId thread , 
                 void** thread_return )
{
    return pthread_join(thread, thread_return);
}

// ----------------------------------------------------------------------------
// MTHREAD_CANCEL
// ----------------------------------------------------------------------------
int MTHREAD_CANCEL(MThreadId thread )
{
    LOG_WARNING("Try to kill %s (%d)\n", 
		(*threadlist_)[thread].name, (int)thread);
    pthread_cancel(thread);
    void* thread_return;
    return MTHREAD_JOIN(thread, &thread_return);
}

// ----------------------------------------------------------------------------
// MTHREAD_CLEANUP_PUSH
// ----------------------------------------------------------------------------
void MTHREAD_CLEANUP_PUSH(void (*handler)(void *), void *arg)
{
  //pthread_cleanup_push(handler, arg);
}

// ----------------------------------------------------------------------------
// MTHREAD_CLEANUP_POP
// ----------------------------------------------------------------------------
void MTHREAD_CLEANUP_POP(bool run)
{
  //pthread_cleanup_pop(run?1:0);
}

// ----------------------------------------------------------------------------
// MTHREAD_EXIT
// ----------------------------------------------------------------------------
void MTHREAD_EXIT(void* retval)
{
  pthread_self();
  pthread_exit(retval);
}

// ----------------------------------------------------------------------------
// Lock::Lock
// ----------------------------------------------------------------------------
Lock::Lock(pthread_mutex_t* mutex) : 
  RobotBase("Lock", CLASS_LOCK), M_mutex(mutex) {

  LOG_DEBUG("Wait Lock 0x%x\n", (int)M_mutex);
  pthread_mutex_lock(M_mutex);
  LOG_DEBUG("Locking 0x%x\n", (int)M_mutex);
}

// ----------------------------------------------------------------------------
// Lock::~Lock
// ----------------------------------------------------------------------------
Lock::~Lock() {
  if (M_mutex) {
    pthread_mutex_unlock(M_mutex);
    LOG_DEBUG("Unlock 0x%x\n", (int)M_mutex);
  }
}

// ----------------------------------------------------------------------------
// Lock::earlyUnlock
// ----------------------------------------------------------------------------
void Lock::earlyUnlock()
{
  pthread_mutex_unlock(M_mutex);
  LOG_DEBUG("Unlock 0x%x\n", (int)M_mutex);
  M_mutex = NULL;
}
