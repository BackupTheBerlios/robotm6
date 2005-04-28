#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "move.h"
#include "movement.h"
#include "robotPosition.h"
#include "mthread.h" //lock
#include "log.h"
#include "movementManager.h"
#include "events.h"
#include "geometry2D.h"
#include "explore/trajectoryGenerator.h"

// ----------------------------------------------------------------------------
// namespace
// ----------------------------------------------------------------------------

namespace {
  pthread_mutex_t repositoryLock = PTHREAD_MUTEX_INITIALIZER;
  void no_op(...) 
  {
    
  };
}

// ----------------------------------------------------------------------------
// Static members
// ----------------------------------------------------------------------------
MoveCL* MoveCL::move_=NULL;

// ----------------------------------------------------------------------------
// MoveCL::MoveCL
// ----------------------------------------------------------------------------
MoveCL::MoveCL() : 
    RobotComponent("Move", CLASS_MOVE), currentMovement_(NULL),
    enableAccelerationController_(false), userAbort_(false),
    defaultRotationGain_(10*MOVE_GAIN_DEFAULT),
    defaultLinearGain_(MOVE_GAIN_DEFAULT),
    defaultMaxRotationSpeed_(MOVE_MAX_ROTATION_SPEED),
    defaultMaxLinearSpeed_(MOVE_SPEED_DEFAULT),
    defaultBasicSpeed_(MOVE_SPEED_DEFAULT),
    defaultRealignDist_(MOVE_REALIGN_DIST_DEFAULT),
    trajectoryIndexOffset_(0)
{
    pthread_mutex_init(&repositoryLock, NULL);
    assert(move_ == NULL);
    move_ = this;
    init_ = true;
    LOG_OK("Initialisation Terminée\n");
}

// ----------------------------------------------------------------------------
// MoveCL::~MoveCL
// ----------------------------------------------------------------------------
MoveCL::~MoveCL()
{
    if (currentMovement_) {
        delete currentMovement_;
        currentMovement_ = NULL;
    }
    move_ = NULL;
}

// ----------------------------------------------------------------------------
// MoveCL::instance
// ----------------------------------------------------------------------------
MoveCL* MoveCL::instance()
{
    assert(move_);
    return move_;
}

// ----------------------------------------------------------------------------
// MoveCL::periodicTask
// ----------------------------------------------------------------------------
void MoveCL::periodicTask(Millisecond time)
{   
    LOG_DEBUG("Periodic Task WAIT LOCK\n");
    Lock localLock(&repositoryLock);
    LOG_DEBUG("Periodic Task %d LOCK\n", (int)currentMovement_);
    if (currentMovement_) {
	try {
	    currentMovement_->periodicTask();
	} catch(...){}
    } else {
	setSpeed(lastSpeedReqLeft_ , lastSpeedReqRight_);
    }
    LOG_DEBUG("Periodic Task UNLOCK\n");
}

// ----------------------------------------------------------------------------
// MoveCL::reset
// ----------------------------------------------------------------------------
bool MoveCL::reset()
{
    LOG_FUNCTION();

    defaultRotationGain_     = MOVE_GAIN_DEFAULT;
    defaultLinearGain_       = MOVE_GAIN_DEFAULT;
    defaultMaxRotationSpeed_ = MOVE_MAX_ROTATION_SPEED;
    defaultMaxLinearSpeed_   = MOVE_SPEED_DEFAULT;
    defaultBasicSpeed_       = MOVE_SPEED_DEFAULT;

    emergencyStop();
    Lock localLock(&repositoryLock);
    LOG_DEBUG("%s LOCK\n", __FUNCTION__);
    if (currentMovement_) {
        delete currentMovement_;
	currentMovement_ = NULL;
    }
    LOG_DEBUG("%s currentMovement_ = NULL;\n", __FUNCTION__);
    //LOG_DEBUG("%s UNLOCK\n", __FUNCTION__);
    return true;
}

// ----------------------------------------------------------------------------
// MoveCL::validate
// ----------------------------------------------------------------------------
bool MoveCL::validate()
{
    return true;
}

// ----------------------------------------------------------------------------
// MoveCL::calibrate
// ----------------------------------------------------------------------------
void MoveCL::calibrate()
{
}

// ----------------------------------------------------------------------------
// MoveCL::enableAccelerationController
// ----------------------------------------------------------------------------
void MoveCL::enableAccelerationController(bool enable)
{
    enableAccelerationController_ = enable;
}

// ----------------------------------------------------------------------------
// MoveCL::setSpeed
// ----------------------------------------------------------------------------
MotorSpeed MoveCL::lastSpeedLeft_ =0;
MotorSpeed MoveCL::lastSpeedRight_=0;
MotorSpeed MoveCL::lastSpeedReqLeft_ =0;
MotorSpeed MoveCL::lastSpeedReqRight_=0;

void MoveCL::setSpeed(MotorSpeed  speedLeft,
                      MotorSpeed  speedRight)
{
    lastSpeedReqLeft_ =speedLeft;
    lastSpeedReqRight_=speedRight;
    LOG_DEBUG("Set speed: %d %d\n",speedLeft, speedRight);
    if (enableAccelerationController_) {
        double deltaLeft  = speedLeft - lastSpeedLeft_;
        if (fabs(deltaLeft) > MOVE_MAX_DELTA_SPEED) {
            
            speedLeft = (MotorSpeed)(lastSpeedLeft_ + sign(deltaLeft) * MOVE_MAX_DELTA_SPEED);
        }
        double deltaRight = speedRight - lastSpeedRight_;
        if (fabs(deltaRight) > MOVE_MAX_DELTA_SPEED) {
            speedRight = (MotorSpeed)(lastSpeedRight_ + sign(deltaRight) * MOVE_MAX_DELTA_SPEED);
        }
    }
   
    MvtMgr->setSpeed(speedLeft, speedRight);
    lastSpeedLeft_  = speedLeft;
    lastSpeedRight_ = speedRight;
}

// ----------------------------------------------------------------------------
// MoveCL::emergencyStop
// ----------------------------------------------------------------------------
void MoveCL::emergencyStop()
{
    stop();
    lastSpeedLeft_    = 0;
    lastSpeedRight_   = 0;
    lastSpeedReqLeft_ = 0;
    lastSpeedReqRight_= 0;
    MvtMgr->setSpeed(0, 0);
}

// ----------------------------------------------------------------------------
// MoveCL::idleMotorLeft
// ----------------------------------------------------------------------------
void MoveCL::idleMotorLeft()
{
    lastSpeedLeft_    = 0;
    lastSpeedReqLeft_ = 0;
    MvtMgr->motor()->idleLeft();
}

// ----------------------------------------------------------------------------
// MoveCL::idleMotorRight
// ----------------------------------------------------------------------------
void MoveCL::idleMotorRight()
{
    lastSpeedRight_   = 0;
    lastSpeedReqRight_= 0;
    MvtMgr->motor()->idleRight();
}

// ----------------------------------------------------------------------------
// MoveCL::setCurrentMovement
// ----------------------------------------------------------------------------
void MoveCL::setCurrentMovement(Movement* mvt, bool needLock)
{
    if (userAbort_) return;
    Lock* localLock=NULL;
    LOG_DEBUG("%s lock:%s currentMvt=%d nextMvt=%d\n", 
	      __FUNCTION__, needLock?"t":"f", 
	      (int)(currentMovement_), (int)mvt);
    if (needLock) localLock = new Lock(&repositoryLock);
    LOG_DEBUG("%s LOCK\n", __FUNCTION__);
    // Fin du mouvement quand mvt==NULL -> raiser un events
    if (mvt != NULL) {
      Events->unraise(EVENTS_MOVE_END);
    } else {
      Events->raise(EVENTS_MOVE_END);
      lastSpeedReqLeft_=0;
      lastSpeedReqRight_=0;
    }
    LOG_DEBUG("%s afterEvents\n", __FUNCTION__);
    if (currentMovement_) {
        LOG_DEBUG("%s delete %d\n", __FUNCTION__, (int) currentMovement_);
        delete currentMovement_;
	currentMovement_=NULL;
    }
    LOG_DEBUG("%s  currentMovement_ = mvt; %d %d\n", 
	      __FUNCTION__, (int) currentMovement_, (int)mvt);
    currentMovement_ = mvt;
    if (currentMovement_) {
        currentMovement_->start();
    }
    if (localLock) {
      LOG_DEBUG("%s UNLOCK\n", __FUNCTION__);
      delete localLock;
    }
}

// ----------------------------------------------------------------------------
// MoveCL::stop
// ----------------------------------------------------------------------------
void MoveCL::stop()
{
    Lock localLock(&repositoryLock);
    LOG_DEBUG("%s LOCK\n", __FUNCTION__);
    if (currentMovement_) {
	// empeche l'execution d'un autre mouvement quand le mvt courant se terminera
        currentMovement_->unregisterNextMovement();
        // termine le mouvement courant
        currentMovement_->stop();
        // currentMovement_->stop appel setCurrentMovement(NULL)
    } else {
	lastSpeedReqLeft_ = 0;
	lastSpeedReqRight_= 0;
    }
    LOG_DEBUG("%s UNLOCK\n", __FUNCTION__);
}

// ----------------------------------------------------------------------------
// MoveCL::forward
// ----------------------------------------------------------------------------
void MoveCL::forward(Millimeter  dist, 
                     MotorSpeed  maxSpeed)
{
    stop();
    if (maxSpeed == MOVE_USE_DEFAULT_SPEED) {
	maxSpeed = defaultBasicSpeed_;
    }
    setCurrentMovement(new MovementForward(dist,  
                                           maxSpeed,
                                           this));
}

// ----------------------------------------------------------------------------
// MoveCL::backward
// ----------------------------------------------------------------------------
void MoveCL::backward(Millimeter  dist, 
                      MotorSpeed  maxSpeed)
{
    stop();
    if (maxSpeed == MOVE_USE_DEFAULT_SPEED) {
	maxSpeed = defaultBasicSpeed_;
    }
    setCurrentMovement(new MovementBackward(dist, 
                                            maxSpeed,
                                            this));
}

// ----------------------------------------------------------------------------
// MoveCL::setSpeedOnDist
// ----------------------------------------------------------------------------
void MoveCL::setSpeedOnDist(Millimeter  dist, 
                            MotorSpeed  speedLeft,
                            MotorSpeed  speedRight,
                            bool        idleBlockedWheel)
{
    stop();
    if (idleBlockedWheel) {
        // desasservi la roue blocquee
        if (speedLeft == 0) {
            idleMotorLeft();
        } else if (speedRight == 0) {
            idleMotorRight();
        }
    }
    setCurrentMovement(new MovementSetSpeed(dist, 
                                            speedLeft,
                                            speedRight,
                                            this));
}

// ----------------------------------------------------------------------------
// Move::rotate
// ----------------------------------------------------------------------------
void MoveCL::rotate(Radian      finalDir,
                    MoveGain    gain,
                    MotorSpeed  maxSpeed)
{
    stop();
    if (gain == MOVE_USE_DEFAULT_GAIN) {
	gain = defaultRotationGain_;
    }if (maxSpeed == MOVE_USE_DEFAULT_SPEED) {
	maxSpeed = defaultMaxRotationSpeed_;
    }
    setCurrentMovement(new MovementRotate(finalDir, 
                                          gain, 
                                          maxSpeed,
                                          this));
}

// ----------------------------------------------------------------------------
// Move::realign
// ----------------------------------------------------------------------------
void MoveCL::realign(Radian      finalDir,
                     Millimeter  backwardDist,
                     MoveGain    gain,
                     MotorSpeed  maxSpeed)
{
    stop();
    if (gain == MOVE_USE_DEFAULT_GAIN) {
	gain = defaultRotationGain_;
    }
    if (maxSpeed == MOVE_USE_DEFAULT_SPEED) {
	maxSpeed = defaultMaxRotationSpeed_;
    }
    if (fabs(backwardDist - MOVE_USE_DEFAULT_DIST)<0.1) {
	backwardDist = defaultRealignDist_;
    }
    setCurrentMovement(new MovementRealign(backwardDist, 
                                           finalDir, 
                                           gain, 
                                           maxSpeed,
                                           this));
}

// ----------------------------------------------------------------------------
// Move::rotateOnWheel
// ----------------------------------------------------------------------------
void MoveCL::rotateOnWheel(Radian      finalDir,
                           bool        stopLeftWheel,
                           MoveGain    gain,
                           MotorSpeed  maxSpeed,
                           bool        idleBlockedWheel)
{
    stop();
    if (gain == MOVE_USE_DEFAULT_GAIN) {
	gain = defaultRotationGain_;
    }
    if (maxSpeed == MOVE_USE_DEFAULT_SPEED) {
	maxSpeed = defaultMaxRotationSpeed_;
    }
    if (idleBlockedWheel) {
        // desasservi la roue blocquee
        if (stopLeftWheel) {
            idleMotorLeft();
        } else {
            idleMotorRight();
        }
    }
    setCurrentMovement(new MovementRotateOnWheel(stopLeftWheel,
                                                 finalDir, 
                                                 gain, 
                                                 maxSpeed,
                                                 this));
}

// ----------------------------------------------------------------------------
// MoveCL::rotateFromAngle
// ----------------------------------------------------------------------------
void MoveCL::rotateFromAngle(Radian      deltaTheta,
                           MoveGain    gain,
                           MotorSpeed  maxSpeed)
{
    stop();
    if (gain == MOVE_USE_DEFAULT_GAIN) {
	gain = defaultRotationGain_;
    }if (maxSpeed == MOVE_USE_DEFAULT_SPEED) {
	maxSpeed = defaultMaxRotationSpeed_;
    }
    setCurrentMovement(new MovementRotateFromAngle(deltaTheta, 
                                                   gain, 
                                                   maxSpeed,
                                                   this));
}

// ----------------------------------------------------------------------------
// Move::go2Target
// ----------------------------------------------------------------------------
void MoveCL::go2Target(Millimeter  x, 
                       Millimeter  y,
                       MoveGain    gain, 
                       MotorSpeed  maxSpeed,
                       bool        noRotation)
{
    go2Target(Point(x, y), gain, maxSpeed);
}

// ----------------------------------------------------------------------------
// MoveCL::go2Target
// ----------------------------------------------------------------------------
void MoveCL::go2Target(Point       pt, 
                       MoveGain    gain, 
                       MotorSpeed  maxSpeed,
                       bool        noRotation)
{
    stop();
    // rotation dans la bonne direction, puis asservissement sur un point final !
    Radian finalDir = dir(RobotPos->pt(), 
                          pt);
    MoveGain newGain=gain;
    MotorSpeed newMaxSpeed = maxSpeed/2;
    if (gain == MOVE_USE_DEFAULT_GAIN) {
	newGain = defaultRotationGain_;
    }if (maxSpeed == MOVE_USE_DEFAULT_SPEED) {
	newMaxSpeed = defaultMaxRotationSpeed_;
    }
    Movement * mvtRotation =NULL;
    if (!noRotation) {
        mvtRotation = new MovementRotate(finalDir, 
                                         newGain, 
                                         newMaxSpeed,
                                         this);
    }
    newGain=gain;
    newMaxSpeed = maxSpeed;
    if (gain == MOVE_USE_DEFAULT_GAIN) {
	newGain = defaultLinearGain_;
    }if (maxSpeed == MOVE_USE_DEFAULT_SPEED) {
	newMaxSpeed = defaultMaxLinearSpeed_;
    }
    Movement * mvtGotoTarget = new MovementGo2Target
                                     (pt, 
                                      newGain, 
                                      newMaxSpeed,
                                      this);
    if (!noRotation) {
        mvtRotation->registerNextMovement(mvtGotoTarget);
    }
    Trajectory t;
    t.push_back(RobotPos->pt());
    t.push_back(pt);
    Log->trajectory(t);
    if (!noRotation) {
        setCurrentMovement(mvtRotation);
    } else {
        setCurrentMovement(mvtGotoTarget);
    }
}

unsigned int trajectoryIndexOffset_;

// ----------------------------------------------------------------------------
// MoveCL::getTrajectoryCurrentIndex
// ----------------------------------------------------------------------------
unsigned int  MoveCL::getTrajectoryCurrentIndex() 
{
    return trajectoryCurrentIndex_ - trajectoryIndexOffset_;
}

// ----------------------------------------------------------------------------
// MoveCL::setTrajectoryCurrentIndex
// ----------------------------------------------------------------------------
void MoveCL::setTrajectoryCurrentIndex(unsigned int currentIndex) 
{
    trajectoryCurrentIndex_ = currentIndex+1;
}

// ----------------------------------------------------------------------------
// MoveCL::followTrajectory
// ----------------------------------------------------------------------------
void MoveCL::followTrajectory(Trajectory const&   trajectory2, 
                              MoveTrajectoryMode  mode,
                              MoveGain            gain, 
                              MotorSpeed          maxSpeed,
                              bool                noRotation)
{
    trajectoryIndexOffset_ = 0;
    // stop();
    // rotation dans la bonne direction, puis asservissement sur le premier point !
    Trajectory* trajectory;
    Trajectory trajectory3;
    if (trajectory2.empty()) {
        trajectory3.clear();
        trajectory3.push_back(RobotPos->pt());
        trajectory3.push_back(RobotPos->pt()+200.*Point(cos(RobotPos->theta()),
                                                         sin(RobotPos->theta()) ));
        trajectory=&trajectory3;
    } else {
        trajectory=const_cast<Trajectory*>(&trajectory2);
    }
    Movement* firstMvt=NULL;
    if (mode==TRAJECTORY_BASIC) {
      Movement* last=NULL;
      Trajectory::const_iterator it=trajectory->begin();
      Point previousPt=RobotPos->pt();
      for(;it!=trajectory->end();it++) {
	 Radian finalDir = dir(previousPt, (*it));
	 Movement* current; 

	 MoveGain newGain=gain;
	 MotorSpeed newMaxSpeed = maxSpeed/2;
	 if (gain == MOVE_USE_DEFAULT_GAIN) {
	     newGain = defaultRotationGain_;
	 }
	 if (maxSpeed == MOVE_USE_DEFAULT_SPEED) {
	     newMaxSpeed = defaultMaxRotationSpeed_;
	 }
	 current= new MovementRotate(finalDir, 
				     newGain, 
				     newMaxSpeed,
				     this);
	 if (last) last->registerNextMovement(current);
	 else firstMvt = current;
	 last=current;

	 newGain=gain;
	 newMaxSpeed=maxSpeed;
	 if (gain == MOVE_USE_DEFAULT_GAIN) {
	     newGain = defaultLinearGain_;
	 }if (maxSpeed == MOVE_USE_DEFAULT_SPEED) {
	     newMaxSpeed = defaultMaxLinearSpeed_;
	 }
	 current = new MovementGo2Target((*it), 
					 newGain, 
					 newMaxSpeed,
					 this);
	 if (last) last->registerNextMovement(current);
	 last=current;
	 previousPt = *it;
      }
    } else {
      Radian finalDir = 0;
      Trajectory t2;
      if (dist(RobotPos->pt(), (*trajectory)[0]) > 120
	  || trajectory->size() == 1) {
	  t2.push_back(RobotPos->pt());
          trajectoryIndexOffset_ = 1;
      }
      Trajectory::const_iterator it= trajectory->begin();
      for(;it != trajectory->end(); it++) {
	  t2.push_back(*it);
      }
      Point rotationTargetPoint;
      double rotationIndex=0;
      Millimeter length=0;
      do {    
	  rotationIndex += MVT_TRAJECTORY_DELTA_INDEX;  
	  if (mode == TRAJECTORY_SPLINE) {
	      Geometry2D::spline(t2, rotationIndex, 
				 rotationTargetPoint);
	  } else {
	      Geometry2D::rectilinear(t2, rotationIndex, 
				      rotationTargetPoint);
	  }
	  length = Geometry2D::getSquareDistance(RobotPos->pt(), rotationTargetPoint);
      }while ((length < MVT_TRAJECTORY_MIN_SQUARE_LENGTH)
	      && (rotationIndex < t2.size()-1));
      finalDir = dir(RobotPos->pt(), rotationTargetPoint);
      /*printf("rotation index=%lf %s %d\n", 
	rotationIndex, rotationTargetPoint.txt(), r2d(finalDir));*/
      Movement * mvtRotation=NULL;
      if (!noRotation) {

	  MoveGain newGain=gain;
	  MotorSpeed newMaxSpeed = maxSpeed/2;
	  if (gain == MOVE_USE_DEFAULT_GAIN) {
	      newGain = defaultRotationGain_;
	  }if (maxSpeed == MOVE_USE_DEFAULT_SPEED) {
	      newMaxSpeed = defaultMaxRotationSpeed_;
	  }
	  mvtRotation = new MovementRotate
	      (finalDir, 
	       newGain, 
	       maxSpeed,
	       this);
      }

      MoveGain newGain=gain;
      MotorSpeed newMaxSpeed = maxSpeed;
      if (gain == MOVE_USE_DEFAULT_GAIN) {
	  newGain = defaultLinearGain_;
      }if (maxSpeed == MOVE_USE_DEFAULT_SPEED) {
	  newMaxSpeed = defaultMaxLinearSpeed_;
      }
      Movement * mvtTrajectory = new MovementTrajectory
                                     (t2,
                                      mode,
                                      newGain, 
                                      newMaxSpeed,
                                      this);
      if (!noRotation) {
	  mvtRotation->registerNextMovement(mvtTrajectory);
	  firstMvt = mvtRotation;
      } else {
	  firstMvt = mvtTrajectory;
      }
    }

    Trajectory t;
    t.push_back(RobotPos->pt());
    Trajectory::const_iterator it=trajectory->begin();
    for(;it!=trajectory->end();it++) t.push_back(*it);
#ifdef LSM_TODO
    Trajectory t2;
    TrajecGen::reniceTrajectory2(t, t2);
    Log->trajectory(t2);
#endif
    setCurrentMovement(firstMvt);
}

// ----------------------------------------------------------------------------
// MoveCL::userAbort
// ----------------------------------------------------------------------------
void MoveCL::userAbort()
{
    LOG_FUNCTION();
    emergencyStop();
    userAbort_ = true;
}
