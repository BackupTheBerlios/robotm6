#include "robotTimer.h"
#include <time.h>
#include "mthread.h"
#include "log.h"

// ----------------------------------------------------------------------------
// private namespace
// ----------------------------------------------------------------------------

namespace {
  pthread_mutex_t repositoryLock3 = PTHREAD_MUTEX_INITIALIZER;
  extern "C" {
    struct timeval chronometerTic;
  }
  void no_op(...){};
}

void* RobotTimerThreadBody(void*);

// ----------------------------------------------------------------------------
// Static members
// ----------------------------------------------------------------------------
RobotTimerCL* RobotTimerCL::timer_=NULL;

// ----------------------------------------------------------------------------
// RobotTimerCL::RobotTimerCL
// ----------------------------------------------------------------------------
RobotTimerCL::RobotTimerCL() : 
  RobotComponent("Timer", CLASS_ROBOT_TIMER), time_(TIME_NEVER)
{
    LOG_FUNCTION();
    no_op(repositoryLock3);
    tic(&chronometerTic);
    reset();
    setInitialized (MTHREAD_CREATE("Timer Thread",
			   &thread_, 
			   NULL, 
			   RobotTimerThreadBody,
			   NULL)==0);
    if (!isInitialized()) {
	LOG_ERROR("Can not run thread RobotTimer\n");
    } else {
	LOG_OK("Initialisation Done\n");
    }
}

// -------------------------------------------------------------------------
// RobotTimerCL::~RobotTimerCL
// -------------------------------------------------------------------------
RobotTimerCL::~RobotTimerCL()
{
    LOG_WARNING("Destructing RobotTimer\n");
    reset();
    MTHREAD_CANCEL(thread_);
    timer_=NULL;
}

// -------------------------------------------------------------------------
// RobotTimerCL::time
// -------------------------------------------------------------------------
// Retourne l'heure actuelle 
// -------------------------------------------------------------------------
Millisecond RobotTimerCL::time()
{
    if (instance()->time_ >=0 ) {
	instance()->time_ = tac(&chronometerTic);
    }
    return instance()->time_;
}

// -------------------------------------------------------------------------
// RobotTimerCL::startMatch
// -------------------------------------------------------------------------
// Met le compteur a 0 
// -------------------------------------------------------------------------
void RobotTimerCL::startMatch()
{
    tic(&chronometerTic);
    instance()->time_=0;
}

// -------------------------------------------------------------------------
// RobotTimerCL::reset
// -------------------------------------------------------------------------
// unregister toutes les fonctions timer et remet l'heure a 
//  ROBOT_TIMER_RESET_VALUE 
// -------------------------------------------------------------------------
bool RobotTimerCL::reset()
{
    LOG_FUNCTION();
    time_=TIME_NEVER;
    callbacks_.clear();
    periodicTasks_.clear();
    return true;
}

// -------------------------------------------------------------------------
// RobotTimerCL::registerTimerFunction
// -------------------------------------------------------------------------
// Enregistre une fonction qui sera executee quand time()==time 
// -------------------------------------------------------------------------
void RobotTimerCL::registerTimerFunction(TimerFunction fn, 
					 const char* fnName,
					 void* userData,
					 Millisecond time)
{
    assert(time_<0);
    TimerRegisterFunction param;
    param.fn=fn;
    strncpy(param.fnName, fnName, TIMER_FUNCTION_NAME_LENGTH);
    param.userData=userData;
    callbacks_.push_back(std::pair<Millisecond, TimerRegisterFunction>(time, param));
    LOG_INFO("Register Timer Callback %s\n", fnName);
}

// -------------------------------------------------------------------------
// RobotTimerCL::registerPeriodicFunction
// -------------------------------------------------------------------------
// Enregistre une fonction qui est executee regulierement 
// -------------------------------------------------------------------------
void RobotTimerCL::registerPeriodicFunction(TimerFunction fn, 
					    const char* fnName,
					    void* userData)
{
    assert(time_<0);
    TimerRegisterFunction param;
    param.fn=fn;
    strncpy(param.fnName, fnName, TIMER_FUNCTION_NAME_LENGTH);
    param.userData=userData;
    periodicTasks_.push_back(param);
    LOG_INFO("Register Periodic Function %s\n", fnName);
}

// -------------------------------------------------------------------------
// RobotTimerCL::periodicTask
// -------------------------------------------------------------------------
void RobotTimerCL::periodicTask()
{
    time();
    // periodic tasks
    std::deque< TimerRegisterFunction >::iterator it = periodicTasks_.begin();
    for(;it != periodicTasks_.end(); it++) {
	if ((*it).fn) {
	    //execute la fonction
	    (*it).fn((*it).userData, time_);
	}
    }
    // special events
    std::deque<std::pair< Millisecond, TimerRegisterFunction > >::iterator it2 
	= callbacks_.begin();
    for(;it2 != callbacks_.end(); it2++) {
	if ((*it2).first >= 0 && (*it2).first < time_) {
	    if ((*it2).second.fn) {
		// execute le callback
		LOG_DEBUG("Executing: %s time= %2.2lfs\n", 
			  (*it2).second.fnName, (*it2).first/1000.);
		(*it2).second.fn((*it2).second.userData, time_);
	    }
	    // set negative execution time => it won't be run 2 times
	    (*it2).first = -1; 
	}
    }
}

// -------------------------------------------------------------------------
// RobotTimerThreadBody
// -------------------------------------------------------------------------
void* RobotTimerThreadBody(void*)
{
    int ancien_mode=0;
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &ancien_mode);
    while(1) { 
	pthread_testcancel();
	usleep(10000);
	RobotTimerCL::instance()->periodicTask();
    }
    return NULL;
}


// -------------------------------------------------------------------------
// RobotTimerCL::tic
// -------------------------------------------------------------------------
void RobotTimerCL::tic(struct timeval* tv_before )
{
    gettimeofday(tv_before, NULL);
}

// -------------------------------------------------------------------------
// RobotTimerCL::tac
// -------------------------------------------------------------------------
Millisecond RobotTimerCL::tac(struct timeval* tv_before)
{
    struct timeval tv_after;
    Millisecond result;
    gettimeofday(&tv_after, NULL);
    
    result = (tv_after.tv_sec-tv_before->tv_sec)*1000 +
	(tv_after.tv_usec - tv_before->tv_usec)/1000;
    
    return result;
}
