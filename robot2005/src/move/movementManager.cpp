 #include <unistd.h>

 #include "movementManager.h"
 #include "mthread.h"
 #include "log.h"
 #include "robotTimer.h"
 #include "robotConfig2005.h"


 MovementManagerCL* MovementManagerCL::mvtMgr_=NULL;

 // ----------------------------------------------------------------------------
 // MovementManagerCL::MovementManagerCL
 // ----------------------------------------------------------------------------
MovementManagerCL::MovementManagerCL(MotorCL* motor, OdometerCL* odom) : 
     RobotComponent("Movement Manager", CLASS_MOVEMENT_MANAGER),
     motor_(motor), position_(NULL), move_(NULL), odometer_(odom), 
     periodicCallback_(NULL), needMotorReset_(false), threadStarted_(false), 
     direction_(MOVE_DIRECTION_FORWARD)
 {
     LOG_FUNCTION();
     mvtMgr_   = this;
     position_ = new RobotPositionCL();
     move_     = new MoveCL();
     mvtMgr_   = this;

     motorCom_.setAcc=-1;

     startThread();

     reset();
     LOG_OK("Initialisation Terminée\n");
 }

 // ----------------------------------------------------------------------------
 // MovementManagerCL::~MovementManagerCL
 // ----------------------------------------------------------------------------
 MovementManagerCL::~MovementManagerCL()
 {
     LOG_WARNING("Destructing MovementManager\n");
     motorReset();
     MTHREAD_CANCEL(thread_);
     if (position_) { delete position_; position_ = NULL; }
     if (move_)     { delete move_;     move_ = NULL; }
     mvtMgr_=NULL;
 }

 // ----------------------------------------------------------------------------
 // MovementManagerCL::motorReset
 // ----------------------------------------------------------------------------
 void MovementManagerCL::motorReset()
 {
     motorCom_.reset = true; // motor must be reseted by the thread
     int counter=0;
     if (threadStarted_) while(motorCom_.reset && ++counter<100) usleep(1000);
     if (counter >= 100) LOG_ERROR("Cannot reset the motors\n");
 }

 // ----------------------------------------------------------------------------
 // MovementManagerCL::resetPwmAlert
 // ----------------------------------------------------------------------------
 void MovementManagerCL::resetPwmAlert()
 {
     motorCom_.resetPwmAlert = true;
 }

 // ----------------------------------------------------------------------------
 // MovementManagerCL::reset
 // ----------------------------------------------------------------------------
 bool MovementManagerCL::reset()
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
 // MovementManagerCL::validate
 // ----------------------------------------------------------------------------
 bool MovementManagerCL::validate()
 {
   return (position_->validate() 
	   && move_->validate());
 }

 // ----------------------------------------------------------------------------
 // movementManagerThreadBody
 // ----------------------------------------------------------------------------
 static const int MOVEMENT_MANAGER_THREAD_PERIOD=10;


 static void* movementManagerThreadBody(void* data)
 {
     MovementManagerCL* mvtMgr = MovementManagerCL::instance();
     while (true) {
	 mvtMgr->periodicTask();
	 usleep(MOVEMENT_MANAGER_THREAD_PERIOD*1000);
     }
     return NULL;
 }

 // ----------------------------------------------------------------------------
 // MovementManagerCL::startThread
 // ----------------------------------------------------------------------------
 void MovementManagerCL::startThread()
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
 // MovementManagerCL::periodicTask
 // ----------------------------------------------------------------------------
 void MovementManagerCL::periodicTask()
 {
     if (motor_) {
         if (!motor_->isStarted()) {
             motor_->start();
             motor_->registerResetCallback(robotPositionMotorHasBeenReset);
         }
         if (motorCom_.reset) {
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
     }
     Millisecond time = Timer->time();
     if (position_) { position_->periodicTask(time); }
     if (move_)     { move_->periodicTask(time);     }
     if (motor_)    { 
         motor_->setSpeed(motorCom_.speedLeft, motorCom_.speedRight);
         motor_->getPosition(motorCom_.posLeft, motorCom_.posRight);
         motor_->getPWM(motorCom_.pwmLeft, motorCom_.pwmRight);
         motor_->periodicTask();
    }
    if (periodicCallback_) {
        periodicCallback_();
    }
    static short logCounter=0;
    if (logCounter++ > 5) {
        if (motor_) Log->motor(motorCom_.pwmLeft, motorCom_.pwmRight,
                               motorCom_.posLeft, motorCom_.posRight);
        if (position_) Log->position(position_->pos());
        logCounter = 0;
    }
}

/** Defini la constante d'acceleration des moteurs */
void MovementManagerCL::setAcceleration(MotorAcceleration acceleration)
{
    motorCom_.setAcc  = acceleration;
}

// ----------------------------------------------------------------------------
// MovementManagerCL::setSpeed
// ----------------------------------------------------------------------------
void MovementManagerCL::setSpeed(MotorSpeed  left,
				 MotorSpeed  right)
{
    motorCom_.speedLeft  = left;
    motorCom_.speedRight = right; 
}

// ----------------------------------------------------------------------------
// MovementManagerCL::getCoderPosition
// ----------------------------------------------------------------------------
void MovementManagerCL::getCoderPosition(CoderPosition& left, 
				       CoderPosition& right)
{
    left  = (CoderPosition)motorCom_.posLeft;
    right = (CoderPosition)motorCom_.posRight;
}

// ----------------------------------------------------------------------------
// MovementManagerCL::getPWM
// ----------------------------------------------------------------------------
void MovementManagerCL::getPWM(MotorPWM& left, 
			     MotorPWM& right)
{
    left  = motorCom_.pwmLeft;
    right = motorCom_.pwmRight;
}

// ----------------------------------------------------------------------------
// MovementManagerCL::getPWM
// ----------------------------------------------------------------------------
void MovementManagerCL::enableAutomaticReset(bool enable)
{
    if (motor_) motor_->enableAutomaticReset(enable);
}

// ----------------------------------------------------------------------------
// MovementManagerCL::setRobotDirection
// ----------------------------------------------------------------------------
void MovementManagerCL::setRobotDirection(MoveDirection direction)
{
    LOG_WARNING("MovementManager::setRobotDirection(%s)\n",
                direction==MOVE_DIRECTION_FORWARD?
                "MOVE_DIRECTION_FORWARD":"MOVE_DIRECTION_BACKWARD");
    direction_ = direction;
}

// ----------------------------------------------------------------------------
// MovementManagerCL::getMotorDirection
// ----------------------------------------------------------------------------
MotorDirection MovementManagerCL::getMotorDirection()
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

