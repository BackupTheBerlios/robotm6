#include <assert.h>

#define EVENTS_INFO
#include "events.h"
#include "log.h"
#include "mthread.h" // lock

#define EVT_ACTIVE_DELAY 10000 // us

// ----------------------------------------------------------------------------
// EventsManagerActive::EventsManagerActive
// ----------------------------------------------------------------------------
EventsManagerActive::EventsManagerActive() : 
    EventsManager()
{
    LOG_FUNCTION();
    // reset les events status
    reset();
    LOG_OK("Initialisation terminée\n");
}

// ----------------------------------------------------------------------------
// EventsManagerActive::~EventsManagerActive
// ----------------------------------------------------------------------------
EventsManagerActive::~EventsManagerActive()
{

}

// ----------------------------------------------------------------------------
// EventsManagerActive::raise
// ----------------------------------------------------------------------------
void EventsManagerActive::raise(Events evt)
{
    if (evtStatus_[(int)evt])   return;
    if (!evtEnables_[(int)evt]) return;

    evtStatus_[(int)evt] = true;
    if (evtEnables_[(int)evt]) {
        evtCallbackMustBeRun_[(int)evt] = true;
        LOG_INFO("Raise : %s\n", evtName(evt));
	if (evtImmediatCallbacks_[(int)evt].function) {
            LOG_DEBUG("Execute %s immediat callback: %s\n", 
		      evtName(evt), evtImmediatCallbacks_[(int)evt].name);
            evtImmediatCallbacks_[(int)evt].function
	      (evtImmediatCallbacks_[(int)evt].userData, evt);
        }
	if (EventsInfo[(int)evt].groupEvt != EVENTS_GROUP_NONE
	    && !evtStatus_[EventsInfo[(int)evt].groupEvt]) {
	  raise(EventsInfo[(int)evt].groupEvt);
	}
    }
}

// ----------------------------------------------------------------------------
// EventsManagerActive::unraise
// ----------------------------------------------------------------------------
void EventsManagerActive::unraise(Events evt)
{
    if (!evtStatus_[(int)evt]) return;
    evtStatus_[(int)evt] = false;
    if (evtEnables_[(int)evt]) {
        evtNotCallbackMustBeRun_[(int)evt] = true;
        LOG_INFO("Unraise : %s\n", evtName(evt));   
        if (evtNotImmediatCallbacks_[(int)evt].function) {
            LOG_DEBUG("Execute %s not immediat callback: %s\n", 
		      evtName(evt), evtNotImmediatCallbacks_[(int)evt].name);
            evtNotImmediatCallbacks_[(int)evt].function
	      (evtNotImmediatCallbacks_[(int)evt].userData, evt);
        }
    }
}

// ----------------------------------------------------------------------------
// EventsManagerActive::wait
// ----------------------------------------------------------------------------
void EventsManagerActive::wait(Events evt)
{
    unraiseEventsGroups();
    unraiseEventsBumpers();
    if (!evtEnables_[(int)evt]) return;
    memcpy(evtWaitStatus_, evtStatus_, EVENTS_NBR*sizeof(bool));
    while(!(evtStatus_[(int)evt])) {
        usleep(EVT_ACTIVE_DELAY);
	memcpy(evtWaitStatus_, evtStatus_, EVENTS_NBR*sizeof(bool));
    }
    runCallbacks();
}

// ----------------------------------------------------------------------------
// EventsManagerActive::waitNot
// ----------------------------------------------------------------------------
void EventsManagerActive::waitNot(Events evt)
{
    unraiseEventsGroups();
    unraiseEventsBumpers();
    if (!evtEnables_[(int)evt]) return;
    memcpy(evtWaitStatus_, evtStatus_, EVENTS_NBR*sizeof(bool));
    while((evtStatus_[(int)evt])) {
        usleep(EVT_ACTIVE_DELAY);
	memcpy(evtWaitStatus_, evtStatus_, EVENTS_NBR*sizeof(bool));
    }
    runCallbacks();
}


// ----------------------------------------------------------------------------
// EventsManagerActive::wait
// ----------------------------------------------------------------------------
void EventsManagerActive::wait(EventsFn evtFn)
{
    unraiseEventsGroups();
    unraiseEventsBumpers();
    memcpy(evtWaitStatus_, evtStatus_, EVENTS_NBR*sizeof(bool));
    while (!check(evtFn)) {
        usleep(EVT_ACTIVE_DELAY);
	memcpy(evtWaitStatus_, evtStatus_, EVENTS_NBR*sizeof(bool));
    }
    runCallbacks();
}

// ----------------------------------------------------------------------------
// EventsManagerActive::waitNot
// ----------------------------------------------------------------------------
void EventsManagerActive::waitNot(EventsFn evtFn)
{
    unraiseEventsGroups();
    unraiseEventsBumpers();
    memcpy(evtWaitStatus_, evtStatus_, EVENTS_NBR*sizeof(bool));
    
    while (check(evtFn)) {
        usleep(EVT_ACTIVE_DELAY);
	memcpy(evtWaitStatus_, evtStatus_, EVENTS_NBR*sizeof(bool));
    }
    runCallbacks();
}

