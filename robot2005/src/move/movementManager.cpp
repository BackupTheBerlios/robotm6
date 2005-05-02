#include <unistd.h>

//#define LOG_DEBUG_ON
#include "movementManager.h"
#include "mthread.h"
#include "log.h"
#include "robotTimer.h"
#include "robotConfig2005.h"
#include "events.h"


 MovementManagerCL* MovementManagerCL::mvtMgr_=NULL;

 // ----------------------------------------------------------------------------
 // MovementManagerCL::MovementManagerCL
 // ----------------------------------------------------------------------------
MovementManagerCL::MovementManagerCL(MotorCL* motor, OdometerCL* odom) : 
     RobotComponent("Movement Manager", CLASS_MOVEMENT_MANAGER),
     motor_(motor), position_(NULL), move_(NULL), odometer_(odom), 
     periodicCallback_(NULL), needMotorReset_(false), threadStarted_(false), 
     direction_(MOVE_DIRECTION_FORWARD),
     hctlDeltaMoveLeft_(0),
     hctlDeltaMoveRight_(0),
     odomDeltaMoveLeft_(0),
     odomDeltaMoveRight_(0),
     pwmDeltaLeft_(0),
     pwmDeltaRight_(0), 
     oldHctlLeftDelta_(0),
     oldHctlRightDelta_(0),
     oldOdomLeftDelta_(0),
     oldOdomRightDelta_(0),
     pattinageIndex_(0),
     pattinageBufferSize_(0)
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
     resetPatinageDetection();
 }

void MovementManagerCL::motorIdle()
{
  motorCom_.idle=true;
}

void MovementManagerCL::motorIdleLeft()
{
  motorCom_.idleLeft=true;
}

void MovementManagerCL::motorIdleRight()
{
  motorCom_.idleRight=true;
}

void MovementManagerCL::motorUnidle()
{
  motorCom_.unidle=1;
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
     resetPatinageDetection();
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
 static const int MOVEMENT_MANAGER_THREAD_PERIOD=1;


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
         if (motorCom_.idleLeft) {
             motor_->idleLeft();
             motorCom_.idleLeft = false;
         }
         if (motorCom_.idleRight) {
             motor_->idleRight();
             motorCom_.idleRight = false;
         }
         if (motorCom_.idle) {
             motor_->idle();
             motorCom_.idle = false;
         }
         if (motorCom_.unidle==20) {
	   motorCom_.speedLeft = 0;
	   motorCom_.speedRight = 0;
	   motorCom_.unidle = 0;
	 } else if (motorCom_.unidle == 1) {
	     motorCom_.speedLeft=8;
	     motorCom_.speedRight=8;
	     motorCom_.unidle++;
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
	 checkPatinage(); // rais an event if motor and odom disagree
    }
    if (periodicCallback_) {
        periodicCallback_();
    }

    static short logCounter=0;
    if (logCounter++ > 5) {
        if (motor_) Log->motor(motorCom_.pwmLeft, motorCom_.pwmRight,
                               motorCom_.posLeft, motorCom_.posRight);
        if (position_) Log->position(position_->x(),
				     position_->y(),
				     position_->thetaAbsolute());
        logCounter = 0;
    }
}

template <typename T>
void resetPatinageBuffer(T buf[])
{
  for(unsigned int i=0; i< PATINAGE_BUFFER_SIZE; i++) {
    buf[i]=0;
    buf[i]=0;
  }
}

static const MotorPWM PATINAGE_PWM_LIMIT = 60;
// --------------------------------------------------------------------------------
// detection du patinage des roues
// -------------------------------------------------------------------------------- 
void MovementManagerCL::updatePatinageBuffers() 
{
  getCoderPosition(hctlLeftBuffer_[pattinageIndex_],
		   hctlRightBuffer_[pattinageIndex_]);
  getPWM(pwmLeftBuffer_[pattinageIndex_],
	 pwmRightBuffer_[pattinageIndex_]);
  odometer_->getCoderPosition(odomLeftBuffer_[pattinageIndex_],
			      odomRightBuffer_[pattinageIndex_]);
  int pattinageIndexLast = pattinageIndex_;
  pattinageIndex_ = (pattinageIndex_+1) % PATINAGE_BUFFER_SIZE;
  if (pattinageBufferSize_ < PATINAGE_BUFFER_SIZE) { 
    pattinageBufferSize_++;
  } else {
    pattinageBufferSize_ = 2*PATINAGE_BUFFER_SIZE;
    // compare la position des hctl et des odometre entre la position
    // courante et la position il y a PATINAGE_BUFFER_SIZE cycles
    LOG_DEBUG("IL=%d %d H=%d %d \n",
	      pattinageIndex_, pattinageIndexLast, 
	      hctlLeftBuffer_[pattinageIndex_] , hctlLeftBuffer_[pattinageIndexLast]);
    LOG_DEBUG("IR=%d %d H=%d %d\n",
	      pattinageIndex_, pattinageIndexLast, 
	      hctlRightBuffer_[pattinageIndex_] , hctlRightBuffer_[pattinageIndexLast]);

    // ODOMETERS
    CoderPosition delta = abs((odomLeftBuffer_[pattinageIndex_] 
			       - odomLeftBuffer_[pattinageIndexLast]));
    if (delta > CODER_POS_MAX) delta -= CODER_POS_DELTA;
    if (oldOdomLeftDelta_ * motorCom_.speedLeft < -1) {
      // on a change de sens de mouvement, on reset les buffer
      resetPatinageBuffer(odomLeftBuffer_);
      resetPatinageBuffer(hctlLeftBuffer_);
      resetPatinageBuffer(pwmLeftBuffer_);
      odomDeltaMoveLeft_ = 0; 
      resetPatinageBuffer(odomRightBuffer_);
      resetPatinageBuffer(hctlRightBuffer_);
      resetPatinageBuffer(pwmRightBuffer_);
      odomDeltaMoveRight_ = 0;
      LOG_DEBUG("%d\n", oldOdomLeftDelta_);

    } else {
      odomDeltaMoveLeft_  = (int)(RobotConfig->getOdometerKLeft() * delta);
    }
    oldOdomLeftDelta_ = motorCom_.speedLeft;
   

    delta = abs((odomRightBuffer_[pattinageIndex_] 
		 - odomRightBuffer_[pattinageIndexLast]));
    if (delta > CODER_POS_MAX) delta -= CODER_POS_DELTA;
    if (oldOdomRightDelta_ * motorCom_.speedRight < -1) {
      // on a change de sens de mouvement, on reset les buffer
      resetPatinageBuffer(odomLeftBuffer_);
      resetPatinageBuffer(hctlLeftBuffer_);
      resetPatinageBuffer(pwmLeftBuffer_);
      odomDeltaMoveLeft_ = 0; 
      resetPatinageBuffer(odomRightBuffer_);
      resetPatinageBuffer(hctlRightBuffer_);
      resetPatinageBuffer(pwmRightBuffer_);
      odomDeltaMoveRight_ = 0;
      LOG_DEBUG("%d\n", oldOdomRightDelta_);

    } else {
      odomDeltaMoveRight_ = (int)(RobotConfig->getOdometerKRight() * delta);
    }
    oldOdomRightDelta_ = motorCom_.speedRight;


    // HCTLS
    delta = abs((hctlLeftBuffer_[pattinageIndex_] 
		 - hctlLeftBuffer_[pattinageIndexLast]));
    if (delta > MOTOR_POS_MAX) delta -= MOTOR_POS_DELTA;
    hctlDeltaMoveLeft_  = (int)(RobotConfig->getMotorKLeft() * delta);



    delta = abs((hctlRightBuffer_[pattinageIndex_] 
		 - hctlRightBuffer_[pattinageIndexLast]));
    if (delta > MOTOR_POS_MAX) delta -= MOTOR_POS_DELTA;
    hctlDeltaMoveRight_ = (int)(RobotConfig->getMotorKRight() * delta);
    

    pwmDeltaLeft_ =0;	
    pwmDeltaRight_ =0;
    for(unsigned int i=0; i< PATINAGE_BUFFER_SIZE; i++) {
      pwmDeltaLeft_ +=pwmLeftBuffer_[i];
      pwmDeltaRight_ +=pwmRightBuffer_[i];
    } 
    if (pwmLeftBuffer_[pattinageIndex_] < PATINAGE_PWM_LIMIT) {
      resetPatinageBuffer(pwmLeftBuffer_);
    } else {
      for(unsigned int i=0; i< PATINAGE_BUFFER_SIZE; i++) {
	pwmDeltaLeft_ +=pwmLeftBuffer_[i];
      } 
    }
    if (pwmRightBuffer_[pattinageIndex_] < PATINAGE_PWM_LIMIT) {
        resetPatinageBuffer(pwmRightBuffer_);
    } else {
      for(unsigned int i=0; i< PATINAGE_BUFFER_SIZE; i++) {
	pwmDeltaRight_ +=pwmRightBuffer_[i];
      } 
    }
    LOG_DEBUG("left: o=%d h=%d p=%d;  right: o=%d h=%d p=%d\n", 
	      (int)odomDeltaMoveLeft_, (int)hctlDeltaMoveLeft_, (int)floor(pwmDeltaLeft_/PATINAGE_BUFFER_SIZE),
	      (int)(odomDeltaMoveRight_), (int)hctlDeltaMoveRight_, (int)floor(pwmDeltaRight_/PATINAGE_BUFFER_SIZE));
  }
}


void MovementManagerCL::resetPatinageDetection()
{
  pattinageBufferSize_ = 0;
  pwmDeltaLeft_=0;
  pwmDeltaRight_=0;
  resetPatinageBuffer(pwmLeftBuffer_);
  resetPatinageBuffer(pwmRightBuffer_);
}

void MovementManagerCL::checkPatinage() 
{
  // compare motor and odometer pos

  updatePatinageBuffers();
  if (pattinageBufferSize_ <= PATINAGE_BUFFER_SIZE+2) return; // pas
							   // 
							   // assez de donnees !
  // odometre bouge de moins de 2cm
  // hctl bougent de plus de 10 cm
  if ((abs(odomDeltaMoveLeft_) < 10) 
      && (abs(hctlDeltaMoveLeft_) > 60)) { 
    LOG_WARNING("Patinage roue gauche! odom=%d hctl=%d\n", 
		(int)odomDeltaMoveLeft_, (int)hctlDeltaMoveLeft_ );
    Events->raise(EVENTS_PWM_ALERT_LEFT);
    Move->idleMotorLeft();
    resetPatinageDetection();
  }
  if ((abs(odomDeltaMoveRight_) < 10) 
      && (abs(hctlDeltaMoveRight_) > 60)) { 
    LOG_WARNING("Patinage roue droite! odom=%d hctl=%d\n",
		(int)(odomDeltaMoveRight_), (int)hctlDeltaMoveRight_ );
    Events->raise(EVENTS_PWM_ALERT_RIGHT);
    Move->idleMotorRight();
    resetPatinageDetection();
  }

  if ((abs(odomDeltaMoveLeft_) < 10) 
      && (abs(pwmDeltaLeft_) > PATINAGE_PWM_LIMIT*PATINAGE_BUFFER_SIZE)) { 
    LOG_WARNING("Forcage roue gauche! odom=%d pwm=%d\n", 
		(int)odomDeltaMoveLeft_, (int)floor(pwmDeltaLeft_ /PATINAGE_BUFFER_SIZE));
    Events->raise(EVENTS_PWM_ALERT_LEFT);
    Move->idleMotorLeft();
    resetPatinageDetection();
  }
  if ((abs(odomDeltaMoveRight_) < 10) 
      && (abs(pwmDeltaRight_) > PATINAGE_PWM_LIMIT*PATINAGE_BUFFER_SIZE)) { 
    LOG_WARNING("Forcage roue droite! odom=%d pwm=%d\n",
		(int)(odomDeltaMoveRight_), (int)floor(pwmDeltaRight_/PATINAGE_BUFFER_SIZE));
    Events->raise(EVENTS_PWM_ALERT_RIGHT);
    Move->idleMotorRight();
    resetPatinageDetection();
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

