#include "strategyLargeAttack.h"
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

static CollisionEnum checkCollisionEvents()
{
    bool leftPwm = Events->isInWaitResult(EVENTS_PWM_ALERT_LEFT);
    bool rightPwm = Events->isInWaitResult(EVENTS_PWM_ALERT_RIGHT);
    if (leftPwm && rightPwm) return COLLISION_BOTH;
    if (leftPwm) return COLLISION_LEFT;
    if (rightPwm) return COLLISION_RIGHT;
    return COLLISION_NONE;
}

// ---------------------------------------------------------------
// strategie exploration en passant par la rangee en y = 1650
// ---------------------------------------------------------------
bool StrategyLargeAttackCL::preDefinedSkittleExploration1()
{
    return preDefinedSkittleExploration2();
    LOG_COMMAND("preDefinedLargeSkittleExploration1\n");
    Trajectory t;
    t.push_back(Point(2640, 1650)); 
    t.push_back(Point(3240, 1650));
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->followTrajectory(t, TRAJECTORY_BASIC);
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }

    //alignBorder();

    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->rotate(-M_PI_2);
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }
    
    //alignBorder();
    
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->go2Target(Point(3190, 650));
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }

    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(Point(3190, 1050));
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }
    
    t.clear();
    t.push_back(Point(2550, 1050)); 
    t.push_back(Point(2594, 1400)); 
    t.push_back(Point(3340, 1350)); 
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->followTrajectory(t, TRAJECTORY_BASIC);
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }

    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(Point(3190, 1350));
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }

    return true;
}

bool StrategyLargeAttackCL::centerOnSupport(Point supportCenter) {
    LOG_FUNCTION();
    const int XY_SQUARE_SAFETY = 100; // allow some distance
    const Radian THETA_DELTA = M_PI_4/3;
    unsigned int retries = 0;
    // TODO: get MOVE_XY_SQUARE_EPSILON from move.h
    bool centered;
    while (!(centered = (Geometry2D::getSquareDistance(RobotPos->pt(),
						       supportCenter)
			 < (MOVE_XY_SQUARE_EPSILON + XY_SQUARE_SAFETY)))
	   && retries < 2) {
	LOG_INFO("centerOnSupport try %d\n", retries);
	retries++;
	Point currentPos = RobotPos->pt();
	// try to go there directly
	if (RobotPos->isTargetForward(supportCenter))
	    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
	else
	    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
	Move->go2Target(supportCenter);
	Events->wait(evtEndMove);
	if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
	    if (checkEndEvents()) return false; // end of match
	    CollisionEnum collision = checkCollisionEvents();
	    if (collision == COLLISION_LEFT ||
		collision == COLLISION_RIGHT)
	    {
		// turn on wheel
		bool collisionLeft = (collision == COLLISION_LEFT);
		LOG_INFO("collision (%s) while centering on support\n", collisionLeft?"left":"right");
		MotorDirection dir = MvtMgr->getMotorDirection();
		bool forward = (dir == MOTOR_DIRECTION_FORWARD);
		char* tmp;
		switch (dir) {
		case MOTOR_DIRECTION_STOP: tmp = "STOP"; break;
		case MOTOR_DIRECTION_FORWARD: tmp = "FORWARD"; break;
		case MOTOR_DIRECTION_BACKWARD: tmp = "BACKWARD"; break;
		case MOTOR_DIRECTION_LEFT: tmp = "LEFT"; break;
		case MOTOR_DIRECTION_RIGHT: tmp = "RIGHT"; break;
	        default: tmp=""; break;
		}
		LOG_INFO("direction: %s\n", tmp);
		forward = RobotPos->isTargetForward(supportCenter);

		bool stopLeft = collisionLeft;
		Radian deltaTheta = THETA_DELTA*(collisionLeft ^ forward? -1: 1);
		Move->rotateOnWheel(RobotPos->thetaAbsolute() + deltaTheta, stopLeft);
		Events->wait(evtEndMove);
		if (!Events->isInWaitResult(EVENTS_MOVE_END))
		    if (checkEndEvents()) return false; // end of match.
		// just try again (don't treat other events)
	    } else if (collision == COLLISION_BOTH) {
		// just try again
	    } else if (collision == COLLISION_NONE) {
		// no idea what happened -> return false
		return false;
	    }
	}
    }
    LOG_INFO("leaving centerSupport (%s)\n", centered?"success":"failed");
    return centered;
}

bool StrategyLargeAttackCL::rotateOnSupport(Point supportCenter,
					    Radian targetTheta) {
    LOG_FUNCTION();
    Point currentPos = RobotPos->pt();
    const int MAX_ROTATE_DELTA_SQUARE = 500;
    unsigned int retries = 0;
    bool rotated;
    while (!(rotated = (fabs(RobotPos->thetaAbsolute() - targetTheta)
			< MOVE_ROTATION_EPSILON))
	   && retries < 2) {
	retries++;
	if (Geometry2D::getSquareDistance(supportCenter, currentPos)
	    > MAX_ROTATE_DELTA_SQUARE) {
	    LOG_INFO("rotateOnSupport: centering first.\n");
	    // too far away from center
	    if (RobotPos->isTargetForward(supportCenter))
		MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
	    else
		MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
	    Move->go2Target(supportCenter);
	    Events->wait(evtEndMove);
	    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
		if (checkEndEvents()) return false; // end of game
		// don't really know, what else to do here. just try to rotate.
	    }
	}
	// we should be centered now.
	bool stopLeft;
	const Radian COUNTER_THETA_DELTA = M_PI_4/3;
	Move->rotate(targetTheta);
	Events->wait(evtEndMove);
	if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
	    if (checkEndEvents()) return false;
	    CollisionEnum collision = checkCollisionEvents();
	    if (collision == COLLISION_LEFT ||
		collision == COLLISION_RIGHT)
	    {
		// rotate counter-direction
		bool collisionLeft = (collision == COLLISION_LEFT);
		LOG_INFO("collision (%s) in rotateOnSupport\n", collisionLeft?"left":"right");
		MotorDirection dir = MvtMgr->getMotorDirection();
		bool turningLeft = (dir == MOTOR_DIRECTION_LEFT);
		Radian counterTheta = COUNTER_THETA_DELTA
		    * (collisionLeft ^ turningLeft? 1: -1);
		stopLeft = collisionLeft;
		Move->rotateOnWheel(RobotPos->thetaAbsolute() + counterTheta, stopLeft);
		Events->wait(evtEndMove);
		if (!Events->isInWaitResult(EVENTS_MOVE_END))
		    if (checkEndEvents()) return false; // end of match.
	    } else if (collision == COLLISION_BOTH) {
		LOG_INFO("collision on both sides. counter-rotate\n");
		// rotate slightly counterDirection and continue loop
		MotorDirection dir = MvtMgr->getMotorDirection();
		Radian counterTheta = COUNTER_THETA_DELTA
		    * (dir == MOTOR_DIRECTION_LEFT)?-1:1;
		Move->rotate(RobotPos->thetaAbsolute() + counterTheta);
		Events->wait(evtEndMove);
		if (!Events->isInWaitResult(EVENTS_MOVE_END))
		    if (checkEndEvents()) return false; // end of match
	    } else if (collision == COLLISION_NONE) {
		LOG_INFO("end-move but no collision. -> return false \n");
		// no idea what happened -> return false
		return false;
	    }
	}
    }
    LOG_INFO("leaving rotateOnSupport (%s)\n", rotated?"success":"failed");
    return rotated;
}

// should be easy...
bool StrategyLargeAttackCL::leaveSupport(Point supportCenter, Point target) {
    LOG_FUNCTION();
    const Radian THETA_DELTA = M_PI_4/3;
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->go2Target(target);
    Events->wait(evtEndMove);
    if (Events->isInWaitResult(EVENTS_MOVE_END)) {
	return true;
    } else {
	if (checkEndEvents()) return false; // end of match
	CollisionEnum collision = checkCollisionEvents();
	if (collision == COLLISION_LEFT ||
	    collision == COLLISION_RIGHT)
	{
	    // turn on wheel
	    bool collisionLeft = (collision == COLLISION_LEFT);
	    LOG_INFO("Collision (%s) in leaveSupport\n", collisionLeft?"left":"right");
	    bool stopLeft = collisionLeft;
	    Radian deltaTheta = THETA_DELTA* (collisionLeft? 1: -1);
	    Move->rotateOnWheel(RobotPos->thetaAbsolute() + deltaTheta, stopLeft);
	    Events->wait(evtEndMove);
	}
	// try again
	Move->go2Target(target);
	Events->wait(evtEndMove);
	return (Events->isInWaitResult(EVENTS_MOVE_END));
    }
}

bool StrategyLargeAttackCL::handleCollision(CollisionEnum collision,
					    Point lineStart,
					    Point lineEnd) {
    LOG_FUNCTION();
    PwmAlertObstacle pwmObstacle;
    getPwmObstaclePos(pwmObstacle);
    
    if (pwmObstacle.colliDetected) {
	LOG_WARNING("predefinedSkittleExploration: Collision detected\n");
	Point supportCenter; // will be set in calcSupportCenterCollision.
	bool potentialSupport =
	    calcSupportCenterCollision(pwmObstacle.ptObstacle, supportCenter);
	if (!potentialSupport) return false; // collision, but no support -> bad collision
	// TODO: handle multiple support-centers correctly.
	Log->support(supportCenter, supportCenter);

	// some data we are going to need:
	const bool isHorizontalLine = (lineStart.x == lineEnd.x);
	const bool isPositifLine = (isHorizontalLine?
				    (lineEnd.y > lineStart.y):
				    (lineEnd.x > lineStart.x));

	Radian lineDirection; // direction of line.
	Point nextIntersectionOnLine = supportCenter; // will be adjusted.
	Radian toLineDirection; // where's the line seen from support.
	Point intermediatePoint = supportCenter; // will be adjusted to 10cm towards line.
	bool doBlockLeftWheel; // used to destroy neighboring skittles.
	if (isHorizontalLine) {
	    if (isPositifLine) {
		lineDirection = 0;
		nextIntersectionOnLine.x += 150;
	    } else {
		lineDirection = M_PI;
		nextIntersectionOnLine.x -= 150;
	    }
	    nextIntersectionOnLine.y = lineStart.y;
	    if (supportCenter.y > lineStart.y) {
		toLineDirection = -M_PI_2;
		intermediatePoint.y -= 100;
		doBlockLeftWheel = isPositifLine;
	    } else {
		toLineDirection = M_PI_2;
		intermediatePoint.y += 100;
		doBlockLeftWheel = !isPositifLine;
	    }
	} else {
	    if (isPositifLine) {
		lineDirection = M_PI_2;
		nextIntersectionOnLine.y += 150;
	    } else {
		lineDirection = -M_PI_2;
		nextIntersectionOnLine.y -= 150;
	    }
	    nextIntersectionOnLine.x = lineStart.x;
	    if (supportCenter.x > lineStart.x) {
		toLineDirection = M_PI;
		intermediatePoint.x -= 100;
		doBlockLeftWheel = isPositifLine;
	    } else {
		toLineDirection = 0;
		intermediatePoint.x += 100;
		doBlockLeftWheel = !isPositifLine;
	    }
	}
	
	
	// go directly on support.
	bool centered = centerOnSupport(supportCenter);
	if (!centered) return false;
	bool rotated = rotateOnSupport(supportCenter, toLineDirection);
	if (!rotated) return false;

	bool outsideSupport = leaveSupport(supportCenter, intermediatePoint);
	if (!outsideSupport) return false;

	LOG_INFO("destroying skittles on other side of support\n");
	// destroy skittles (if there are any) on neighborcase.

	Move->rotateOnWheel(-M_PI_4, doBlockLeftWheel);
	Events->wait(evtEndMove);
	if (!Events->isInWaitResult(EVENTS_MOVE_END))
	    // for now just return false.
	    return false;
	
	// same direction as line
	Move->rotate(lineDirection);
	Events->wait(evtEndMove);
	if (!Events->isInWaitResult(EVENTS_MOVE_END))
	    // for now just return false.
	    return false;
	
	// go to next square-intersection.
	Move->go2Target(nextIntersectionOnLine);
	Events->wait(evtEndMove);
	if (!Events->isInWaitResult(EVENTS_MOVE_END))
	    // for now just return false.
	    return false;
    }
    return true;
}


// ---------------------------------------------------------------
// strategie exploration en passant par la rangee en y = 1350
// ---------------------------------------------------------------
bool StrategyLargeAttackCL::preDefinedSkittleExploration2()
{
    LOG_COMMAND("preDefinedLargeSkittleExploration2\n");
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    bool noRotation = true;
    Point lineStart(2500, 1500);
    Point lineEnd(3200, 1500);
    Move->go2Target(lineStart,
		    MOVE_USE_DEFAULT_GAIN,
		    MOVE_USE_DEFAULT_SPEED,
		    noRotation);
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        if (checkEndEvents()) return false; // c'est la fin du match?
	CollisionEnum collision = checkCollisionEvents();
	if (collision != COLLISION_NONE) {
	    if (!handleCollision(collision, lineStart, lineEnd))
		return false;
	} else {
	    LOG_WARNING("don't know what caused abort of movement. -> abort predefined exploration\n");
	    return false;
	}
    } else {
	// go2Target succeeded its movement.
	Move->rotate(0); // face right border
	Events->wait(evtEndMove);
	if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
	    if (checkEndEvents()) return false; // end of match
	    // ok. normally the collision can only be on the left side...
	    CollisionEnum collision = checkCollisionEvents();
	    if (collision == COLLISION_LEFT) {
		if (!handleCollision(collision, lineStart, lineEnd))
		    return false;
	    } else if (collision == COLLISION_NONE) {
		LOG_WARNING("unhandled event. leaving function\n");
		return false;
	    } else {
		LOG_WARNING("collision, but most likely not a support\n");
		return false;
	    }
	}
    }

    bool endOfLine = false;
    while (!endOfLine) {
	Move->go2Target(lineEnd);
	Events->wait(evtEndMove);
	if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
	    // let's hope it's a support.
	    if (checkEndEvents()) return false; // c'est la fin du match?
	    CollisionEnum collision = checkCollisionEvents();
	    if (collision != COLLISION_NONE) {
		if (!handleCollision(collision, lineStart, lineEnd))
		    return false;
	    } else {
		LOG_WARNING("don't know what caused abort of movement. -> abort predefined exploration\n");
		return false;
	    }
	    if (!RobotPos->isTargetForward(lineEnd))
		endOfLine = true;
	} else {
	    // ok. no more supports detected
	    endOfLine = true;
	}
    }

    LOG_INFO("predefined-large finished.\n");

    //alignBorder();
    
    // on recule un petit peu car on ne sais pas ce qu'on va se prendre en
    // approchant du robot adverse!, mieux vaut tenir que courir
    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Trajectory t;
    t.clear();
    t.push_back(Point(3144, 1350)); 
    t.push_back(Point(3190, 1700)); 
    Move->followTrajectory(t, TRAJECTORY_BASIC);
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }

    //alignBorder();

    // on va droit sur l'adversaire
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->go2Target(Point(3190, 650));
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }

    // on recule un peu
    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(Point(3190, 1050));
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }
    
    t.clear();
    t.push_back(Point(2550, 1050)); 
    t.push_back(Point(2594, 1650)); 
    t.push_back(Point(3340, 1650)); 
    MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD);
    Move->followTrajectory(t, TRAJECTORY_BASIC);
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }

    MvtMgr->setRobotDirection(MOVE_DIRECTION_BACKWARD);
    Move->go2Target(Point(3190, 1650));
    Events->wait(evtEndMove);
    if (!Events->isInWaitResult(EVENTS_MOVE_END)) {
        // on n'a pas reussi
        // c'est la fin du match?
        if (checkEndEvents()) return false;
        // TODO manage collisions
        return false;
    }

    return true;
}
