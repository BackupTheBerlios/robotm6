#include <assert.h>
#include <signal.h>

#include "strategy.h"
#include "robotMain.h"
#include "log.h"
#include "robotTimer.h"
#include "mthread.h"
#include "simulatorClient.h"

// ============================================================================
// =============================  class RobotMainFullCL   =======================
// ============================================================================

namespace {
  const VerboseLevel verbose()
  {
      return VERBOSE_INFO;
  }
  const ClassId getClassId() 
  {
      return CLASS_ROBOT_MAIN;
  }
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
  static int sigInt=0;
  extern "C" void robotMainFullSIGINT(int sig)
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
  void robotMainFullUserAbortCB(void* data, EventsEnum evt)
  {
      RobotMainFullCL* main = (RobotMainFullCL*)data;
      if (sigInt == SIGSEGV) {
	  LOG_ERROR("Segmentation fault...\n");
      } else {
	  LOG_WARNING("USER ABORT\n");
      }
      Log->closeLogFile();

      IOMGR->emergencyStop();
      MOVE->userAbort();
      MVTMGR->reset();
      
      LOG_WARNING("exit\n");
      if (main) delete main;
      MTHREAD_KILL_ALL();
      exit(0);
  }

} // end of private namespace



// -------------------------------------------------------------------------
// RobotMainFullCL::RobotMainFullCL
// -------------------------------------------------------------------------
RobotMainFullCL::RobotMainFullCL() : 
    RobotMainCL(),
    ioMgr_(NULL), mvtMgr_(NULL), evtMgr_(NULL)
{ 
    if (RobotConfig->needSimulator()) {
        if (!Simulator->connectToServer()) {
            LOG_ERROR("Cannot connect to SIMULATOR server\n");
        } else {
            if (RobotConfig->isRobotAttack) {
                Simulator->setRobotName("RobotMain Attack");
                Simulator->setRobotModel(ROBOT_MODEL_ATTACK);
                Simulator->setRobotMotorCoef(300, 0.001, 1);
            } else {
                Simulator->setRobotName("RobotMain Defence");
                Simulator->setRobotModel(ROBOT_MODEL_DEFENCE);
                Simulator->setRobotMotorCoef(300, 0.001, 1);
            }
        }
    }
    evtMgr_   = new EVENTS_MANAGER_DEFAULT();
    ioMgr_    = new IoManagerCL();
    mvtMgr_   = new MovementManager(); // simulator doit etre cree avant mvtMgr
    
    // enregistre la detection du Ctrl+C et du callback correspondant qui 
    // stoppe le programme immediatement
    (void) signal(SIGINT,  robotMainFullSIGINT);
    (void) signal(SIGTERM, robotMainFullSIGINT);
    //(void) signal(SIGSEGV, robotMainFullSIGINT);
    
    // enregistre la detection du Ctrl+C et du callback correspondant qui 
    // stoppe le programme immediatement
    Events->registerImmediatCallback(EVENTS_USER_ABORT, this, 
				     robotMainFullUserAbortCB, 
				     "robotMainFullUserAbortCB");
}

// -------------------------------------------------------------------------
// RobotMainFullCL::~RobotMainFullCL
// -------------------------------------------------------------------------
RobotMainFullCL::~RobotMainFullCL()
{
    Timer->reset();
    Events->reset();
    if (mvtMgr_)    { delete mvtMgr_;    mvtMgr_   =NULL;}
    if (ioMgr_)     { delete ioMgr_;     ioMgr_    =NULL;}
    if (evtMgr_)    { delete evtMgr_;    evtMgr_   =NULL;}
}

// -------------------------------------------------------------------------
// RobotMainFullCL::menu
// -------------------------------------------------------------------------
void RobotMainFullCL::menu(int   argc, 
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
#ifdef LSM_TODO
    while(1) {
	if (hli_->menu((*it).text)) {
	    run(((*it).strategy), argc, argv);
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
#endif
}

// -------------------------------------------------------------------------
// RobotMainFullCL::run
// -------------------------------------------------------------------------
void RobotMainFullCL::run(StrategyCL* strategy, 
                          int argc, 
                          char* argv[])
{
    LOG_FUNCTION();
    timer_->registerTimerFunction(timerFunctionGameOver,
				  "timerFunctionGameOver",
				  this,
				  TIME_MATCH);
    LCD->setLed(LCD_LED_RED, LCD_LED_OFF);
    LCD->setLed(LCD_LED_GREEN, LCD_LED_OFF);
    RobotMainCL::run(strategy, argc, argv);
}

// -------------------------------------------------------------------------
// RobotMainFullCL::resetAll
// -------------------------------------------------------------------------
void RobotMainFullCL::resetAll() 
{
    if (timer_)    { timer_->reset();    }
    if (evtMgr_)   { evtMgr_->reset();   }
    if (ioMgr_)    { ioMgr_->reset();    }
    if (mvtMgr_)   { mvtMgr_->reset();   }
    if (!checkInitDone()) {
	LOG_ERROR("Tous les composants ne sont pas resetés correctement !\n");
    }

    // enregistre la detection du Ctrl+C et du callback correspondant qui 
    // stoppe le programme immediatement
    Events->registerImmediatCallback(EVENTS_USER_ABORT, this, 
				     robotMainFullUserAbortCB, 
				     "robotMainFullUserAbortCB");
}

// -------------------------------------------------------------------------
// RobotMainFullCL::startMatch
// -------------------------------------------------------------------------
void RobotMainFullCL::startMatch() 
{
    RobotMainCL::startMatch(); 
}
