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
Move* Move::move_=NULL;

// ----------------------------------------------------------------------------
// Move::Move
// ----------------------------------------------------------------------------
Move::Move() : 
    RobotComponent("Move", CLASS_MOVE), currentMovement_(NULL),
    enableAccelerationController_(false), userAbort_(false),
    defaultRotationGain_(MOVE_GAIN_DEFAULT),
    defaultLinearGain_(MOVE_GAIN_DEFAULT),
    defaultMaxRotationSpeed_(MOVE_MAX_ROTATION_SPEED),
    defaultMaxLinearSpeed_(MOVE_SPEED_DEFAULT),
    defaultBasicSpeed_(MOVE_SPEED_DEFAULT)
{
    pthread_mutex_init(&repositoryLock, NULL);
    assert(move_ == NULL);
    move_ = this;
    init_ = true;
    LOG_OK("Initialisation Termin�e\n");
}

// ----------------------------------------------------------------------------
// Move::~Move
// ----------------------------------------------------------------------------
Move::~Move()
{
    if (currentMovement_) {
        delete currentMovement_;
        currentMovement_ = NULL;
    }
}

// ----------------------------------------------------------------------------
// Move::instance
// ----------------------------------------------------------------------------
Move* Move::instance()
{
    assert(move_);
    return move_;
}

// ----------------------------------------------------------------------------
// Move::periodicTask
// ----------------------------------------------------------------------------
void Move::periodicTask(Millisecond time)
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
// Move::reset
// ----------------------------------------------------------------------------
bool Move::reset()
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
// Move::validate
// ----------------------------------------------------------------------------
bool Move::validate()
{
    return true;
}

// ----------------------------------------------------------------------------
// Move::calibrate
// ----------------------------------------------------------------------------
void Move::calibrate()
{
#if 0
    int c = 0;
    Position* robotPosition = Position::get();
    Millisecond time=0;

    printf("Pressez la touche 'entree' pour que le robot avance de 2 m\n");
    c = getc(stdin);
    // on avance de 2 metres
    robotPosition->set(0, 0, 0);
    forward(2000, MOVE_SPEED_SLOW);
    while (currentMovement_) {
        robotPosition->periodicTask(time);
        this->periodicTask(time);
        usleep(100);
    }
    // on attend un peu
    c=0;
    while (c++<1000) {
        robotPosition->periodicTask(time);
        this->periodicTask(time);
        usleep(1000);
    }
    // on trouve une meilleure valeur pour K
    int realDist=0;
    printf("Mesurez la distance reellement parcourrue et entrez la (sous forme entiere):\n");
    scanf("%d", &realDist);
    double newK = realDist/norme(robotPosition->pt)*POSITION_ROBOT_K;
    printf("Une meilleure valeur pour POSITION_ROBOT_K serait : %f, ancienne valeur: %f\n", 
           (float)newK,
           (float)POSITION_ROBOT_K);
    
    // trouver la valeur de POSITION_ROBOT_D
    printf("Pressez la touche 'entree' pour que le robot fasse 2 tours sur lui meme\n");
    c = getc(stdin);
    robotPosition->set(0, 0, 0);
    rotateFromAngle(4*M_PI, MOVE_GAIN_SLOW, MOVE_SPEED_SLOW);
    while (currentMovement_) {
        robotPosition->periodicTask(time);
        this->periodicTask(time);
        usleep(100);
    }
    // on attend un peu
    c=0;
    while (c++<1000) {
        robotPosition->periodicTask(time);
        this->periodicTask(time);
        usleep(1000);
    }
    // on trouve une meilleure valeur pour K
    int realDir=0;
    printf("Entrez la direction du robot en degre (0..360)(sous forme entiere, dans le sens trigonometrique):\n");
    scanf("%d", &realDir);
    if (realDir<180) realDir+=720;
    else realDir+=360;
    double newD = (realDir*M_PI/360.)/(robotPosition->theta)*POSITION_ROBOT_D;
    printf("Une meilleure valeur pour POSITION_ROBOT_D serait : %f, ancienne valeur: %f\n", 
           (float)newD,
           (float)POSITION_ROBOT_D);

    printf("Corrigez les valeurs dans position.h, recompiler et recommencer...\n");
#endif
}

// ----------------------------------------------------------------------------
// Move::enableAccelerationController
// ----------------------------------------------------------------------------
void Move::enableAccelerationController(bool enable)
{
    enableAccelerationController_ = enable;
}

// ----------------------------------------------------------------------------
// Move::setSpeed
// ----------------------------------------------------------------------------
MotorSpeed Move::lastSpeedLeft_ =0;
MotorSpeed Move::lastSpeedRight_=0;
MotorSpeed Move::lastSpeedReqLeft_ =0;
MotorSpeed Move::lastSpeedReqRight_=0;

void Move::setSpeed(MotorSpeed  speedLeft,
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
   
    MVTMGR->setSpeed(speedLeft, speedRight);
    lastSpeedLeft_  = speedLeft;
    lastSpeedRight_ = speedRight;
}

// ----------------------------------------------------------------------------
// Move::emergencyStop
// ----------------------------------------------------------------------------
void Move::emergencyStop()
{
    stop();
    lastSpeedLeft_    = 0;
    lastSpeedRight_   = 0;
    lastSpeedReqLeft_ = 0;
    lastSpeedReqRight_= 0;
    MVTMGR->setSpeed(0, 0);
}

// ----------------------------------------------------------------------------
// Move::setCurrentMovement
// ----------------------------------------------------------------------------
void Move::setCurrentMovement(Movement* mvt, bool needLock)
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
// Move::stop
// ----------------------------------------------------------------------------
void Move::stop()
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
// Move::forward
// ----------------------------------------------------------------------------
void Move::forward(Millimeter  dist, 
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
// Move::backward
// ----------------------------------------------------------------------------
void Move::backward(Millimeter  dist, 
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
// Move::rotate
// ----------------------------------------------------------------------------
void Move::rotate(Radian      finalDir,
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
// Move::rotateFromAngle
// ----------------------------------------------------------------------------
void Move::rotateFromAngle(Radian      deltaTheta,
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
void Move::go2Target(Millimeter  x, 
                     Millimeter  y,
                     MoveGain    gain, 
                     MotorSpeed  maxSpeed)
{
    go2Target(Point(x, y), gain, maxSpeed);
}

// ----------------------------------------------------------------------------
// Move::go2Target
// ----------------------------------------------------------------------------
void Move::go2Target(Point       pt, 
                     MoveGain    gain, 
                     MotorSpeed  maxSpeed)
{
    stop();
    // rotation dans la bonne direction, puis asservissement sur un point final !
    Radian finalDir = dir(ROBOT_POS->pt(), 
                          pt);
    MoveGain newGain=gain;
    MotorSpeed newMaxSpeed = maxSpeed/2;
    if (gain == MOVE_USE_DEFAULT_GAIN) {
	newGain = defaultRotationGain_;
    }if (maxSpeed == MOVE_USE_DEFAULT_SPEED) {
	newMaxSpeed = defaultMaxRotationSpeed_;
    }
    Movement * mvtRotation = new MovementRotate
                                   (finalDir, 
                                    newGain, 
                                    newMaxSpeed,
                                    this);

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
    mvtRotation->registerNextMovement(mvtGotoTarget);

    Trajectory t;
    t.push_back(ROBOT_POS->pt());
    t.push_back(pt);
    Log->trajectory(t);
    setCurrentMovement(mvtRotation);
}

// ----------------------------------------------------------------------------
// Move::followTrajectory
// ----------------------------------------------------------------------------
void Move::followTrajectory(Trajectory const&   trajectory2, 
                            MoveTrajectoryMode  mode,
                            MoveGain            gain, 
                            MotorSpeed          maxSpeed,
			    bool                noRotation)
{
    // stop();
    // rotation dans la bonne direction, puis asservissement sur le premier point !
    Trajectory* trajectory;
    Trajectory trajectory3;
    if (trajectory2.empty()) {
        trajectory3.clear();
        trajectory3.push_back(ROBOT_POS->pt());
        trajectory3.push_back(ROBOT_POS->pt()+200.*Point(cos(ROBOT_POS->theta()),
                                                         sin(ROBOT_POS->theta()) ));
        trajectory=&trajectory3;
    } else {
        trajectory=const_cast<Trajectory*>(&trajectory2);
    }
    Movement* firstMvt=NULL;
    if (mode==TRAJECTORY_BASIC) {
      Movement* last=NULL;
      Trajectory::const_iterator it=trajectory->begin();
      Point previousPt=ROBOT_POS->pt();
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
      if (dist(ROBOT_POS->pt(), (*trajectory)[0]) > 120
	  || trajectory->size() == 1) {
	  t2.push_back(ROBOT_POS->pt());
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
	  length = Geometry2D::getSquareDistance(ROBOT_POS->pt(), rotationTargetPoint);
      }while ((length < MVT_TRAJECTORY_MIN_SQUARE_LENGTH)
	      && (rotationIndex < t2.size()-1));
      finalDir = dir(ROBOT_POS->pt(), rotationTargetPoint);
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
    t.push_back(ROBOT_POS->pt());
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
// Move::userAbort
// ----------------------------------------------------------------------------
void Move::userAbort()
{
  LOG_FUNCTION();
  emergencyStop();
  userAbort_ = true;
}