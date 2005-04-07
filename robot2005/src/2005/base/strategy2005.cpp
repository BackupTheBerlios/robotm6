#include <assert.h>
#include <stdarg.h>

#include "strategy2005.h"
#include "robotMain.h"
#include "log.h"
#include "events.h"
#include "robotTimer.h"
//#include "odometer.h"
#include "lcd.h"
#include "simulatorClient.h"
#include "robotPosition.h"
#include "move.h"
#include "movementManager.h"
//#include "ioManager.h"


// ============================================================================
// ==============================   class Strategy2005CL   ====================
// ============================================================================

// -------------------------------------------------------------------------
// Strategy2005CL::Strategy2005CL
// -------------------------------------------------------------------------
Strategy2005CL::Strategy2005CL(const char* name, 
                               const char* menuName, 
                               ClassId classId, 
                               RobotMainCL* main): 
    StrategyCL(name, menuName, classId, main), 
    testBumper_(true), testMove_(true),
    soundDefeat_(SOUND_PERDU),
    soundVictory_(SOUND_GAGNE)
{
}

// -------------------------------------------------------------------------
// Strategy2005CL::~Strategy2005CL
// -------------------------------------------------------------------------
Strategy2005CL::~Strategy2005CL()
{
}

// -------------------------------------------------------------------------
// Strategy2005CL::waitStart
// -------------------------------------------------------------------------
// Procedure de depart
// -------------------------------------------------------------------------
bool Strategy2005CL::waitStart(InitMode mode) 
{
    switch(mode) {
    case INIT_COMPLETE:
        autoCheck();
    case INIT_FAST:
        waitJackout();
        break;
    case INIT_NONE:
        // on demare tout de suite
        break;
    }
    main_->startMatch();
    return true;
}

// -------------------------------------------------------------------------
// Strategy2005CL::emergencyStop
// -------------------------------------------------------------------------
// Quand le match est arrete avant la fin par l'arret d'urgence
// si on est arrete a presque 1 minute 30, c'est la fin du match
// -------------------------------------------------------------------------
void Strategy2005CL::emergencyStop()
{
    Move->stop();
    if (Timer->time()>TIME_MATCH*0.9) {
        // fin du match normale
        gameOver();
    } else {
        // on a arrete le robot avant la fin du match
        LOG_WARNING("EMERGENCY_STOP is pressed\n");
        Lcd->print("EMERGENCY STOP");
        Log->stopMatch();

        // unregister game over and emergency stop exceptions
	Events->registerCallback(EVENTS_EMERGENCY_STOP, this, NULL, "");
	Events->registerCallback(EVENTS_GAME_OVER, this, NULL, "");
	Events->registerCallback(EVENTS_TIMER_ALERT, this, NULL, "");
	
	Move->emergencyStop();
	MvtMgr->reset();
	//IOMGR->emergencyStop();
	
	Log->closeLogFile();
	Sound->play(SOUND_ANORMAL_EMERGENCY_STOP, SND_PRIORITY_URGENT);
	sleep(2);
	if (menu("End of the match\nWinner   Loser")) {
	    Sound->play(soundVictory_);
	} else {
	    Sound->play(soundDefeat_);
	}
    }
}

// -------------------------------------------------------------------------
// Strategy2005CL::gameOver
// -------------------------------------------------------------------------
// Au bout d'1m30 on arrete le robot, on donne des chocolats, on demande si
// on a gagne, on joue de la musique
// -------------------------------------------------------------------------
void Strategy2005CL::gameOver()
{
    Move->stop();
    LOG_WARNING("GAME is OVER\n");
    Lcd->print("GAME OVER");

    Log->stopMatch();
    Position pos = RobotPos->pos();

    // unregister game over and emergency stop exceptions
    Events->registerCallback(EVENTS_EMERGENCY_STOP, this, NULL, "");
    Events->registerCallback(EVENTS_GAME_OVER,      this, NULL, "");
    Events->registerCallback(EVENTS_TIMER_ALERT,    this, NULL, "");

    // Action a exectuer en fin de match: on arrete le robot!
    Timer->reset();
    Move->emergencyStop();
    MvtMgr->reset();
    //IOMGR->emergencyStop();

    Log->closeLogFile();

    Sound->play(SOUND_C_FAIT);
    sleep(2);
    if (menu("End of the match\nWinner   Loser")) {
        Sound->play(soundVictory_);
    } else {
        Sound->play(soundDefeat_);
    }
}

// -------------------------------------------------------------------------
// Strategy2005CL::timerAlert
// -------------------------------------------------------------------------
// Action a effectuer avant la fin du match
// Overlaod this function!
// -------------------------------------------------------------------------
bool Strategy2005CL::timerAlert()
{
    LOG_WARNING("TIMER_ALERT\n");
    Events->unraise(EVENTS_TIMER_ALERT);
    Sound->play(SOUND_TIMER_ALERT, SND_PRIORITY_URGENT);

    return true; // overload functions should return false
}

// ----------------------------------------------------------------------------
// Strategy2005CL::checkEndEvents
// ----------------------------------------------------------------------------
// Execute les commandes en cas d'arret d'urgence, de fin de match
// Retourne false si la strategy est terminee et qu'il faut retourner au menu
// endEvt=true si l'evenement a ete catche
// ----------------------------------------------------------------------------
bool Strategy2005CL::checkEndEvents()
{
    if (Events->isInWaitResult(EVENTS_EMERGENCY_STOP)
	|| Events->check(EVENTS_EMERGENCY_STOP)) {
        emergencyStop();
        return true;
    } else if (Events->isInWaitResult(EVENTS_GAME_OVER)
	       || Events->check(EVENTS_GAME_OVER)) {
        gameOver();
        return true;
    } else if (Events->isInWaitResult(EVENTS_TIMER_ALERT)
	       || Events->check(EVENTS_TIMER_ALERT)) {
        return timerAlert();
    } else if (Events->isInWaitResult(EVENTS_USER_ABORT)
	       || Events->check(EVENTS_USER_ABORT)) {
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
// evtEndNoMove
// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre la fin d'un
// mouvement sans detecter les collisions ou le patinage du robot.
// Attends: find du mouvment, arret d'urgence, fin du match, timer_alert
// ----------------------------------------------------------------------------
bool evtEndMoveNoCollision(bool evt[])
{
    return evt[EVENTS_MOVE_END] 
        || evt[EVENTS_EMERGENCY_STOP] 
        || evt[EVENTS_GAME_OVER]
        || evt[EVENTS_TIMER_ALERT];
}

// ----------------------------------------------------------------------------
// evtEndMoveCollision
// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre la fin d'un 
// mouvement en testant en plus les collisions basees sur les bumper
// ----------------------------------------------------------------------------
bool evtEndMoveCollision(bool evt[])
{
    return evtEndMove(evt)
        || evt[EVENTS_GROUP_BUMPER];
}

// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre la fin d'un 
// mouvement en testant en plus les collisions basees sur les bumper
// ----------------------------------------------------------------------------
bool evtEndMove(bool evt[])
{
    return evtEndMoveNoCollision(evt)
	|| evt[EVENTS_PWM_ALERT_LEFT]
	|| evt[EVENTS_PWM_ALERT_RIGHT];
}

// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre la pression d'une 
// touche du lcd
// ----------------------------------------------------------------------------
bool evtDialogButtonFilter(bool evt[])
{
    return evt[EVENTS_BUTTON_YES]
	|| evt[EVENTS_BUTTON_NO];
}

// ---------------------------------------------------------------------------
// evtRebootSwitch
// ---------------------------------------------------------------------------
bool evtRebootSwitch(bool evt[]) 
{
  return (evt[EVENTS_BUTTON_YES] 
	  || evt[EVENTS_BUTTON_NO]
	  || evt[EVENTS_SWITCH_REBOOT]);
}

// -------------------------------------------------------------------------
// Stategy::checkCollision
// -------------------------------------------------------------------------
// S'eloigne d'un obstacle qui vient d'etre detecte et ajoute un element à 
// la liste HLI->bompObstacles
// -------------------------------------------------------------------------
bool Strategy2005CL::checkCollision(bool &collisionEvt)
{
#ifdef LSM_TODO
    if (Timer->time() < 1500) {
	collisionEvt = false;
	return true;
    }
    if (!Events->isInWaitResult(EVENTS_GROUP_BUMPER)
	&& !Events->isInWaitResult(EVENTS_PWM_ALERT)) {
	collisionEvt = false;
        return true;
    }
    MoveDirection oldDir = MvtMgr->getDirection();
    do {
      LOG_COMMAND("Avoid Collision\n");
      collisionEvt = true;

      Obstacle obstacle;

      // evitement de collision
      MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
      if (Events->isInWaitResult(EVENTS_PWM_ALERT)) {
//	  SOUND->play(SOUND_ALERT_MOTOR);
	// motor pwm
	MotorSpeed  speedLeft;
	MotorSpeed  speedRight;
	MvtMgr->getMotorSpeed(speedLeft, speedRight);
	if (speedLeft+speedRight > 0) {
            Point detectionPoint=RobotPos->pt() 
                + ROBOT_RAYON * Point(cos(RobotPos->thetaAbsolute()), 
                                      sin(RobotPos->thetaAbsolute()));
	    obstacle=Obstacle(detectionPoint
                              + 50.*Point(cos(RobotPos->thetaAbsolute()), 
				    sin(RobotPos->thetaAbsolute())),
                              detectionPoint,
                              Timer->time()
                              );
	    Move->backward(100);
	} else {
	    Point detectionPoint=RobotPos->pt() 
                - ROBOT_RAYON * Point(cos(RobotPos->thetaAbsolute()), 
                                      sin(RobotPos->thetaAbsolute()));
	    obstacle=Obstacle(detectionPoint
                              - 50.*Point(cos(RobotPos->thetaAbsolute()), 
                                       sin(RobotPos->thetaAbsolute())),
                              detectionPoint,
                              Timer->time()
                              );
	    Move->forward(100);
	}
#ifdef LSM_TODO
	HLI->setBumpObstacle(obstacle);
#endif
	Events->unraise(EVENTS_PWM_ALERT);
      } else {
#ifdef LSM_TODO
	    // bumper events
	    int bump=0;
	    if (Events->isInWaitResult(EVENTS_BUMPER_FC))  bump = 0;
	    if (Events->isInWaitResult(EVENTS_BUMPER_FR))  bump = 1;
	    if (Events->isInWaitResult(EVENTS_BUMPER_FL))  bump = 2;
	    if (Events->isInWaitResult(EVENTS_BUMPER_SRF)) bump = 3;
	    if (Events->isInWaitResult(EVENTS_BUMPER_SRR)) bump = 4;
	    if (Events->isInWaitResult(EVENTS_BUMPER_SLF)) bump = 5;
	    if (Events->isInWaitResult(EVENTS_BUMPER_SLR)) bump = 6;
	    if (Events->isInWaitResult(EVENTS_BUMPER_RR))  bump = 7;
	    if (Events->isInWaitResult(EVENTS_BUMPER_RL))  bump = 8;
	    if (Events->isInWaitResult(EVENTS_BUMPER_RC))  bump = 9;
	    if (Events->isInWaitResult(EVENTS_BUMPER_BORDURE_RR)) bump = 10;
	    if (Events->isInWaitResult(EVENTS_BUMPER_BORDURE_RL)) bump = 11;
	    if (Events->isInWaitResult(EVENTS_BUMPER_TOP_F)) bump = 12;
	    if (Events->isInWaitResult(EVENTS_BUMPER_TOP_R)) bump = 13;
	    Point detectionPoint=RobotPos->pt() 
		+ ((dridaqButtonMapping_[bump].pos.r)
		   *Point(cos(RobotPos->thetaAbsolute()+
			      dridaqButtonMapping_[bump].pos.theta), 
			  sin(RobotPos->thetaAbsolute()+
			      dridaqButtonMapping_[bump].pos.theta)));
	    obstacle=Obstacle(detectionPoint + (OBSTACLE_RAYON)
			      *Point(cos(RobotPos->thetaAbsolute()+
					 dridaqButtonMapping_[bump].pos.theta), 
				     sin(RobotPos->thetaAbsolute()+
					 dridaqButtonMapping_[bump].pos.theta)),
			      detectionPoint,
			      Timer->time());
	    if (Events->isInWaitResult(EVENTS_BUMPER_FC)
		|| Events->isInWaitResult(EVENTS_BUMPER_FR)
		|| Events->isInWaitResult(EVENTS_BUMPER_FL)
		|| Events->isInWaitResult(EVENTS_BUMPER_SRF)
		|| Events->isInWaitResult(EVENTS_BUMPER_SLF)
		|| Events->isInWaitResult(EVENTS_BUMPER_TOP_F)) {
		Move->backward(100);
	    } else {
		Move->forward(100);
	    }

	HLI->setBumpObstacle(obstacle);
#endif
      }

      Events->wait(evtEndMoveCollision);
      bool tmpEvt=false;
      if (!checkEvents(tmpEvt)) return false;
    } while (Events->isInWaitResult(EVENTS_GROUP_BUMPER)
	     || Events->isInWaitResult(EVENTS_PWM_ALERT));
    MvtMgr->setRobotDirection(oldDir);
#endif
    return true;
}

// --------------------------------------------------------------------------
// Strategy2005CL::setStartingPosition
// --------------------------------------------------------------------------
// Met a jour la position de depart du robot 
// --------------------------------------------------------------------------
void Strategy2005CL::setStartingPosition()
{
    RobotPos->set(RobotConfig->startingPos.center.x,
                  RobotConfig->startingPos.center.y,
                  RobotConfig->startingPos.direction);
    Simulator->setRobotPosition(RobotPos->pos());
    LOG_WARNING("setStartingPosition: %s\n", RobotPos->txt());
}

// --------------------------------------------------------------------------
// Strategy2005CL::menu
// --------------------------------------------------------------------------
// affiche un texte sur le lcd et attends la pression d'une touche. Le bouton 
// yes renvoie true, le bouton NO renvoie false
// --------------------------------------------------------------------------
bool Strategy2005CL::menu(const char*fmt, ...)
{
    static char text[80];
    va_list argp;
    va_start(argp, fmt);
    vsprintf(text, fmt, argp);
    va_end(argp);

    if (strlen(text)>0 && fmt != NULL) {
        Lcd->print(text);
    }
    Events->waitNot(evtDialogButtonFilter);
    Events->wait(evtDialogButtonFilter);
    return (Events->isInWaitResult(EVENTS_BUTTON_YES));
}
