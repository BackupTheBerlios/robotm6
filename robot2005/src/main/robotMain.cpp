#include <assert.h>

#include "strategy.h"
#include "robotMain.h"
#include "robotTimer.h"
#include "log.h" 
#include "robotString.h"
#include "timestamp.h"

// ============================================================================
// ==============================  class RobotMainCL   ==========================
// ============================================================================
RobotMainCL* RobotMainCL::main_;
// -------------------------------------------------------------------------
// RobotMainCL::RobotMainCL
// -------------------------------------------------------------------------
RobotMainCL::RobotMainCL() : 
  RobotBase("RobotMain", CLASS_ROBOT_MAIN), log_(NULL), timer_(NULL)
{
    main_   = this;
    log_    = new LogCL();
    timer_  = RobotTimerCL::instance(); 
  //  robotComponents_ = RobotComponent::getComponentsList();
    RString::bPrintfInit(); // enable printf(%b)
    LOG_WARNING("Program was compiled on %s at %s by %s in %s on %s\n", 
                __COMPILE_DATE__, __COMPILE_TIME__, 
		__COMPILE_USER__, __COMPILE_DIR__,
                __COMPILE_HOST__);
}

// -------------------------------------------------------------------------
// RobotMainCL::~RobotMainCL
// -------------------------------------------------------------------------
RobotMainCL::~RobotMainCL() 
{
    if (timer_) { delete timer_; timer_ = NULL; }
    if (log_)   { delete log_;   log_   = NULL; }
    main_ = NULL;
    exit(0);
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
    timer_->reset();
    if (!checkInitDone()) {
	LOG_ERROR("Tous les composants ne sont pas resetés correctement !\n");
    }
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
// RobotMainCL::menu
// -------------------------------------------------------------------------
void RobotMainCL::menu(int argc, 
		     char* argv[])
{
    if (!checkInitDone()) {
        LOG_ERROR("A module is not initialized\n");
    }
    MainMenuList::iterator it;
    it =  menu_.begin();
    while(1) {
        printf("%s%s\n> ", KB_PURPLE, (*it).text);
	char txt[32];
	scanf("%s", txt);
	printf("%s",KB_RESTORE);
	if (txt[0] == 'y' || txt[0] == 'Y') {
            run(((*it).strategy), argc, argv);
	    resetAll();
        }
	if (++it == menu_.end()) {
	    it =  menu_.begin();
	}
    }
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
