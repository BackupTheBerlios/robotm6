#include <assert.h>

#define EVENTS_INFO
#include "events.h"
#include "log.h"
#include "mthread.h" // lock

// ----------------------------------------------------------------------------
// namespace
// ----------------------------------------------------------------------------

namespace {
  pthread_mutex_t repositoryLock2 = PTHREAD_MUTEX_INITIALIZER;
  
  pthread_cond_t anyEventChanged = PTHREAD_COND_INITIALIZER;
  pthread_cond_t eventsRaisedCond[EVENTS_NBR];
  pthread_cond_t eventsNotRaisedCond[EVENTS_NBR];

  void no_op(...) {};
}

// ----------------------------------------------------------------------------
// EventsManager::EventsManager
// ----------------------------------------------------------------------------
EventsManagerPassiveCL::EventsManagerPassiveCL() : 
    EventsManagerCL()
{
    LOG_FUNCTION();
    pthread_mutex_init(&repositoryLock2, NULL);

    //  Initialisation du namespace (anonymous)
    no_op(repositoryLock2, anyEventChanged, eventsRaisedCond, 
          eventsNotRaisedCond);

    // initialisation des pthread_cond
    for (int i = 0; i < (int)EVENTS_NBR; ++i) {
         pthread_cond_init(eventsRaisedCond+i, NULL);
         pthread_cond_init(eventsNotRaisedCond+i, NULL);
    }

    // reset les events status
    reset();
    LOG_OK("Initialisation terminée\n");
}

// ----------------------------------------------------------------------------
// EventsManagerPassiveCL::~EventsManagerPassiveCL
// ----------------------------------------------------------------------------
EventsManagerPassiveCL::~EventsManagerPassiveCL()
{

}

// ----------------------------------------------------------------------------
// EventsManager::raise
// ----------------------------------------------------------------------------
void EventsManagerPassiveCL::raise(EventsEnum evt)
{
    Lock localLock(&repositoryLock2);
    if (evtStatus_[(int)evt]) return;
    evtStatus_[(int)evt] = true;
    if (evtEnables_[(int)evt]) {
        LOG_INFO("Raise : %s\n", evtName(evt));
        // Wake single events:
        pthread_cond_broadcast(eventsRaisedCond+(int)evt);
        // Wake pending Fn filters:
        pthread_cond_broadcast(&anyEventChanged);
 
        evtCallbackMustBeRun_[(int)evt] = true;

	if (evtImmediatCallbacks_[(int)evt].function) {
            localLock.earlyUnlock();
	    LOG_DEBUG("Execute %s immediat callback: %s\n", 
		      evtName(evt), evtImmediatCallbacks_[(int)evt].name);
            evtImmediatCallbacks_[(int)evt].function
	      (evtImmediatCallbacks_[(int)evt].userData, evt);
        }
	localLock.earlyUnlock();
	if (EventsInfo[(int)evt].groupEvt != EVENTS_GROUP_NONE
	    && !evtStatus_[EventsInfo[(int)evt].groupEvt]) {
	  raise(EventsInfo[(int)evt].groupEvt);
	}
    }
}

// ----------------------------------------------------------------------------
// EventsManager::unraise
// ----------------------------------------------------------------------------
void EventsManagerPassiveCL::unraise(EventsEnum evt)
{
    Lock localLock(&repositoryLock2);
    if (!evtStatus_[(int)evt]) return;
    evtStatus_[(int)evt] = false;
    if (evtEnables_[(int)evt]) {
        LOG_INFO("Unraise : %s\n", evtName(evt));
        // Wake single events:
        pthread_cond_broadcast(eventsNotRaisedCond+(int)evt);
        // Wake pending Fn filters:
        pthread_cond_broadcast(&anyEventChanged);

        evtNotCallbackMustBeRun_[(int)evt] = true;

        if (evtNotImmediatCallbacks_[(int)evt].function) {
            localLock.earlyUnlock();
	    LOG_DEBUG("Execute %s not immediat callback: %s\n", 
		      evtName(evt), evtNotImmediatCallbacks_[(int)evt].name);
            evtNotImmediatCallbacks_[(int)evt].function
	      (evtNotImmediatCallbacks_[(int)evt].userData, evt);
        }
    }
}

// ----------------------------------------------------------------------------
// EventsManager::wait
// ----------------------------------------------------------------------------
void EventsManagerPassiveCL::wait(EventsEnum evt)
{
    Lock localLock(&repositoryLock2);
    unraiseEventsGroups();
    unraiseEventsBumpers();
    if (!evtEnables_[(int)evt]) return;
    while(!(evtStatus_[(int)evt])) {
        pthread_cond_wait( eventsRaisedCond+(int)evt,
                           &repositoryLock2 );
    }
    memcpy(evtWaitStatus_, evtStatus_, EVENTS_NBR*sizeof(bool));
    runCallbacks();
}

// ----------------------------------------------------------------------------
// EventsManagerPassiveCL::waitNot
// ----------------------------------------------------------------------------
void EventsManagerPassiveCL::waitNot(EventsEnum evt)
{
    Lock localLock(&repositoryLock2);
    unraiseEventsGroups();
    unraiseEventsBumpers();
    if (!evtEnables_[(int)evt]) return;
    while((evtStatus_[(int)evt])) {
        pthread_cond_wait( eventsNotRaisedCond+(int)evt,
                           &repositoryLock2 );
    }
    memcpy(evtWaitStatus_, evtStatus_, EVENTS_NBR*sizeof(bool));
    runCallbacks();
}


// ----------------------------------------------------------------------------
// EventsManagerPassiveCL::wait
// ----------------------------------------------------------------------------
void EventsManagerPassiveCL::wait(EventsFn evtFn)
{
    Lock localLock(&repositoryLock2);
    unraiseEventsGroups();
    unraiseEventsBumpers();
    while (!check(evtFn)) {
        pthread_cond_wait(&anyEventChanged,
                          &repositoryLock2);
    }
    memcpy(evtWaitStatus_, evtStatus_, EVENTS_NBR*sizeof(bool));
    runCallbacks();
}

// ----------------------------------------------------------------------------
// EventsManagerPassiveCL::waitNot
// ----------------------------------------------------------------------------
void EventsManagerPassiveCL::waitNot(EventsFn evtFn)
{
    Lock localLock(&repositoryLock2);
    unraiseEventsGroups();
    unraiseEventsBumpers();
    while (check(evtFn)) {
        pthread_cond_wait(&anyEventChanged,
                          &repositoryLock2);
    }
    memcpy(evtWaitStatus_, evtStatus_, EVENTS_NBR*sizeof(bool));
    runCallbacks();
}

