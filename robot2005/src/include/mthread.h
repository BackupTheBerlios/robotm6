/**
 * @file   mthread.h
 * @author Alexis Nikichine
 * @date   Sat May 17 21:32:46 2003
 * 
 * @brief  A set of functions wrapping around pthread functions, that
 *   additionnally provide some logging functionnality
 */

#ifndef __M_THREAD_H__
#define __M_THREAD_H__

#include <pthread.h>
#include "robotBase.h"

typedef pthread_t MThreadId;

/**
 * @define MTHREAD_SELF
 * @brief wrapper around pthread_self
 * The pthread_self() function returns the ID of the calling thread. 
 */
MThreadId MTHREAD_SELF();

/**
 * MTHREAD_CREATE
 * @brief Wrapper arount pthread_create that additionnaly reports the thread 
 * creation to the telemetry system. Additionnaly, it takes a char* argument, 
 * thread_name
 * @param thread_name thread_name is the (char*) name of the newly created 
 * thread
 * @param thread, 
 * @param attr, 
 * @param start_routine 
 * @param arg are the arguments of the same type  as those of  pthread_create.
*/

int MTHREAD_CREATE(const char* thread_name,
		   MThreadId* thread,
		   pthread_attr_t* attr,
		   void* (*start_routine)(void* ), 
		   void* arg );

/**
 * MTHREAD_JOIN
 * @brief Wrapper around pthread_join
 * The pthread_join() function suspends execution of the calling thread until
 * the target thread terminates, unless the target thread has already 
 * terminated. If status is non-NULL, the value passed to pthread_exit() by 
 * the terminated thread is stored in the location pointed to by status. When 
 * a pthread_join() function returns successfully, the target thread has been
 * terminated. The result of multiple simultaneous calls to pthread_join() for
 * the same target thread is undefined. If the thread calling pthread_join() 
 * is canceled, the target thread is not detached. 
 */
int MTHREAD_JOIN(MThreadId thread ,
                 void** thread_return );

/**
 * MTHREAD_EXIT
 * @brief Wrapper around pthread_exit. 
 * @param retval is a (void*) returned by the calling thread
 */
void MTHREAD_EXIT(void* retval);

/**
 * MTHREAD_CANCEL
 * @brief Wrapper around pthread_cancel
 * The pthread_cancel() function requests that thread be canceled. Ca permet 
 * de tuer un thread!
 */
int MTHREAD_CANCEL(MThreadId thread );

/**
 * MTHREAD_CLEANUP_PUSH
 * @brief Wrapper around pthread_cleanup_push
 * The pthread_cleanup_push() function pushes the specified cancellation
 * cleanup handler onto the cancellation cleanup stack of the calling thread.
 * When a thread exits or is cancelled, and its cancellation cleanup stack is 
 * not empty, the cleanup handlers are invoked with the specified argument in 
 * last-in-first-out order from the cancellation cleanup stack. 
 */
void MTHREAD_CLEANUP_PUSH(void (*handler)(void *), 
                          void *arg);

/**
 * MTHREAD_CLEANUP_POP
 * @brief Wrapper around pthread_cleanup_push
 * The pthread_cleanup_pop() function removes the cleanup handler routine at 
 * the top of the cancellation cleanup stack of the calling thread. If run
 * is true, the handler function is executed. 
 */
void MTHREAD_CLEANUP_POP(bool run);

void MTHREAD_KILL_ALL();

/**
 * @class Lock
 * @brief Lock a mutex in the constructor and unlock it in the destructor
 */
class Lock : public RobotBase {
  public:
    Lock(pthread_mutex_t* mutex);
    ~Lock();
    
    void earlyUnlock();
  private:
    Lock(const Lock&);
    Lock& operator=(const Lock&);

    pthread_mutex_t* M_mutex;
};

#endif // __M_THREAD_H__
