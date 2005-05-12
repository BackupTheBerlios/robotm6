#include "strategyAttack.h"
#include "gridAttack.h"
#include "robotTimer.h"
#include "log.h"
#include "robotPosition.h"
#include "move.h"
#include "movementManager.h"
#include "events.h"
#include "geometry2D.h"
#include "bumperMapping.h"

typedef GridAttack::GridUnit GridUnit;

MoveGain attackExploreGain = 2;
MotorSpeed attackExploreMaxSpeed = 30;

// ----------------------------------------------------------------------------
// returns true, if either the movement finished (PWM or reached target), or
// if both bumpers (either front or rear) are activated.
// ----------------------------------------------------------------------------
static bool evtEndMoveBorder(bool evt[]) {
    return evtEndMove(evt)
	|| (evt[EVENTS_BUMPER_FL] &&
	    evt[EVENTS_BUMPER_FR])
	|| (evt[EVENTS_BUMPER_RL] &&
	    evt[EVENTS_BUMPER_RR]);
}

// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre la fin d'un 
// mouvement en testant en plus si il y a un fosse devant
// ----------------------------------------------------------------------------
static bool evtEndMoveBridge(bool evt[])
{
    return evtEndMove(evt)
        || evt[EVENTS_GROUP_BRIDGE];
}

/**
 * end of movement, or side-sharps.
 */
static bool evtEndMoveSharps(bool evt[])
{
    return evtEndMove(evt)
	|| evt[EVENTS_ENV_SIDE_LEFT]
	|| evt[EVENTS_ENV_SIDE_RIGHT];
}

// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre la fin d'un 
// mouvement en testant en plus les sharps a l'avant du robot
// ----------------------------------------------------------------------------
/*
static bool evtEndMoveEnvDetector(bool evt[])
{
    return evtEndMove(evt)
        || evt[EVENTS_ENV_TOP_RIGHT]
        || evt[EVENTS_ENV_TOP_LEFT];
}
*/

// ----------------------------------------------------------------------------
// First part of alignBorder. selects closest border, and starts movement
// to border.
// ----------------------------------------------------------------------------
BorderEnum StrategyAttackCL::startAlign()
{
    // get position and direction
    Point pos = RobotPos->pt();
    Radian angle = RobotPos->thetaAbsolute();

    BorderEnum targetBorder;
    
    // find closest border
    // are we facing borderX3644 or borderYs
    if (isZeroAngle(angle, M_PI_4)) {
	MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
	targetBorder = BORDER_X3644;
    } else if (isZeroAngle(angle + M_PI, M_PI_4)) {
	MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
	targetBorder = BORDER_X3644;
    } else {
	// facing borderYs
	if (pos.y > 1050) {
	    targetBorder = BORDER_Y2100;
	    if (isZeroAngle(angle - M_PI_2, M_PI_4))
		MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
	    else
		MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
	} else {
	    targetBorder = BORDER_Y0;
	    if (isZeroAngle(angle + M_PI_2, M_PI_4))
		MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
	    else
		MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
	}
    }

    // could be anything, but char* suggested a square's length
    const Millimeter OUTSIDE_DELTA = TERRAIN_CASE_LARGEUR;
    
    Point targetPoint;
    switch (targetBorder) {
    case BORDER_X3644:
	targetPoint = Point(3644 + OUTSIDE_DELTA, pos.y);
	break;
    case BORDER_Y0:
	targetPoint = Point(pos.x, -OUTSIDE_DELTA);
	break;
    case BORDER_Y2100:
	targetPoint = Point(pos.x, 2100 + OUTSIDE_DELTA);
	break;
    }

    Move->go2Target(targetPoint);
    
    return targetBorder;
}

// ----------------------------------------------------------------------------
// returns true, if either both bumper at the front, or both bumper behind are
// activated.
// ----------------------------------------------------------------------------
bool StrategyAttackCL::bumpedBorder() {
    return (Events->isInWaitResult(EVENTS_BUMPER_FL) &&
	    Events->isInWaitResult(EVENTS_BUMPER_FR))
	|| (Events->isInWaitResult(EVENTS_BUMPER_RL) &&
	    Events->isInWaitResult(EVENTS_BUMPER_RR));
}

// ----------------------------------------------------------------------------
// finishes the borderAlign: if we bumped a border, the position and angle are
// updated.
// ----------------------------------------------------------------------------
bool StrategyAttackCL::finishAlign(BorderEnum targetBorder) {
    const Millimeter MAX_DISTANCE_DELTA = TERRAIN_CASE_LARGEUR;

    // TODO: get real values from somewhere:
    const Millimeter FRONT_TO_CENTER = 70;
    const Millimeter BACK_TO_CENTER = 70;
    
    Point reachedPos = RobotPos->pt();
    Radian reachedAngle = RobotPos->thetaAbsolute();
    if (Events->isInWaitResult(EVENTS_MOVE_END)) {
	// usually good. really bad here, as we reached the point
	// outside of the terrain...
	LOG_ERROR("reached target-point outside of terrain (%.2lf, %.2lf).\n",
		  reachedPos.x,
		  reachedPos.y);
	return false;
    } else if (bumpedBorder()) {
	// we bumped
	if (targetBorder == BORDER_X3644 &&
	    reachedPos.x > 3644 - MAX_DISTANCE_DELTA)
	{
	    if (isZeroAngle(reachedAngle, M_PI_4)) {
		RobotPos->set(3644 - FRONT_TO_CENTER, reachedPos.y, 0);
	    } else if (isZeroAngle(reachedAngle + M_PI, M_PI_4)) {
		RobotPos->set(3644 - BACK_TO_CENTER, reachedPos.y, M_PI);
	    } else {
		LOG_ERROR("Angle's just too bad for adjustement: (%.2lf, %.2lf) %.2lf\n",
			  reachedPos.x, reachedPos.y, reachedAngle);
		return false;
	    }
	} else if (targetBorder == BORDER_Y0 &&
		   reachedPos.y < MAX_DISTANCE_DELTA)
	{
	    if (isZeroAngle(reachedAngle + M_PI_2, M_PI_4)) {
		RobotPos->set(reachedPos.x, BACK_TO_CENTER, M_PI_2);
	    } else if (isZeroAngle(reachedAngle - M_PI_2, M_PI_4)) {
		RobotPos->set(reachedPos.x, FRONT_TO_CENTER, -M_PI_2);
	    } else {
		LOG_ERROR("Angle's just too bad for adjustement: (%.2lf, %.2lf) %.2lf\n",
			  reachedPos.x, reachedPos.y, reachedAngle);
		return false;
	    }
	} else if (targetBorder == BORDER_Y2100 &&
		   reachedPos.y > 2100 - MAX_DISTANCE_DELTA)
	{
	    if (isZeroAngle(reachedAngle + M_PI_2, M_PI_4)) {
		RobotPos->set(reachedPos.x, 2100 - FRONT_TO_CENTER, M_PI_2);
	    } else if (isZeroAngle(reachedAngle - M_PI_2, M_PI_4)) {
		RobotPos->set(reachedPos.x, 2100 - BACK_TO_CENTER, -M_PI_2);
	    } else {
		LOG_ERROR("Angle's just too bad for adjustement: (%.2lf, %.2lf) %.2lf\n",
			  reachedPos.x, reachedPos.y, reachedAngle);
		return false;
	    }
	}
	LOG_INFO("Adjusted position to (%.2lf, %.2lf) and theta to %.2lf\n",
		 RobotPos->x(), RobotPos->y(), RobotPos->theta());
	return true;
    } else if (Events->isInWaitResult(EVENTS_PWM_ALERT_LEFT) ||
	       Events->isInWaitResult(EVENTS_PWM_ALERT_RIGHT)) {
	LOG_WARNING("didn't reach border due to PWM-alert (most likely collision).\n");
	return false;
    }
    return false; // should never come here...
}

// ----------------------------------------------------------------------------
// selects the closest border (without turning), and realigns the robot to it
// (moves to border and updates the position, when it detects the border).
// Returns false, if we couldn't detect the border (obstacle...).
// ----------------------------------------------------------------------------
bool StrategyAttackCL::alignBorder()
{
    // TODO: activate events
    BorderEnum targetBorder = startAlign();
    Events->wait(evtEndMoveBorder);
    return finishAlign(targetBorder);
}

static Millisecond avoidObstacleTimeout=0;
bool evtEndMoveAvoidObstacle(bool evt[])
{
  return evtEndMoveNoCollision(evt) 
    || (Timer->time() > avoidObstacleTimeout);
}

bool StrategyAttackCL::getPwmObstaclePos(PwmAlertObstacle &pwmObstacle) 
{
  MotorDirection dirMotor = MvtMgr->getMotorDirection();
  if (Events->isInWaitResult(EVENTS_PWM_ALERT_LEFT)) {
    LOG_WARNING("Left wheel is blocked\n");
    pwmObstacle.leftWheelBlocked = true;
    pwmObstacle.colliDetected=true;
    pwmObstacle.forward = (dirMotor == MOTOR_DIRECTION_RIGHT || dirMotor == MOTOR_DIRECTION_FORWARD);
    pwmObstacle.ptObstacle=getAttackWheelPos(WHEEL_LEFT, pwmObstacle.forward);
  }
  if (Events->isInWaitResult(EVENTS_PWM_ALERT_RIGHT)) {
    LOG_WARNING("Right wheel is blocked\n");
    pwmObstacle.rightWheelBlocked = true;
    pwmObstacle.colliDetected=true;
    pwmObstacle.forward = (dirMotor == MOTOR_DIRECTION_LEFT || dirMotor == MOTOR_DIRECTION_FORWARD);
    pwmObstacle.ptObstacle=getAttackWheelPos(WHEEL_RIGHT, pwmObstacle.forward);
  } 
  return pwmObstacle.colliDetected;
}

bool StrategyAttackCL::goOverSupport()
{
  LOG_COMMAND("goOverSupport\n");
  PwmAlertObstacle pwmObstacle;
  getPwmObstaclePos(pwmObstacle);
  LOG_INFO("pwmObstacle.ptObstacle=%s\n", pwmObstacle.ptObstacle.txt());
  if (pwmObstacle.colliDetected) {
    pwmObstacle.colliDetected = calcSupportCenterCollision(pwmObstacle.ptObstacle, pwmObstacle.ptObstacle);
    LOG_INFO("pwmObstacle.ptObstacle: SUPPORT=%s\n", pwmObstacle.ptObstacle.txt());
  
  }
  grid_->setObstacleTime(Timer->time(), pwmObstacle.ptObstacle);
  
  if (pwmObstacle.colliDetected) {
    LOG_WARNING("Skittle support detected %s!\n",  pwmObstacle.ptObstacle.txt());
    Log->support(pwmObstacle.ptObstacle, pwmObstacle.ptObstacle);
    if (pwmObstacle.forward) {
      Move->setSpeedOnDist(150, 
			   pwmObstacle.leftWheelBlocked?0:30,
			   pwmObstacle.rightWheelBlocked?0:30);
    } else {
      Move->setSpeedOnDist(150, 
			   pwmObstacle.leftWheelBlocked?0:-30,
			   pwmObstacle.rightWheelBlocked?0:-30);
    }
    Events->wait(evtEndMove);
  }
  if (checkEndEvents()) return false;
  if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
  return avoidObstacle();
}

bool StrategyAttackCL::avoidObstacle()
{
  LOG_COMMAND("avoidObstacle\n");
  Millimeter collisionEscapeDist=200;
  Radian     collisionEscapeDir =M_PI_4;
  avoidObstacleTimeout=Timer->time()+8000;
  MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);  
  MotorDirection dirMotor = MvtMgr->getMotorDirection();
  bool leftWheelBlocked=false;
  bool rightWheelBlocked=false;
  if (Events->isInWaitResult(EVENTS_PWM_ALERT_LEFT)) {
    LOG_WARNING("Left wheel is blocked\n");
    leftWheelBlocked=true;
  }
  if (Events->isInWaitResult(EVENTS_PWM_ALERT_RIGHT)) {
    LOG_WARNING("Right wheel is blocked\n");
    rightWheelBlocked=true;
  } 

  
  switch(dirMotor) {
  case MOTOR_DIRECTION_STOP:
  case  MOTOR_DIRECTION_FORWARD:
    LOG_WARNING("Avoid Collision, lastDir=forward\n\n");
    Move->backward(collisionEscapeDist);
    break;
  case MOTOR_DIRECTION_BACKWARD:
    LOG_WARNING("Avoid Collision, lastDir=backward\n\n");
    Move->forward(collisionEscapeDist);
    break;
  case MOTOR_DIRECTION_LEFT:
    LOG_WARNING("Avoid Collision, lastDir=rotate left\n\n");
    Move->rotateFromAngle(-collisionEscapeDir);
    break;
  case MOTOR_DIRECTION_RIGHT:
    LOG_WARNING("Avoid Collision, lastDir=rotate right\n\n");
    Move->rotateFromAngle(collisionEscapeDir);
    break;
  }
  Events->wait(evtEndMoveAvoidObstacle);
  if (checkEndEvents()) return false;
  if (Events->isInWaitResult(EVENTS_MOVE_END)) {
    avoidObstacleTimeout=Timer->time()+8000;
    switch(dirMotor) {
    case MOTOR_DIRECTION_LEFT:
      if (leftWheelBlocked) Move->forward(collisionEscapeDist);
      else if (rightWheelBlocked) Move->backward(collisionEscapeDist);
      break;
    case MOTOR_DIRECTION_RIGHT:
      if (leftWheelBlocked) Move->backward(collisionEscapeDist);
      else if (rightWheelBlocked) Move->forward(collisionEscapeDist);
      break;
    default:
      return false;
    }
  } 
  Events->wait(evtEndMove);
  if (checkEndEvents()) return false;
  return false;
}

// --------------------------------------------------------------------------
// Choisit une des 2 trajectoires predefinie et l'execute jusqu'à collision
// --------------------------------------------------------------------------
bool StrategyAttackCL::preDefinedSkittleExploration()
{
    bool result = false;
    if (RobotPos->y() > ATTACK_EXPLORE_METHOD_Y) {
        result = preDefinedSkittleExploration1();
    } else {
        result = preDefinedSkittleExploration2();
    }
    if (result) {
        LOG_OK("God Bless Odometry!\n");
        result = killCenterSkittles();
    }
    return result;
}

// ---------------------------------------------------------------
// strategie exploration en passant par la rangee en y = 1650
// ---------------------------------------------------------------
bool StrategyAttackCL::preDefinedSkittleExploration1()
{
    LOG_COMMAND("preDefinedSkittleExploration1\n");
    unsigned int retry=0;

    // va au bout premiere rangee 
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->go2Target(Point(2640, 1650), attackExploreGain, attackExploreMaxSpeed);
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
    }

    retry = 0;
    do {
      MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
      Move->go2Target(Point(3194, 1650), attackExploreGain, attackExploreMaxSpeed);
      Events->wait(evtEndMove);
      if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        goOverSupport(); 
	if (checkEndEvents()) return false;
      } else break;
    } while(retry++<ATTACK_EXPLORE_RETRY);

    //alignBorder();
    
     
    // va contre le robot adverse
    retry = 0;
    do {
      MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
      Move->go2Target(Point(3190, 800), attackExploreGain, attackExploreMaxSpeed);
      Events->wait(evtEndMove);
      if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
	if (RobotPos->y()>1000) break;
        avoidObstacle();
        if (checkEndEvents()) return false;
	if (RobotPos->y()>1000) break;
      } else break;
    } while(retry++<ATTACK_EXPLORE_RETRY);

    // recule d'une case
    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    MvtMgr->resetPatinageDetection();
    Move->go2Target(Point(3190, 1000), attackExploreGain, attackExploreMaxSpeed);
    MvtMgr->resetPatinageDetection();
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        if (RobotPos->y() > 1000) {
	    avoidObstacle();
	    if (checkEndEvents()) return false;
	}
    }

    // va vers le debut de la 3e rangee 
    retry=0;
    do {
      if (RobotPos->isTargetForward(Point(2894, 1000))) {
	MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
      } else {
	MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
      }
      Move->go2Target(Point(2894, 1000), attackExploreGain, attackExploreMaxSpeed);
      Events->wait(evtEndMove);
      if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        if (retry==0) goOverSupport();
	else avoidObstacle();
        if (checkEndEvents()) return false;
      } else break;
    } while(retry++<ATTACK_EXPLORE_RETRY);

    // va vers le pont du milieu
    if (!killCenterSkittles()) {
      if (checkEndEvents()) return false;
    } 

    // va vers le debut de la 2e rangee 
    retry=0;
    do {
      if (RobotPos->isTargetForward(Point(2594, 1350))) {
	MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
      } else {
	MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
      }
      Move->go2Target(Point(2594, 1370), attackExploreGain, attackExploreMaxSpeed);
      Events->wait(evtEndMove);
      if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        if (retry==0) goOverSupport();
	else avoidObstacle();
        if (checkEndEvents()) return false;
      } else break;
    } while(retry++<ATTACK_EXPLORE_RETRY);

    // va vers le pont du milieu
    if (!killCenterSkittles()) {
      if (checkEndEvents()) return false;
    } 
  
    // va vers le fond de la 2e rangee
    retry=0;
    do {
      MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
      Move->go2Target(Point(3494, 1370), attackExploreGain, attackExploreMaxSpeed);
      Events->wait(evtEndMove);
      if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        if (retry==0) goOverSupport();
	else avoidObstacle();
        if (checkEndEvents()) return false;
      } else break;
    } while(retry < ATTACK_EXPLORE_RETRY);
    // va vers le pont du milieu
    if (!killCenterSkittles()) {
      if (checkEndEvents()) return false;
    } 

    return true;
}

// ---------------------------------------------------------------
// strategie exploration en passant par la rangee en y = 1350
// ---------------------------------------------------------------
bool StrategyAttackCL::preDefinedSkittleExploration2()
{
    LOG_COMMAND("preDefinedSkittleExploration2\n");
    unsigned int retry=0;
    // va au debut de la 2e rangee
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->go2Target(Point(2640, 1350), attackExploreGain, attackExploreMaxSpeed);  
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        goOverSupport();
    }

    // va vers le fond de la 2e rangee
    retry=0;
    do {
      MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
      Move->go2Target(Point(3194, 1350), attackExploreGain, attackExploreMaxSpeed); 
      Events->wait(evtEndMove);
      if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        goOverSupport();
      }
    } while(retry++<ATTACK_EXPLORE_RETRY);
   
    //alignBorder();
    
    // on recule un petit peu sur la premiere rangee 
    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(Point(3194, 1650), attackExploreGain, attackExploreMaxSpeed);
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        avoidObstacle();
    }

    //alignBorder();

    // on va droit sur l'adversaire
    retry=0;
    do {
      MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
      Move->go2Target(Point(3194, 800), attackExploreGain, attackExploreMaxSpeed);
      Events->wait(evtEndMove);
      if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
	  // on n'a pas reussi
	  // c'est la fin du match?
	  if (checkEndEvents()) return false;
	  if (RobotPos->y() < 1000) break;
	  avoidObstacle();
	  if (checkEndEvents()) return false;
      }  else break;
    } while(retry++<ATTACK_EXPLORE_RETRY);

    // on recule un peu
    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    MvtMgr->resetPatinageDetection();
    Move->go2Target(Point(3190, 1000), attackExploreGain, attackExploreMaxSpeed);
    MvtMgr->resetPatinageDetection();
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
	if (RobotPos->y() > 1000) {
	    avoidObstacle();
	    if (checkEndEvents()) return false;
	}
    }
    
    // va vers le debut de la 3e rangee 
    retry=0;
    do {
      MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
      Move->go2Target(Point(2894, 1000), attackExploreGain, attackExploreMaxSpeed);
      Events->wait(evtEndMove);
      if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        if (retry==0) goOverSupport();
	else avoidObstacle();
        if (checkEndEvents()) return false;
      } else break;
    } while(retry++<ATTACK_EXPLORE_RETRY);

    // va vers le pont du milieu
    if (!killCenterSkittles()) {
      if (checkEndEvents()) return false;
    }
  
    retry=0;
    do {
      MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
      Move->go2Target(Point(2594, 950), attackExploreGain, attackExploreMaxSpeed);
      Events->wait(evtEndMove);
      if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
	if (retry==0) goOverSupport();
	else avoidObstacle();
	if (checkEndEvents()) return false;
      } else break;
    }while(retry++ < ATTACK_EXPLORE_RETRY);

      // va vers le pont du milieu
    if (!killCenterSkittles()) {
      if (checkEndEvents()) return false;
    }

    //  va vers la 1ere rangee
    retry=0;
    do {
      MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
      Move->go2Target(Point(2594, 1700), attackExploreGain, attackExploreMaxSpeed);
      Events->wait(evtEndMove);
      if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        if (retry==0) goOverSupport();
	else avoidObstacle();
	if (checkEndEvents()) return false;
      } else break;
    } while(retry++ < ATTACK_EXPLORE_RETRY);

      // va vers le pont du milieu
    if (!killCenterSkittles()) {
      if (checkEndEvents()) return false;
    }

    // va au bout de la premiere rangee
    retry=0;
    do {
      MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
      Move->go2Target(Point(3194, 1700), attackExploreGain, attackExploreMaxSpeed);
      Events->wait(evtEndMove);
      if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        if (retry==0) goOverSupport();
	else avoidObstacle();
        if (checkEndEvents()) return false;
      } else break;
    } while(retry++ < ATTACK_EXPLORE_RETRY);

    return true;
}

// --------------------------------------------------------------------------
// fonction a appeler en boucle: elle explore le terrain dans une direction 
// puis quand on la rapelle elle explore le terrain dans une autre direction 
// perpendiculaire a la precedente
// --------------------------------------------------------------------------
bool StrategyAttackCL::basicSkittleExploration()
{
    LOG_COMMAND("basicSkittleExploration\n");
    updateGrid();
    GridPoint gpts[3];
    if (lastExplorationDir_ == ATTACK_EXPLORE_COL) {
        basicSkittleExplorationRow(gpts);
    } else {
        basicSkittleExplorationCol(gpts);
    }
    static char gptsTxt[128];
    strcpy(gptsTxt, "Exploration points");
    for(int i=0;i<3;i++) {
        sprintf(gptsTxt, "%s, gpts[%d]=(%d %d)", gptsTxt, i, gpts[i].x, gpts[i].y); 
    }
    LOG_INFO(gptsTxt);
    return exploreGrid(gpts);
}

// --------------------------------------------------------------------------
// genere 3 points qui permettent d'explorer une ligne (en x)
// gpts[0]=centre de la case la plus proche du robot
// gpts[1]=point ou il faut une rotation de 90degre
// gpts[2]=point cible au bout le la ligne commencant en gpts[1]
// --------------------------------------------------------------------------
void StrategyAttackCL::basicSkittleExplorationRow(GridPoint gpts[3])
{
    lastExplorationDir_ = ATTACK_EXPLORE_ROW;
    Millisecond time = Timer->time();
    gpts[0] = GridAttack::getGPoint(RobotPos->pt());
    
    int score[6];
    int scoreR[6];
    int bestScore=0;
    int bestLine=0;
    assert(grid_);

    for(int i=1; i<6; i++) {
        // TODO s'arreter de compter s'il y a un obstacle entre nous et la rangee
        int deltaScore=abs(i-(int)gpts[0].x)*50;
        score[i]  = grid_->scoreRow(gpts[0].x, (GridUnit)i, true,  time)-deltaScore;
        if (score[i] > bestScore) {
            bestScore = score[i];
            bestLine = i;
        }
        scoreR[i] = grid_->scoreRow(gpts[0].x, (GridUnit)i, false, time)-deltaScore;
        if (scoreR[i] > bestScore) {
            bestScore = scoreR[i];
            bestLine = -i;
        }
        
    }
    static int lastBestLine_=2;
    if (bestLine == 0) {
        // il n'y a pas de bonne ligne...
        // TODO: ne pas prendre la meme que la derniere fois
        if (RobotPos->x() < 2900) {
	  bestLine = 4+(lastBestLine_+1)%3;
        } else {
	  bestLine = -(4+(lastBestLine_+1)%3);
        }
	
    }
    lastBestLine_ = abs(bestLine);

    // point de depart de la colonne choisie
    gpts[1]=GridPoint(gpts[0].x, abs(bestLine));
    if (gpts[1].x <= 0) gpts[1].x = 1;
    if (gpts[1].x >= 4) gpts[1].x = 3;
    if (gpts[1].y <= 0) gpts[1].y = 5-gpts[1].x;
    if (gpts[1].y >= 6) gpts[1].y = 6;
    
    // point d'arrivee de la colonne. bestLine >0 => on va vers 
    // les y croissants, <0: on va vers les y decroissant
    gpts[2]=GridPoint(bestLine>0? 3 : 1, gpts[1].y);
}

// --------------------------------------------------------------------------
// genere 3 points qui permettent d'explorer une colonne (en y)
// gpts[0]=centre de la case la plus proche du robot
// gpts[1]=point ou il faut une rotation de 90degre
// gpts[2]=point cible au bout le la collone commencant en gpts[1]
// --------------------------------------------------------------------------
void StrategyAttackCL::basicSkittleExplorationCol(GridPoint gpts[3])
{
    lastExplorationDir_ = ATTACK_EXPLORE_COL;

    Millisecond time = Timer->time();
    gpts[0] = GridAttack::getGPoint(RobotPos->pt());
    assert(grid_);

    int score[4];
    int scoreR[4];
    int bestScore=0;
    int bestLine=0;
    for(int i=1; i<4; i++) {
        int deltaScore=abs(i-(int)gpts[0].y)*50;
        score[i]  = grid_->scoreCol((GridUnit)i, gpts[0].y, true,  time)-deltaScore;
        if (score[i] > bestScore) {
            bestScore = score[i];
            bestLine = i;
        }
        scoreR[i] = grid_->scoreCol((GridUnit)i, gpts[0].y, false, time)-deltaScore;
        if (scoreR[i] > bestScore) {
            bestScore = scoreR[i];
            bestLine = -i;
        }
    }
    static int lastBestCol_=2;
    if (bestLine == 0) {
        // il n'y a pas de bonne ligne...
        // TODO: ne pas prendre la meme que la derniere fois
        if (RobotPos->x() < 1500) {
	  bestLine = 1+(lastBestCol_)%3;
        } else {
	  bestLine = -(1+(lastBestCol_)%3);
        }
	
    }
    lastBestCol_ = abs(bestLine);

    // point de depart de la colonne choisie
    gpts[1]=GridPoint(abs(bestLine), gpts[0].y);
    if (gpts[1].x <= 0) gpts[1].x = 1;
    if (gpts[1].x >= 4) gpts[1].x = 3;
    if (gpts[1].y <= 0) gpts[1].y = 5-gpts[1].x;
    if (gpts[1].y >= 6) gpts[1].y = 6;
    
    // point d'arrivee de la colonne. bestLine >0 => on va vers 
    // les y croissants, <0: on va vers les y decroissant
    gpts[2]=GridPoint(gpts[1].x, bestLine>0? 6:5-gpts[1].x);
}

// ------------------------------------------------------------------------
// gere tous les deplacement pour suivre la trajectoire definie par les 3 
// points de passage
// ------------------------------------------------------------------------
bool StrategyAttackCL::exploreGrid(GridPoint gpts[3])
{
    bool needMoveBackward=false;
    if (!gotoGtps1(gpts)) 
        return false;
    if (!rotateOnGtps1(gpts)) 
        return false;
    //   if (!backBeforeGpts2(gpts)) 
    //    return false;
    if (!gotoGpts2(gpts, needMoveBackward)) 
        return false;
    //if (needMoveBackward) {
    //    if (!goBackToGpts2(gpts)) 
    //        return false;
    //}
    return true;
}

bool StrategyAttackCL::analyzeSharps(Point& supportCenter) {
    Point pt;
    const Millimeter SHARP_DETECT_DISTANCE = 300.;
    if (Events->isInWaitResult(EVENTS_ENV_SIDE_LEFT)) {
        pt = RobotPos->pt() +
	    SHARP_DETECT_DISTANCE * Point(cos(RobotPos->thetaAbsolute()+M_PI/2),
					  sin(RobotPos->thetaAbsolute()+M_PI/2));
        if (pt.x < 2500) return false; // on ne detecte pas les quilles chez nous!
        Log->skittle(pt);
	return calcSupportCenterCollision(pt, supportCenter);
    } else if (Events->isInWaitResult(EVENTS_ENV_SIDE_RIGHT)) {
        pt = RobotPos->pt() +
	    SHARP_DETECT_DISTANCE * Point(cos(RobotPos->thetaAbsolute()-M_PI/2),
					  sin(RobotPos->thetaAbsolute()-M_PI/2));
        if (pt.x < 2500) return false; // on ne detecte pas les quilles chez nous!
        Log->skittle(pt);
	return calcSupportCenterCollision(pt, supportCenter);
    }
    return true; // never going to reach this line (return in both if-branches).
}

/**
 * goes to the specified point. If however a sharp detects the presence
 * of skittles at the side, it automatically goes to the potential
 * skittles.
 */
bool StrategyAttackCL::go2TargetWatching(Point p) {
    Move->go2Target(p, attackExploreGain, attackExploreMaxSpeed);
    bool skittlesDetected = false;
    while (true) {
	Events->wait(evtEndMoveSharps);
	if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
	// c'est la fin du match?
	if (checkEndEvents()) return false;
	Point supportCenter; // will be potentially updated in analyzeSharps)
	if (Events->isInWaitResult(EVENTS_ENV_SIDE_LEFT) ||
	    Events->isInWaitResult(EVENTS_ENV_SIDE_RIGHT))
	{
	    if (!skittlesDetected && analyzeSharps(supportCenter)) {
		LOG_INFO("Detected skittles at %s.\n", supportCenter.txt());
		Move->go2Target(supportCenter, attackExploreGain, attackExploreMaxSpeed);
		skittlesDetected = true;
	    }
	    // else ignore this event.
	} else {
	    return goOverSupport();
	}
    }
}
    
// ------------------------------------------------------------------------
// On va jusqu'au point gpts[1]
// ------------------------------------------------------------------------
bool StrategyAttackCL::gotoGtps1(GridPoint gpts[3])
{
    LOG_FUNCTION();
    // --- le robot est en gpts[0], il faut aller vers gpts[1] ---
    Point pt1 = GridAttack::getPoint(gpts[1]); // conversion en mm
    // trouver le meilleur sens pour aller en pt1, il faut eviter de
    // faire un demi tour
    Radian dirPt1 = dir(RobotPos->pt(), pt1);
    if (isZeroAngle(RobotPos->thetaAbsolute() - dirPt1, 
                    M_PI/2)) {
        // il faut aller vers l'avant
        MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    } else {
        // il faut aller vers l'arriere
        MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    }

    // decaller le point d'arrivee pour etre sur d'etre au centre du 
    // case car on s'arrete toujours trop tot
    pt1.x += ATTACK_CHANGE_TARGET_POINT_DIST * cos(dirPt1);
    pt1.y += ATTACK_CHANGE_TARGET_POINT_DIST * sin(dirPt1);
    unsigned int retry=0;
    do {
	// TODO: HERE
	//go2TargetWatching(pt1);
	Move->go2Target(pt1, attackExploreGain, attackExploreMaxSpeed);
	Events->wait(evtEndMove);
	// on a reussi ?
	if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
	// c'est la fin du match?
	if (checkEndEvents()) return false;
	goOverSupport();
    } while(retry++ < ATTACK_EXPLORE_RETRY);
    return true;
}

// ---------------------------------------------------------------------
// On tourne autour du point gpts[1] pour s'orienter vers gpts[2]
// ---------------------------------------------------------------------
bool StrategyAttackCL::rotateOnGtps1(GridPoint gpts[3])
{
    LOG_FUNCTION();
    // --- on est en pt1, tourner dans la direction de pt2 ---
    Point pt2 = GridAttack::getPoint(gpts[2]); // conversion en mm
    Radian dirPt2 = dir(RobotPos->pt(), pt2);
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->rotate(dirPt2);
    Events->wait(evtEndMove);
    // on a reussi ?
    if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
    // c'est la fin du match?
    if (checkEndEvents()) return false;
    goOverSupport();
    return true;
}

// ---------------------------------------------------------------------
// On recule un peu pour visiter la case qui est derriere gpts[1]
// ---------------------------------------------------------------------
bool StrategyAttackCL::backBeforeGpts2(GridPoint gpts[3])
{
    LOG_FUNCTION();
    // recule un peu a condition de ne pas risquer de s'envoyer dans le fosse
    // ou de ne pas foncer dans les robots adverses en debut de match
    Point targetPoint=RobotPos->pt();
    bool needMoveBackward = false;
    if (gpts[1].x > gpts[2].x) {
        targetPoint.x += TERRAIN_CASE_LARGEUR;
        needMoveBackward = true;
    } else if (gpts[1].y > gpts[2].y) {
        targetPoint.y += TERRAIN_CASE_LARGEUR;
        needMoveBackward = true;
    } else if (gpts[1].y < gpts[2].y) {
        if (attackPhase_ == ATTACK_GRAND_MENAGE) {
            targetPoint.y -= TERRAIN_CASE_LARGEUR;
            needMoveBackward = true;
        }
    }
    if (needMoveBackward) {
        MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
	// TODO: HERE
	//go2TargetWatching(targetPoint);
        Move->go2Target(targetPoint, attackExploreGain, attackExploreMaxSpeed);
        Events->wait(evtEndMove);
        // on a reussi ?
        if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
        // c'est la fin du match?
        if (checkEndEvents()) return false;
	return goOverSupport();
    } 
    return true;
}


// ---------------------------------------------------------------------
// On va vers gpts[2] et eventuellement on va une case plus loin pour 
// faire tomber les quilles qui sont pres des bords
// ---------------------------------------------------------------------
bool StrategyAttackCL::gotoGpts2(GridPoint gpts[3],
                               bool &needMoveBackward)
{
    LOG_FUNCTION();
    // on regard si on ne peut pas aller une plus loin que le point d'arrivee
    Point targetPoint=GridAttack::getPoint(gpts[2]);
    needMoveBackward = false;
    /*
    if (gpts[1].x < gpts[2].x) {
        targetPoint.x += TERRAIN_CASE_LARGEUR;
        needMoveBackward = true;
    } else if (gpts[1].y < gpts[2].y) {
        targetPoint.y += TERRAIN_CASE_LARGEUR;
        needMoveBackward = true;
    } else if (gpts[1].y > gpts[2].y) {
        if (attackPhase_ == ATTACK_GRAND_MENAGE) {
            targetPoint.y -= TERRAIN_CASE_LARGEUR;
            needMoveBackward = true;
        }
    } 
    */
    if (!needMoveBackward) {
        // si on n'aura pas a reculer a la fin du mouvement
        // decaller le point d'arrivee pour etre sur d'etre au centre du 
        // case car on s'arrete toujours trop tot
        Radian dirPt2 = dir(RobotPos->pt(), targetPoint);
        targetPoint.x += ATTACK_CHANGE_TARGET_POINT_DIST * cos(dirPt2);
        targetPoint.y += ATTACK_CHANGE_TARGET_POINT_DIST * sin(dirPt2);
    }

    unsigned int retry=0;
    do {
      MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
      // TODO: HERE
      //go2TargetWatching(targetPoint);
      Move->go2Target(targetPoint, attackExploreGain, attackExploreMaxSpeed);
      Events->wait(evtEndMove);
      // on a reussi ?
      if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
      // c'est la fin du match?
      if (checkEndEvents()) return false;
      goOverSupport();
    } while(retry++ < ATTACK_EXPLORE_RETRY);
    return true;
}
// ---------------------------------------------------------------------
// On a depasse Gpts2, on recule pour aller sur gpts2
// ---------------------------------------------------------------------
bool StrategyAttackCL::goBackToGpts2(GridPoint gpts[3])
{
    LOG_FUNCTION();
    // on est alle trop loin, il faut reculer
    Point targetPoint = GridAttack::getPoint(gpts[2]);
    Radian dirPt2 = dir(RobotPos->pt(), targetPoint);
    // decaller le point d'arrivee pour etre sur d'etre au centre du 
    // case car on s'arrete toujours trop tot
    targetPoint.x += ATTACK_CHANGE_TARGET_POINT_DIST * cos(dirPt2+M_PI);
    targetPoint.y += ATTACK_CHANGE_TARGET_POINT_DIST * sin(dirPt2+M_PI);
    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    // TODO: HERE
    // go2TargetWatching(targetPoint);
    Move->go2Target(targetPoint, attackExploreGain, attackExploreMaxSpeed);
    Events->wait(evtEndMove);
    // on a reussi ?
    if (Events->isInWaitResult(EVENTS_MOVE_END)) return true;
    // c'est la fin du match?
    if (checkEndEvents()) return false;
    return false;
}

// ------------------------------------------------------------------------
// enlever les zones dangereuses de match si on est passe sur toutes 
// les zones de quilles de depart
// ------------------------------------------------------------------------
void StrategyAttackCL::updateGrid()
{
    if ((attackPhase_ != ATTACK_GRAND_MENAGE) &&
        ((Timer->time() >= ATTACK_TIME_EXPLORE_ALL) ||
         (grid_ && 
          grid_->hasNoUnexploredSkittle()))) {
        setAttackPhase(ATTACK_GRAND_MENAGE);
    }
}

// ------------------------------------------------------------------------
// va faire tomber les quilles du milieu du terrain (la pile de 3) 
// ------------------------------------------------------------------------
bool StrategyAttackCL::killCenterSkittles()
{
  if (skittleMiddleProcessed_) return true; // c'est deja fait
    LOG_COMMAND("killCenterSkittles");
    isProcessingMiddleSkittles_ = true;
    Trajectory t;
    // on va ver le pont du milieu en arrivant par le milieu du terrain
    if (RobotPos->x()>2400) {
        t.push_back(Point(RobotPos->x(), 1050));
    }
    t.push_back(Point(2400, 1150));
    t.push_back(Point(2150, 1260)); // TODO check this limit
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Events->enable(EVENTS_NO_BRIDGE_BUMP_LEFT);
    Events->enable(EVENTS_NO_BRIDGE_BUMP_RIGHT);
    Move->followTrajectory(t, TRAJECTORY_BASIC, attackExploreGain, attackExploreMaxSpeed);
    Events->wait(evtEndMoveBridge);
    // on a reussi ?
    if (Events->isInWaitResult(EVENTS_MOVE_END)) {
        LOG_OK("On a fait tomber les quilles du milieu\n");
        skittleMiddleProcessed_ = true; // on a du faire tomber les quilles
    } else {
        // c'est la fin du match?
        if (checkEndEvents()) goto endMiddleSkittleError;
        // on est tombe dans le trou?
        if ((Events->isInWaitResult(EVENTS_NO_BRIDGE_BUMP_LEFT) ||
             Events->isInWaitResult(EVENTS_NO_BRIDGE_BUMP_RIGHT))
            && RobotPos->x()<2400) {
            Move->stop(); // on ne va pas plus loin!
            LOG_OK("On a fait tomber les quilles du milieu\n");
            skittleMiddleProcessed_ = true; // on a du faire tomber les quilles
            MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
            Move->backward(100);
            Events->wait(evtEndMoveNoCollision);
            if (checkEndEvents()) goto endMiddleSkittleError;
        } 
        if (RobotPos->x() < 2200) {
            LOG_OK("On a fait tomber les quilles du milieu\n");
            skittleMiddleProcessed_ = true; // on a du faire tomber les quilles
        }
        // en cas de probleme, on ne fait rien, la suite de la procedure 
        // consiste a s'eloigner du fosse
    }
    Events->disable(EVENTS_NO_BRIDGE_BUMP_LEFT);
    Events->disable(EVENTS_NO_BRIDGE_BUMP_RIGHT);
    
    // on s'eloigne du fosse
    while(RobotPos->x()<2300) {
        MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
        Move->backward(300);
        Events->wait(evtEndMove);
        // on a reussi ?
        if (Events->isInWaitResult(EVENTS_MOVE_END)) break;
        // c'est la fin du match?
        if (checkEndEvents()) return false;
	return goOverSupport();
    }
    Bumper->disableCaptor(BRIDG_BUMP_LEFT);
    Bumper->disableCaptor(BRIDG_BUMP_RIGHT);
    isProcessingMiddleSkittles_ = false;
    return true;
 endMiddleSkittleError:
    Bumper->disableCaptor(BRIDG_BUMP_LEFT);
    Bumper->disableCaptor(BRIDG_BUMP_RIGHT);
    isProcessingMiddleSkittles_ = false;
    return false;
}
