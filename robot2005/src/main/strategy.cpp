#include <assert.h>

#include "strategy.h"
#include "robotMain.h"
#include "log.h"
#include "robotTimer.h"
#include "odometer.h"
#include "lcd.h"
#include "simulatorClient.h"

// ============================================================================
// ==============================   class StrategyCL   ==========================
// ============================================================================

// -------------------------------------------------------------------------
// StrategyCL::StrategyCL
// -------------------------------------------------------------------------
StrategyCL::StrategyCL(const char* name, 
		   const char* menuName, 
		   ClassId classId, 
		   RobotMainCL* main): 
    RobotBase(name, classId), main_(main),  
    testBumper_(true), testMove_(true)
{
    strncpy(menuName_, menuName, LCD_CHAR_PER_LINE);
}

// -------------------------------------------------------------------------
// StrategyCL::~StrategyCL
// -------------------------------------------------------------------------
StrategyCL::~StrategyCL()
{
}

// -------------------------------------------------------------------------
// StrategyCL::name
// -------------------------------------------------------------------------
const char* StrategyCL::name() const 
{
    return getName();
}

// -------------------------------------------------------------------------
// StrategyCL::menuName
// -------------------------------------------------------------------------
const char* StrategyCL::menuName() const 
{
  return menuName_;
}

// -------------------------------------------------------------------------
// StrategyCL::waitStart
// -------------------------------------------------------------------------
// Procedure de depart
// -------------------------------------------------------------------------
bool StrategyCL::waitStart(InitMode mode) 
{
  switch(mode) {
  case INIT_COMPLETE:
      autoCheck();
  case INIT_FAST:
#ifdef LSM_TODO
      if (!ODOMETER->isSimu()) {
	ODOMETER->setMode(ODOMETER_AUTOMATIC);
	RobotPos->setOdometerType(ODOMETER_UART_AUTOMATIC);
      }
#endif
      waitJackout();
      break;
  case INIT_NONE:
#ifdef LSM_TODO
      if (!ODOMETER->isSimu()) {
	ODOMETER->setMode(ODOMETER_AUTOMATIC);
	RobotPos->setOdometerType(ODOMETER_UART_AUTOMATIC);
      }
#endif
      // on demare tout de suite
    break;
  }
  main_->startMatch();
  return true;
}

// -------------------------------------------------------------------------
// StrategyCL::emergencyStop
// -------------------------------------------------------------------------
// Quand le match est arrete avant la fin par l'arret d'urgence
// si on est arrete a presque 1 minute 30, c'est la fin du match
// -------------------------------------------------------------------------
void StrategyCL::emergencyStop()
{
    Move->stop();
     if (Timer->time()>TIME_MATCH*0.9) {
	gameOver();
    } else {
	LOG_WARNING("EMERGENCY_STOP is pressed\n");
	LCD->print("EMERGENCY STOP");
	Log->stopMatch();
        LCD->setLed(LCD_LED_RED, LCD_LED_BLINK);
        LCD->setLed(LCD_LED_GREEN, LCD_LED_OFF);
      // unregister game over and emergency stop exceptions
	Events->registerCallback(EVENTS_EMERGENCY_STOP, this, NULL, "");
	Events->registerCallback(EVENTS_GAME_OVER, this, NULL, "");
	Events->registerCallback(EVENTS_TIMER_ALERT, this, NULL, "");
	
	Move->emergencyStop();
	MvtMgr->reset();
	IOMGR->emergencyStop();
	
	Log->closeLogFile();
#ifdef LSM_TODO
	SOUND->play(SOUND_ANORMAL_EMERGENCY_STOP, SND_PRIORITY_URGENT);
	sleep(2);
	if (HLI->menu("End of the match\nWinner   Loser")) {
	    SOUND->play(SOUND_GAGNE);
	} else {
	    SOUND->play(SOUND_PERDU);
	}
	SOUND->startMusic();
	HLI->menu("Press a key\nto stop music");
	SOUND->stopMusic();
#endif 
    }
}

// -------------------------------------------------------------------------
// StrategyCL::gameOver
// -------------------------------------------------------------------------
// Au bout d'1m30 on arrete le robot, on donne des chocolats, on demande si
// on a gagne, on joue de la musique
// -------------------------------------------------------------------------
void StrategyCL::gameOver()
{
  Move->stop();
  LOG_WARNING("GAME is OVER\n");
  LCD->print("GAME OVER");
  LCD->ledPolice();

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
  IOMGR->emergencyStop();
  
  Log->closeLogFile();
  
  
#ifdef LSM_TODO
  SOUND->play(SOUND_C_FAIT);
  sleep(2);
  if (HLI->menu("End of the match\nWinner   Loser")) {
      SOUND->play(soundVictory_);
      if (!Events->check(EVENTS_EMERGENCY_STOP)) {
          ODOMETER->setMode(ODOMETER_AUTOMATIC);
          sleep(1);
          RobotPos->set(pos.center.x, pos.center.y, pos.direction);
          happyEnd();
      }
  } else {
      SOUND->play(soundDefeat_);
  }

  SOUND->startMusic();
  HLI->menu("Press a key\nto stop music");
  SOUND->stopMusic();
#endif
}

bool evtHappyEnd(bool evt[])
{
    return evt[EVENTS_MOVE_END] 
        || evt[EVENTS_EMERGENCY_STOP]
        || evt[EVENTS_PWM_ALERT];
}

// -------------------------------------------------------------------------
// StrategyCL::happyEnd
// -------------------------------------------------------------------------
// Apres la fin du match le robot va taper dans les mains de l'equipe
// 2 positions: le coins le plus proche et le milieu du bord correspondant
// -------------------------------------------------------------------------
void StrategyCL::happyEnd()
{
}

// -------------------------------------------------------------------------
// StrategyCL::timerAlert
// -------------------------------------------------------------------------
// Peu avant la fin du match, Le robot va relacher ses dernieres balles
// -------------------------------------------------------------------------
bool StrategyCL::timerAlert()
{
    LOG_WARNING("TIMER_ALERT\n");
    Events->unraise(EVENTS_TIMER_ALERT);
    //SOUND->play(SOUND_TIMER_ALERT, SND_PRIORITY_URGENT);
    sleep(3);
    gameOver();
    return false;
}

// ----------------------------------------------------------------------------
// StrategyCL::checkEvents
// ----------------------------------------------------------------------------
// Execute les commandes en cas d'arret d'urgence, de fin de match
// Retourne false si la strategy est terminee et qu'il faut retourner au menu
// endEvt=true si l'evenement a ete catche
// ----------------------------------------------------------------------------
bool StrategyCL::checkEvents(bool &endEvt)
{
    endEvt = true;
    if (Events->isInWaitResult(EVENTS_EMERGENCY_STOP)
	|| Events->check(EVENTS_EMERGENCY_STOP)) {
        emergencyStop();
        return false;
    } else if (Events->isInWaitResult(EVENTS_GAME_OVER)
	       || Events->check(EVENTS_GAME_OVER)) {
        gameOver();
        return false;
    } else if (Events->isInWaitResult(EVENTS_TIMER_ALERT)
	       || Events->check(EVENTS_TIMER_ALERT)) {
        return timerAlert();
    } else if (Events->isInWaitResult(EVENTS_USER_ABORT)
	       || Events->check(EVENTS_USER_ABORT)) {
        return false;
    }
    endEvt = false;
    return true;
}


// ----------------------------------------------------------------------------
// evtEndNoMove
// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre la fin d'un 
// mouvement
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
// mouvement
// ----------------------------------------------------------------------------
bool evtEndMoveCollision(bool evt[])
{
    return evt[EVENTS_MOVE_END] 
        || evt[EVENTS_EMERGENCY_STOP] 
        || evt[EVENTS_GAME_OVER]
        || evt[EVENTS_TIMER_ALERT]
        || evt[EVENTS_GROUP_BUMPER]
	|| evt[EVENTS_PWM_ALERT];
}
// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre la fin d'un 
// mouvement
// ----------------------------------------------------------------------------
bool evtEndMove(bool evt[])
{
    return evtEndMoveNoCollision(evt)
        || evt[EVENTS_GROUP_BUMPER]
	|| evt[EVENTS_PWM_ALERT];
}

// -------------------------------------------------------------------------
// Stategy::checkCollision
// -------------------------------------------------------------------------
// S'eloigne d'un obstacle qui vient d'etre detecte et ajoute un element à 
// la liste HLI->bompObstacles
// -------------------------------------------------------------------------
bool StrategyCL::checkCollision(bool &collisionEvt)
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
// StrategyCL::setStartingPosition
// --------------------------------------------------------------------------
// Met a jour la position de depart du robot 
// --------------------------------------------------------------------------
void StrategyCL::setStartingPosition()
{
    RobotPos->set(RobotConfig->startingPos.center.x,
                  RobotConfig->startingPos.center.y,
                  RobotConfig->startingPos.direction);
    Simulator->setRobotPosition(RobotPos->pos());
    LOG_WARNING("setStartingPosition: %s\n", RobotPos->txt());
}
