#include <assert.h>
#include <signal.h>

#include "strategy.h"
#include "robotMain.h"
#include "robotTimer.h"
#include "mthread.h"
// #define LOG_CLASSID CLASS_DEFAULT
#include "log.h" 
#include "robotString.h"
#include "timestamp.h"

#include "events.h"
#include "robotDevices.h"
#include "movementManager.h"

// ============================================================================
// ==============================  class RobotMainCL   ==========================
// ============================================================================
RobotMainCL* RobotMainCL::main_;

namespace {
  // -------------------------------------------------------------------------
  // timerFunctionGameOver
  // -------------------------------------------------------------------------
  // fonction appelee a la fin du match
  // -------------------------------------------------------------------------
  void timerFunctionGameOver(void*       userData, 
			     Millisecond time)
  {
      Events->raise(EVENTS_GAME_OVER);
  }
  
  // -------------------------------------------------------------------------
  // robotMainSIGINT
  // -------------------------------------------------------------------------
  // Function executed when catching signal CTRl+C 
  // -------------------------------------------------------------------------
  static bool sigintProcessed_=false;
  static int  sigInt=0;

  extern "C" void robotMainSIGINT(int sig)
  {
      if (!sigintProcessed_) {
	  sigInt = sig;
	  sigintProcessed_ = true;
	  Events->raise(EVENTS_USER_ABORT);
	  sleep(2);
	  exit(0); // au cas ou ca ne march pas autrement
      }
      exit(0);
  }

  // -------------------------------------------------------------------------
  // robotMainFullUserAbortCB
  // -------------------------------------------------------------------------
  // Callback du Ctrl+C qui tue le robot
  // -------------------------------------------------------------------------
  void robotMainUserAbortCB(void* data, EventsEnum evt)
  {
      RobotMainCL* main = (RobotMainCL*)data;
      if (main) {
          main->robotMainUserAbortCB();
          delete main;
          MTHREAD_KILL_ALL();
      }
      exit(0);
  }

} // end of private namespace

// -------------------------------------------------------------------------
// RobotMainCL::robotMainUserAbortCB
// -------------------------------------------------------------------------
void RobotMainCL::robotMainUserAbortCB()
{
    if (sigInt == SIGSEGV) {
	  LOG_ERROR("Segmentation fault...\n");
      } else {
	  LOG_WARNING("USER ABORT\n");
      }
      Log->closeLogFile();

      devices_->emergencyStop();
      mvtMgr_->reset();
      
      LOG_WARNING("exit\n");;
}
// -------------------------------------------------------------------------
// RobotMainCL::RobotMainCL
// -------------------------------------------------------------------------
RobotMainCL::RobotMainCL() : 
  RobotBase("RobotMain", CLASS_ROBOT_MAIN), 
  log_(NULL), timer_(NULL),
  devices_(NULL), mvtMgr_(NULL), evtMgr_(NULL)
{
    main_     = this;
    log_      = new LogCL();
    timer_    = RobotTimerCL::instance(); 
    evtMgr_   = new EVENTS_MANAGER_DEFAULT();
    devices_  = new RobotDevicesCL();
    devices_->allocDevices();
    mvtMgr_   = new MovementManagerCL(devices_->getMotor(), devices_->getOdometer());

    // Gumstix doesn't support %b, and we don't use it anyway [flo]
    //RString::bPrintfInit(); // enable printf(%b)
    LOG_WARNING("Program was compiled on %s at %s by %s in %s on %s\n", 
                __COMPILE_DATE__, __COMPILE_TIME__, 
		__COMPILE_USER__, __COMPILE_DIR__,
                __COMPILE_HOST__);

    // enregistre la detection du Ctrl+C et du callback correspondant qui 
    // stoppe le programme immediatement
    (void) signal(SIGINT,  robotMainSIGINT);
    (void) signal(SIGTERM, robotMainSIGINT);
    //(void) signal(SIGSEGV, robotMainFullSIGINT);
    
    // enregistre la detection du Ctrl+C et du callback correspondant qui 
    // stoppe le programme immediatement
    Events->registerImmediatCallback(EVENTS_USER_ABORT, this, 
				     ::robotMainUserAbortCB, 
				     "robotMainUserAbortCB");
}

// -------------------------------------------------------------------------
// RobotMainCL::~RobotMainCL
// -------------------------------------------------------------------------
RobotMainCL::~RobotMainCL() 
{
    if (mvtMgr_) { delete mvtMgr_;  mvtMgr_ = NULL; }
    if (devices_){ delete devices_; devices_= NULL; }
    if (evtMgr_) { delete evtMgr_;  evtMgr_ = NULL; }
    if (timer_)  { delete timer_;   timer_  = NULL; }
    if (log_)    { delete log_;     log_    = NULL; }
    main_ = NULL;
    exit(0);
}

// -------------------------------------------------------------------------
// RobotMainCL::menu
// -------------------------------------------------------------------------
void RobotMainCL::menu(int   argc, 
                       char* argv[])
{
    // verifie que tous les composants sont initialises
    if (!checkInitDone()) {
	LOG_ERROR("A module is not initialized\n");
    }
    int strI=0;

    // menu qui permet d'iterer sur les missions
    strI=0;
    MainMenuList::iterator it;
    it =  menu_.begin();
    while(1) {
        StrategyCL* strat = (*it).strategy;
	if (strat && strat->menu((*it).text)) {
	    run(strat, argc, argv);
	    resetAll();
	} else {
	    ++it;
	}
	++strI;
	
	if (it == menu_.end()) {
	    it =  menu_.begin();
	    strI = 0;
	}
    }
}

// -------------------------------------------------------------------------
// RobotMainCL::run
// -------------------------------------------------------------------------
void RobotMainCL::run(StrategyCL* strategy, 
                      int argc, 
                      char* argv[])
{
    checkInitDone();
    if (!strategy) return;
    log_->newStrategy();
    // declenche l'events GAMEOVER a la fin du temps reglementaire
    Timer->registerTimerFunction(timerFunctionGameOver,
                                 "timerFunctionGameOver",
                                 this, 
                                 TIME_MATCH);
    if (argc>0) {
      LOG_WARNING("Starting strategy %s:%s\n", argv[0], strategy->name());
    } else {
      LOG_WARNING("Starting strategy %s\n", strategy->name());
    }
    strategy->run(argc, argv);
    
    LOG_WARNING("Strategy finished\n");
    resetAll();
}

// -------------------------------------------------------------------------
// RobotMainCL::resetAll
// -------------------------------------------------------------------------
void RobotMainCL::resetAll()
{
    if (timer_)    { timer_->reset();    }
    if (evtMgr_)   { evtMgr_->reset();   }
    if (devices_)  { devices_->reset();    }
    if (mvtMgr_)   { mvtMgr_->reset();   }
    if (!checkInitDone()) {
	LOG_ERROR("Tous les composants ne sont pas resetés correctement !\n");
    }
    // enregistre la detection du Ctrl+C et du callback correspondant qui 
    // stoppe le programme immediatement
    Events->registerImmediatCallback(EVENTS_USER_ABORT, this, 
				     ::robotMainUserAbortCB, 
				     "robotMainUserAbortCB");
}

// -------------------------------------------------------------------------
// RobotMainCL::checkInitDone
// -------------------------------------------------------------------------
bool RobotMainCL::checkInitDone()
{
    bool ok=true;
/*
    RobotComponentList::iterator it;
    for (it =  robotComponents_->begin();
         it != robotComponents_->end();
         it++) {
        if ((*it)->isInitialized()) {
            LOG_OK("%s is initialized\n", (*it)->robotBaseName());
        } else {
            ok=false;
            LOG_ERROR("%s is NOT initialized\n", (*it)->robotBaseName());
        }
    }
*/
    return ok;
}

// -------------------------------------------------------------------------
// RobotMainCL::addStrategyToMenu
// -------------------------------------------------------------------------
void RobotMainCL::addStrategyToMenu(StrategyCL* strate)
{
    MainMenuItem m;
    m.strategy=strate;
    snprintf(m.text, 35,"%s\nRun       Next", strate->menuName());
    menu_.push_back(m);
}

// -------------------------------------------------------------------------
// RobotMainCL::clearMenu
// -------------------------------------------------------------------------
void RobotMainCL::clearMenu()
{
    menu_.clear();
}

// -------------------------------------------------------------------------
// RobotMainCL::startMatch
// -------------------------------------------------------------------------
void RobotMainCL::startMatch()
{
    LOG_FUNCTION();
    timer_->startMatch();
    log_->startMatch();
}
