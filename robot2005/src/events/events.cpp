#include <assert.h>

#define EVENTS_INFO
#include "events.h"
#include "log.h"
#include "mthread.h" // lock

// ----------------------------------------------------------------------------
// namespace
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// static members
// ----------------------------------------------------------------------------
EventsManagerCL* EventsManagerCL::evtMgr_ = NULL;

// ----------------------------------------------------------------------------
// EventsManagerCL::EventsManagerCL
// ----------------------------------------------------------------------------
EventsManagerCL::EventsManagerCL() : 
  RobotComponent("Events", CLASS_EVENTS_MANAGER)
{
    assert(!evtMgr_);
    evtMgr_ = this;
}

// ----------------------------------------------------------------------------
// EventsManagerCL::~EventsManagerCL
// ----------------------------------------------------------------------------
EventsManagerCL::~EventsManagerCL()
{

}

// ----------------------------------------------------------------------------
// EventsManagerCL::validate
// ----------------------------------------------------------------------------
bool EventsManagerCL::validate()
{
    return false;
}

// ----------------------------------------------------------------------------
// EventsManagerCL::evtName
// ----------------------------------------------------------------------------
const char* EventsManagerCL::evtName(EventsEnum evt)
{
    return EventsInfo[(int)evt].name;
}

// ----------------------------------------------------------------------------
// EventsManagerCL::reset
// ----------------------------------------------------------------------------
bool EventsManagerCL::reset()
{
    LOG_FUNCTION();
    for (int i = 0; i < (int)EVENTS_NBR; ++i) {
        assert(EventsInfo[i].evt == i);
        evtStatus_[i]            = false;
        evtEnables_[i]           = true;

        evtCallbacks_[i].function            = NULL;
        evtImmediatCallbacks_[i].function    = NULL;
        evtCallbackMustBeRun_[i]             = false;
	evtNotCallbacks_[i].function         = NULL;
        evtNotImmediatCallbacks_[i].function = NULL;
        evtNotCallbackMustBeRun_[i]          = false;
    }
    init_ = true;
    return true;
}

// ----------------------------------------------------------------------------
// EventsManagerCL::print
// ----------------------------------------------------------------------------
void EventsManagerCL::print(bool displayTrue, bool displayFalse)
{
    for (int i = 0; i < (int)EVENTS_NBR; ++i) {
	if (!(evtStatus_[i] ^ displayTrue)) {
	    LOG_INFO("%s, value=%s, enable=%s, cb=0x%x\n", 
		     evtName((EventsEnum)i), 
		     evtStatus_[i]? "TRUE":"FALSE", 
		     evtEnables_[i]?"TRUE":"FALSE",
		     (int)evtCallbacks_[i].function);
	}
    }
}

// ----------------------------------------------------------------------------
// EventsManagerCL::enable
// ----------------------------------------------------------------------------
void EventsManagerCL::enable(EventsEnum evt)
{
    evtEnables_[(int)evt] = true;
    LOG_INFO("%s, enabled\n", evtName(evt));
}

// ----------------------------------------------------------------------------
// EventsManagerCL::disable
// ----------------------------------------------------------------------------
void EventsManagerCL::disable(EventsEnum evt)
{
    evtEnables_[(int)evt] = false;
    evtStatus_[(int)evt] = false;
    LOG_INFO("%s, disabled\n", evtName(evt));
}

// ----------------------------------------------------------------------------
// EventsManagerCL::registerCallback
// ----------------------------------------------------------------------------
void EventsManagerCL::registerCallback(EventsEnum        evt,
				     void*             userData,
				     EventsFunctionPtr evtCallback,
				     const char*       callBackName)
{
    evtCallbacks_[(int)evt].function = evtCallback;
    evtCallbacks_[(int)evt].userData = userData;
    strncpy(evtCallbacks_[(int)evt].name, callBackName, 32);
    LOG_INFO("Register callback for %s: %s\n", 
	     evtName(evt), callBackName);
}

// ----------------------------------------------------------------------------
// EventsManagerCL::registerImmediatCallback
// ----------------------------------------------------------------------------
void EventsManagerCL::registerImmediatCallback(EventsEnum        evt,
					     void*             userData,
					     EventsFunctionPtr evtCallback,
					     const char*       callBackName)
{
    evtImmediatCallbacks_[(int)evt].function = evtCallback;
    evtImmediatCallbacks_[(int)evt].userData = userData;
    strncpy(evtImmediatCallbacks_[(int)evt].name, callBackName, 32);
    LOG_INFO("Register immediat callback for %s: %s\n", 
	     evtName(evt), callBackName);
}

// ----------------------------------------------------------------------------
// EventsManagerCL::registerNotCallback
// ----------------------------------------------------------------------------
void EventsManagerCL::registerNotCallback(EventsEnum        evt,
					void*             userData,
					EventsFunctionPtr evtCallback,
					const char*       callBackName)
{
    evtNotCallbacks_[(int)evt].function = evtCallback;
    evtNotCallbacks_[(int)evt].userData = userData;
    strncpy(evtNotCallbacks_[(int)evt].name, callBackName, 32);
    LOG_INFO("Register not callback for %s: %s\n",
	     evtName(evt), callBackName);
}

// ----------------------------------------------------------------------------
// EventsManagerCL::registerNotImmediatCallback
// ----------------------------------------------------------------------------
void EventsManagerCL::registerNotImmediatCallback(EventsEnum        evt,
						void*             userData,
						EventsFunctionPtr evtCallback,
						const char*       callBackName)
{
    evtNotImmediatCallbacks_[(int)evt].function = evtCallback;
    evtNotImmediatCallbacks_[(int)evt].userData = userData;
    strncpy(evtNotImmediatCallbacks_[(int)evt].name, callBackName, 32);
    LOG_INFO("Register not immediat callback for %s: %s\n", 
	     evtName(evt), callBackName);
}

// ----------------------------------------------------------------------------
// unraiseEventsBumpers
// ----------------------------------------------------------------------------
void EventsManagerCL::unraiseEventsBumpers()
{
    evtStatus_[(int)EVENTS_BRIDGE_BUMP_LEFT]=false;  // front center
    evtStatus_[(int)EVENTS_BRIDGE_BUMP_RIGHT]=false;  // front right
    
    evtStatus_[(int)EVENTS_BUMPER_FC]=false;  // front center
    evtStatus_[(int)EVENTS_BUMPER_FR]=false;  // front right
    evtStatus_[(int)EVENTS_BUMPER_FL]=false;  // front left
    evtStatus_[(int)EVENTS_BUMPER_SLF]=false; // side left front
    evtStatus_[(int)EVENTS_BUMPER_SLR]=false; // side left rear
    evtStatus_[(int)EVENTS_BUMPER_SRF]=false; // side right front
    evtStatus_[(int)EVENTS_BUMPER_SRR]=false; // side right rear
    evtStatus_[(int)EVENTS_BUMPER_RR]=false; // rear right
    evtStatus_[(int)EVENTS_BUMPER_RL]=false; // rear left
    evtStatus_[(int)EVENTS_BUMPER_RC]=false; // rear center
    evtStatus_[(int)EVENTS_BUMPER_BORDURE_RR]=false; // bordure rear right (z=15mm)
    evtStatus_[(int)EVENTS_BUMPER_BORDURE_RL]=false; // bordure rear left
    evtStatus_[(int)EVENTS_BUMPER_TOP_F]=false; // top front   (z=390mm) => detection barre d'enbut
    evtStatus_[(int)EVENTS_BUMPER_TOP_R]=false;; 
}

// ----------------------------------------------------------------------------
// unraiseEventsGroups
// ----------------------------------------------------------------------------
void EventsManagerCL::unraiseEventsGroups()
{
  evtStatus_[(int)EVENTS_GROUP_NONE]         = false;
  evtStatus_[(int)EVENTS_GROUP_BUMPER]       = false;
  evtStatus_[(int)EVENTS_GROUP_BRIDGE]       = false;
  evtStatus_[(int)EVENTS_GROUP_USER]         = false;
  evtStatus_[(int)EVENTS_GROUP_ENV_DETECTOR] = false;
  evtStatus_[(int)EVENTS_GROUP_MOTOR]        = false;
  evtStatus_[(int)EVENTS_GROUP_MATCH]        = false;

  evtStatus_[(int)EVENTS_PWM_ALERT]           = false;
}

// ----------------------------------------------------------------------------
// EventsManagerCL::check
// ----------------------------------------------------------------------------
bool EventsManagerCL::check(EventsFn evtFn)
{
    assert(evtFn);
    bool tmpArray[(int)EVENTS_NBR];
    for (int i = 0; i < (int)EVENTS_NBR; ++i) {
        tmpArray[i] = (evtStatus_[i] && evtEnables_[i]);
    }
    return evtFn(tmpArray);
}

// ----------------------------------------------------------------------------
// EventsManagerCL::check
// ----------------------------------------------------------------------------
bool EventsManagerCL::check(EventsEnum evt)
{
    return evtStatus_[(int)evt] && evtEnables_[(int)evt];
}

// ----------------------------------------------------------------------------
// EventsManagerCL::isInWaitResult
// ----------------------------------------------------------------------------
bool EventsManagerCL::isInWaitResult(EventsEnum evt)
{
  return evtWaitStatus_[(int)evt] && evtEnables_[(int)evt];
}


// ----------------------------------------------------------------------------
// EventsManagerCL::runCallbacks
// ----------------------------------------------------------------------------
void EventsManagerCL::runCallbacks()
{
  for (int evt = 0; evt < (int)EVENTS_NBR; ++evt) {
    
    if (evtEnables_[evt]) { 
      if (evtCallbackMustBeRun_[evt] 
	  && evtCallbacks_[evt].function) {
	evtCallbacks_[evt].function(evtCallbacks_[evt].userData, 
				    (EventsEnum)evt);
	LOG_DEBUG("Execute %s callback: %s\n", 
		  evtName((EventsEnum)evt), evtCallbacks_[evt].name);
      } else if (evtNotCallbackMustBeRun_[evt] 
	  && evtNotCallbacks_[evt].function) {
	evtNotCallbacks_[evt].function(evtNotCallbacks_[evt].userData, 
				       (EventsEnum)evt);
	LOG_DEBUG("Execute %s not callback: %s\n", 
		  evtName((EventsEnum)evt), evtNotCallbacks_[evt].name);
      }
    }
    evtCallbackMustBeRun_[evt]    = false;
    evtNotCallbackMustBeRun_[evt] = false;
  }
}
