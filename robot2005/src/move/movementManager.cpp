#include <unistd.h>

#include "movementManager.h"
#include "mthread.h"
#include "log.h"
#include "robotTimer.h"


MovementManager* MovementManager::mvtMgr_=NULL;

// ----------------------------------------------------------------------------
// MovementManager::MovementManager
// ----------------------------------------------------------------------------
MovementManager::MovementManager() : 
    RobotComponent("Movement Manager", CLASS_MOVEMENT_MANAGER),
    motor_(NULL), position_(NULL), move_(NULL), 
    periodicCallback_(NULL), needMotorReset_(false), threadStarted_(false), 
    direction_(MOVE_DIRECTION_FORWARD)
{
    LOG_FUNCTION();
    position_ = new RobotPosition();
    move_     = new Move();
    mvtMgr_   = this;

    motorCom_.setAcc=-1;

    startThread();

    reset();
    LOG_OK("Initialisation Terminée\n");
}

// ----------------------------------------------------------------------------
// MovementManager::~MovementManager
// ----------------------------------------------------------------------------
MovementManager::~MovementManager()
{
    LOG_WARNING("Destructing MovementManager\n");
    motorReset();
    MTHREAD_CANCEL(thread_);
    if (motor_)    { delete motor_;    motor_ = NULL; }
    if (position_) { delete position_; position_ = NULL; }
    if (move_)     { delete move_;     move_ = NULL; }
    mvtMgr_=NULL;
}

// ----------------------------------------------------------------------------
// MovementManager::motorReset
// ----------------------------------------------------------------------------
void MovementManager::motorReset()
{
  motorCom_.reset = true; // motor must be reseted by the thread
  int counter=0;
  if (threadStarted_) while(motorCom_.reset && ++counter<100) usleep(1000);
  if (counter >= 100) LOG_ERROR("Cannot reset the motors\n");
}

// ----------------------------------------------------------------------------
// MovementManager::resetPwmAlert
// ----------------------------------------------------------------------------
void MovementManager::resetPwmAlert()
{
    motorCom_.resetPwmAlert = true;
}

// ----------------------------------------------------------------------------
// MovementManager::reset
// ----------------------------------------------------------------------------
bool MovementManager::reset()
{
    init_=threadStarted_;
    motorReset();
    if (position_) { init_ &= position_->reset(); }
    if (move_)     { init_ &= move_->reset(); }
    periodicCallback_ = NULL;
    if (!init_) {
        LOG_ERROR("MovementManager n'est pas resete correctement\n");
    } else {
        LOG_OK("MovementManager est resete\n");
    }
    setRobotDirection(MOVE_DIRECTION_FORWARD);
    return isInitialized();
}
    
// ----------------------------------------------------------------------------
// MovementManager::validate
// ----------------------------------------------------------------------------
bool MovementManager::validate()
{
  return (position_->validate() 
	  && move_->validate());
}

// ----------------------------------------------------------------------------
// movementManagerThreadBody
// ----------------------------------------------------------------------------
static const int MOVEMENT_MANAGER_THREAD_PERIOD=1000;


static void* movementManagerThreadBody(void* data)
{
    MovementManager* mvtMgr = MovementManager::instance();
    while (true) {
        mvtMgr->periodicTask();
        usleep(MOVEMENT_MANAGER_THREAD_PERIOD);
    }
    return NULL;
}

// ----------------------------------------------------------------------------
// MovementManager::startThread
// ----------------------------------------------------------------------------
void MovementManager::startThread()
{
    threadStarted_= (MTHREAD_CREATE("MvtMgr Thread",
			   &thread_, 
			   NULL, 
			   movementManagerThreadBody,
			   NULL)==0);
    if (!threadStarted_) {
      LOG_ERROR("startThread failed (%d)\n", init_);
    } else {
      LOG_OK("startThread\n");
    }
}

// ----------------------------------------------------------------------------
// MovementManager::periodicTask
// ----------------------------------------------------------------------------
void MovementManager::periodicTask()
{
    if (!motor_) {
	// TODO: getClassConfig doesn't contain motorReal and no automaticMotorReset [flo]
	/*
        if (getClassConfig()->motorReal) {
	motor_ = new MotorReal(getClassConfig()->automaticMotorReset);
        } else {
#ifdef SIMULATED
	motor_ = new MotorSimu(getClassConfig()->automaticMotorReset);
#else
	motor_ = new MotorReal(getClassConfig()->automaticMotorReset);
#endif // SIMULATED
      }*/
    }
    if (motorCom_.reset && motor_) {
        motor_->reset();
        motorCom_.reset = false;
	if (position_) { position_->resetHctlCoders(); }
    }
    if (motorCom_.setAcc > 0 && motor_) {
        motor_->setAcceleration(motorCom_.setAcc);
        motorCom_.setAcc =-1;
    }
    if (motorCom_.resetPwmAlert) {
	resetPwmAlert();
	motorCom_.resetPwmAlert=false;
    }
    Millisecond time = Timer->time();
    if (position_) { position_->periodicTask(time); }
    if (move_)     { move_->periodicTask(time);     }
    if (motor_)    { 
      motor_->setSpeed(motorCom_.speedLeft, motorCom_.speedRight);
      motor_->getPosition(motorCom_.posLeft, motorCom_.posRight);
      motor_->getPWM(motorCom_.pwmLeft, motorCom_.pwmRight);
      // TODO: Motor doesn't have periodicTask anymore [flo]
      //motor_->periodicTask(time); 
    }
    if (periodicCallback_) {
        periodicCallback_();
    }
}

/** Defini la constante d'acceleration des moteurs */
void MovementManager::setAcceleration(MotorAcceleration acceleration)
{
  motorCom_.setAcc  = acceleration;
}

// ----------------------------------------------------------------------------
// MovementManager::setSpeed
// ----------------------------------------------------------------------------
void MovementManager::setSpeed(MotorSpeed  left,
			       MotorSpeed  right)
{
  motorCom_.speedLeft  = left;
  motorCom_.speedRight = right; 
}

// ----------------------------------------------------------------------------
// MovementManager::getCoderPosition
// ----------------------------------------------------------------------------
void MovementManager::getCoderPosition(CoderPosition& left, 
				       CoderPosition& right)
{
  left  = (CoderPosition)motorCom_.posLeft;
  right = (CoderPosition)motorCom_.posRight;
}

// ----------------------------------------------------------------------------
// MovementManager::getPWM
// ----------------------------------------------------------------------------
void MovementManager::getPWM(MotorPWM& left, 
			     MotorPWM& right)
{
  left  = motorCom_.pwmLeft;
  right = motorCom_.pwmRight;
}

// ----------------------------------------------------------------------------
// MovementManager::getPWM
// ----------------------------------------------------------------------------
void MovementManager::enableAutomaticReset(bool enable)
{
  if (motor_) motor_->enableAutomaticReset(enable);
}

// ----------------------------------------------------------------------------
// MovementManager::setRobotDirection
// ----------------------------------------------------------------------------
void MovementManager::setRobotDirection(MoveDirection direction)
{
    LOG_WARNING("MovementManager::setRobotDirection(%s)\n",
                direction==MOVE_DIRECTION_FORWARD?
                "MOVE_DIRECTION_FORWARD":"MOVE_DIRECTION_BACKWARD");
    direction_ = direction;
}

// ----------------------------------------------------------------------------
// MovementManager::getMotorDirection
// ----------------------------------------------------------------------------
MotorDirection MovementManager::getMotorDirection()
{
    MotorSpeed  speedLeft=0;
    MotorSpeed  speedRight=0;
    getMotorSpeed(speedLeft, speedRight);
    if (speedLeft == 0 && speedRight == 0) {
	return MOTOR_DIRECTION_STOP;
    } else if (speedLeft > speedRight) {
	if (speedRight > 0) {
	    return MOTOR_DIRECTION_FORWARD;
	} else if (speedLeft < 0) {
	    return MOTOR_DIRECTION_BACKWARD;
	} else {
	    return MOTOR_DIRECTION_RIGHT;
	}
    } else { // speedRight >= speedLeft
	if (speedLeft > 0) {
	    return MOTOR_DIRECTION_FORWARD;
	} else if (speedRight< 0) {
	    return MOTOR_DIRECTION_BACKWARD;
	} else {
	    return MOTOR_DIRECTION_LEFT;
	}
    }
    return MOTOR_DIRECTION_STOP;
}

