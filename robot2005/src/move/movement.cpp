#include <assert.h>

#include "movement.h"
#include "movementManager.h"
#include "geometry2D.h"
#include "log.h"
#include "robotTimer.h"

// ============================================================================
// private namespace
// ============================================================================

namespace {
    const unsigned int MOVE_INTEGRAL_MEMORY_SIZE = 10;
    static const Millisecond MVT_TIMEOUT = 10000;
    double sigmaError_=0;
    double integralMemory_[MOVE_INTEGRAL_MEMORY_SIZE];
    int integralMemoryIndex_=0;

    inline void clearIntegralTerm()
    {
        sigmaError_ = 0.;
        memset(integralMemory_, 0, 
               MOVE_INTEGRAL_MEMORY_SIZE*sizeof(double));
        integralMemoryIndex_=0;
    }

    inline void updateIntegralTerm(double newTerm)
    {
        sigmaError_ -= integralMemory_[(++integralMemoryIndex_) 
                                      % MOVE_INTEGRAL_MEMORY_SIZE];
        integralMemory_[(++integralMemoryIndex_) 
                       % MOVE_INTEGRAL_MEMORY_SIZE] = newTerm;
        sigmaError_ += integralMemory_[(++integralMemoryIndex_) 
                                      % MOVE_INTEGRAL_MEMORY_SIZE];
    }

    inline double getIntegralTerm()
    {
        return sigmaError_;
    }
}

// ============================================================================
// class Movement
// ============================================================================

bool Movement::endOfMovement_          = true;
int  Movement::countMovementAllocated_ = 0;
char Movement::txt_[MOVEMENT_TXT_LENGHT];

// TODO: [flo] INFINITE_TIME from 2004 has become???
#define INFINITE_TIME 0
// ----------------------------------------------------------------------------
// Movement::Movement
// ----------------------------------------------------------------------------
Movement::Movement(MoveType    type, 
                   const char* Name,
                   MotorSpeed  maxSpeed,
                   MoveGain    gain,
                   Move*       move):
    RobotBase("Movement", CLASS_MOVEMENT), 
    maxSpeed_(maxSpeed), gain_(gain),
    robotPosition_(NULL),  startTime_(INFINITE_TIME),
    type_(type), move_(move), nextMovement_(NULL)
{
    robotPosition_=ROBOT_POS;
    assert(robotPosition_);
    assert(move_);
    if (Name) {
        strncpy(name_, Name, MOVEMENT_NAME_LENGHT);
    }
    ++countMovementAllocated_;
    LOG_OK("Movement %s cr��\n", name());
}

// ----------------------------------------------------------------------------
// Movement::~Movement
// ----------------------------------------------------------------------------
Movement::~Movement()
{
    --countMovementAllocated_;
}

// ----------------------------------------------------------------------------
// Movement::name
// ----------------------------------------------------------------------------
const char* Movement::name()
{
  return name_;
}

// ----------------------------------------------------------------------------
// Movement::setLRSpeed
// ----------------------------------------------------------------------------
void Movement::setLRSpeed(MoveLinearSpeed   linearSpeed,
                          MoveRotationSpeed rotationSpeed)
{
    double speedRight=0, speedLeft=0;
    if (!endOfMovement_) {
        // Convertir (linearSpeed, rotationSpeed) en (speedLeft, speedRight)
        speedRight = (linearSpeed  
		      + POSITION_ROBOT_HCTL_D * rotationSpeed / 5.);
        speedLeft  = (linearSpeed 
		      - POSITION_ROBOT_HCTL_D * rotationSpeed / 5.);

        // Augmenter la consigne si elle est trop petite
        while ((fabs(linearSpeed) > 0 || fabs(rotationSpeed) > 0) /* consigne non nulle*/
               && (fabs(speedRight) < (double)(MOVE_MIN_SPEED) &&
                   fabs(speedLeft)  < (double)(MOVE_MIN_SPEED))) {
            // les 2 vitesse envoyees aux moteurs sont trop faibles
            speedRight *= 1.5;
            speedLeft  *= 1.5;
        }

        // limiter la consigne (proportionnellement sur les 2 roues) si necessaire
        if (fabs(speedRight) > maxSpeed_) {
            speedLeft  = speedLeft * (maxSpeed_/fabs(speedRight));      
            speedRight = sign(speedRight) * maxSpeed_;
        }
        if (fabs(speedLeft) > maxSpeed_) {
            speedRight = speedRight * (maxSpeed_/fabs(speedLeft));
            speedLeft  = sign(speedLeft) * maxSpeed_;
        }
    }
    LOG_DEBUG("%d %d\n", (int)speedLeft, (int)speedRight);
    move_->setSpeed((MotorSpeed)speedLeft, 
                    (MotorSpeed)speedRight);
}

// ----------------------------------------------------------------------------
// Movement::moveToPoint
// ----------------------------------------------------------------------------
bool Movement::moveToPoint(Point target, MoveGain gain,
                           Point startingPoint, Millimeter maxDist)
{
    MoveGain kl = gain;
    MoveGain kr = 5*gain;
    bool forward = (MVTMGR->getDirection() == MOVE_DIRECTION_FORWARD);
    // attractive points coordinates 
    Point attractivePoint = robotPosition_->pt();
    attractivePoint.x += MOVE_ATTRACTIVE_DIST*cos(robotPosition_->theta()
                                                  + (forward?0:M_PI));
    attractivePoint.y += MOVE_ATTRACTIVE_DIST*sin(robotPosition_->theta()
                                                  + (forward?0:M_PI));
    
    // end ?= current point=target point 
    // or length from starting point > distance to final point 
    if (dist(attractivePoint, target) < MOVE_XY_EPSILON
        || (maxDist>0 && (dist(startingPoint, robotPosition_->pt()) > maxDist))) {
        return true;
    }

    // compute speed to go to the target
    double linearSpeed    = (- (kl*(attractivePoint.x-target.x))*cos(robotPosition_->theta())
                             - (kl*(attractivePoint.y-target.y))*sin(robotPosition_->theta()));  
    double angularSpeed   =  (kr/POSITION_ROBOT_HCTL_D
                              * ((  attractivePoint.x-target.x)*sin(robotPosition_->theta())
                                 - (attractivePoint.y-target.y)*cos(robotPosition_->theta())));

    // il faut toujours avancer!
    if (linearSpeed < MOVE_MIN_SPEED) {
        linearSpeed = MOVE_MIN_SPEED;
    }

    setLRSpeed((forward?1:-1)*linearSpeed, angularSpeed);
    return false;
}

// ----------------------------------------------------------------------------
// Movement::registerNextMovement
// ----------------------------------------------------------------------------
void Movement::registerNextMovement(Movement* nextMovement)
{
    LOG_DEBUG("%s %d %d\n", __FUNCTION__, (int)this, (int)nextMovement);
    // delete current nextMovement if it exist
    unregisterNextMovement();
    // register new movement
    nextMovement_ = nextMovement;
}

// ----------------------------------------------------------------------------
// Movement::unregisterNextMovement
// ----------------------------------------------------------------------------
void Movement::unregisterNextMovement()
{
    if (nextMovement_) {
        nextMovement_->unregisterNextMovement();
	LOG_DEBUG("%s delete %d\n", __FUNCTION__, (int)nextMovement_);
        delete nextMovement_;
	nextMovement_=NULL;
    }
}

// ----------------------------------------------------------------------------
// Movement::start
// ----------------------------------------------------------------------------
void Movement::start()
{
    endOfMovement_ = false;
    LOG_INFO("Start movement %s\n", txt());
    startTime_=Timer->time();
    clearIntegralTerm();
    startingPoint_ = robotPosition_->pt();
    MVTMGR->resetPwmAlert();
}

// ----------------------------------------------------------------------------
// Movement::stop
// ----------------------------------------------------------------------------
void Movement::stop()
{
    if (!endOfMovement_) {
        endOfMovement_ = true;
        LOG_INFO("Stop movement %s %d\n", name(), (int)this);
        // lance le movement suivant
        move_->setCurrentMovement(nextMovement_, false);
    }
}


// ============================================================================
// class MovementForward
// ============================================================================

// ----------------------------------------------------------------------------
// MovementForward::MovementForward
// ----------------------------------------------------------------------------
MovementForward::MovementForward(Millimeter dist,
                                 MotorSpeed maxSpeed,
                                 Move*      move) :
    Movement(MOVE_FORWARD_TYPE, "Forward", maxSpeed, MOVE_GAIN_DEFAULT, move), 
    distance_(dist)
{
   
}
    
// ----------------------------------------------------------------------------
// MovementForward::periodicTask
// ----------------------------------------------------------------------------
void MovementForward::periodicTask()
{
    Point currentPoint = robotPosition_->pt();
    assert(!endOfMovement_);
    Millimeter distToTarget = distance_ - dist(startingPoint_, currentPoint);
    if (distToTarget > 0) {
        double linearSpeed = maxSpeed_;
        if (distToTarget < MOVE_LINEAR_REDUCE_SPEED_DIST) {
            linearSpeed *= distToTarget/MOVE_LINEAR_REDUCE_SPEED_DIST;
        }
        if (MVTMGR->getDirection() == MOVE_DIRECTION_FORWARD) {
            setLRSpeed(linearSpeed, 0);
        } else {
            setLRSpeed(-linearSpeed, 0);
        }
    } else {
        stop();
	return;
    }
    if (Timer->time() > (startTime_+MVT_TIMEOUT))  stop();
}

// ----------------------------------------------------------------------------
// MovementForward::txt
// ----------------------------------------------------------------------------
char* MovementForward::txt()
{
    Millimeter distToTarget = distance_ - dist(startingPoint_, robotPosition_->pt());
    snprintf(txt_, MOVEMENT_TXT_LENGHT,
             "%s: robot=%s, dist2target=%d mm",
             name(),
             robotPosition_->txt(),
             (int)distToTarget);
    return txt_;
}

// ============================================================================
// class MovementBackward
// ============================================================================

// ----------------------------------------------------------------------------
// MovementBackward::MovementBackward
// ----------------------------------------------------------------------------
MovementBackward::MovementBackward(Millimeter dist,
                                   MotorSpeed maxSpeed,
                                   Move*      move) :
    Movement(MOVE_BACKWARD_TYPE, "Backward", maxSpeed, MOVE_GAIN_DEFAULT, move), 
    distance_(dist)
{
}
    
// ----------------------------------------------------------------------------
// MovementBackward::periodicTask
// ----------------------------------------------------------------------------
void MovementBackward::periodicTask()
{
    Point currentPoint = robotPosition_->pt();
    assert(!endOfMovement_);
    Millimeter distToTarget = distance_ - dist(startingPoint_, currentPoint);
    if (distToTarget > 0) {
        double linearSpeed = -maxSpeed_;
        if (distToTarget < MOVE_LINEAR_REDUCE_SPEED_DIST) {
            linearSpeed *= distToTarget/MOVE_LINEAR_REDUCE_SPEED_DIST;
        }
        if (MVTMGR->getDirection() == MOVE_DIRECTION_FORWARD) {
            setLRSpeed(linearSpeed, 0);
        } else {
            setLRSpeed(-linearSpeed, 0);
        }
    } else {
        stop();
	return;
    }
    if (Timer->time() > (startTime_+MVT_TIMEOUT))  stop();
}

// ----------------------------------------------------------------------------
// MovementBackward::txt
// ----------------------------------------------------------------------------
char* MovementBackward::txt()
{
    Millimeter distToTarget = distance_ - dist(startingPoint_, robotPosition_->pt());
    snprintf(txt_, MOVEMENT_TXT_LENGHT,
             "%s: robot=%s, dist2target=%d mm",
             name(),
             robotPosition_->txt(),
             (int)distToTarget);
    return txt_;
}

// ============================================================================
// class MovementRotate
// ============================================================================

// ----------------------------------------------------------------------------
// MovementRotate::MovementRotate
// ----------------------------------------------------------------------------
MovementRotate::MovementRotate(Radian     theta,
                               MoveGain   gain,
                               MotorSpeed maxSpeed,
                               Move*      move) :
    Movement(MOVE_ROTATE_TYPE, "Rotate", 
             max(maxSpeed, MOVE_MAX_ROTATION_SPEED), gain, move), 
    theta_(theta)
{
}

// ----------------------------------------------------------------------------
// MovementRotate::periodicTask
// ----------------------------------------------------------------------------
void MovementRotate::periodicTask()
{
    assert(!endOfMovement_);
    // compute the angle between current direction and target direction 
    Radian error = theta_ - na2PI(robotPosition_->theta(),
				  theta_ - Geometry2D::MM_PI);
    // pour eviter une oscillation si error est proche de MM_PI, -MM_PI... 
    if (fabs(na2PI(error, -Geometry2D::MM_PI))>4.*Geometry2D::MM_PI/5.) {
	error = 4*Geometry2D::MM_PI/5.;
    }
    // end = current direction == target direction (+/-epsilon)
    if (fabs(error) < MOVE_ROTATION_EPSILON*gain_) {
        stop();
        return;
    }
   
    double angularSpeed = (gain_ / 2.5 * error)              // proportional
      + (MOVE_ROTATION_INTEGRAL_GAIN * getIntegralTerm()); // integral
   
    updateIntegralTerm(error);
    
   
    setLRSpeed(0, 1.3*angularSpeed);
    if (Timer->time() > (startTime_+MVT_TIMEOUT))  stop();
}

// ----------------------------------------------------------------------------
// MovementRotate::txt
// ----------------------------------------------------------------------------
char* MovementRotate::txt()
{
    snprintf(txt_, MOVEMENT_TXT_LENGHT,
             "%s: robot=%s, target=%d deg",
             name(),
             robotPosition_->txt(),
             r2d(theta_));
    return txt_;
}

// ============================================================================
// class MovementRotateFromAngle
// ============================================================================

// ----------------------------------------------------------------------------
// MovementRotateFromAngle::MovementRotateFromAngle
// ----------------------------------------------------------------------------
MovementRotateFromAngle::MovementRotateFromAngle(Radian     deltaTheta,
                                                 MoveGain   gain,
                                                 MotorSpeed maxSpeed,
                                                 Move*      move) :
    Movement(MOVE_ROTATE_FROM_ANGLE_TYPE,"RotateDelta", 
             max(maxSpeed, MOVE_MAX_ROTATION_SPEED), gain, move)
{
    theta_ = robotPosition_->theta() + deltaTheta;
}
    
// ----------------------------------------------------------------------------
// MovementRotateFromAngle::periodicTask
// ----------------------------------------------------------------------------
void MovementRotateFromAngle::periodicTask()
{
    assert(!endOfMovement_);
    // compute the angle between current direction and target direction 
    Radian error = theta_ - robotPosition_->theta();
    // end = current direction == target direction (+/-epsilon)
    if (fabs(error) < MOVE_ROTATION_EPSILON*gain_) {
        stop();
        return;
    }

    double angularSpeed = (gain_ / 2.5 * error)              // proportional
        + (MOVE_ROTATION_INTEGRAL_GAIN * getIntegralTerm()); // integral
   
    updateIntegralTerm(error);

    setLRSpeed(0, 1.3*angularSpeed);
    if (Timer->time() > (startTime_+MVT_TIMEOUT))  stop();
}

// ----------------------------------------------------------------------------
// MovementRotate::txt
// ----------------------------------------------------------------------------
char* MovementRotateFromAngle::txt()
{
    snprintf(txt_, MOVEMENT_TXT_LENGHT,
             "%s: robot=%s, target=%d deg",
             name(),
             robotPosition_->txt(),
             r2d(theta_));
    return txt_;
}

// ============================================================================
// class MovementGo2Target
// ============================================================================

// ----------------------------------------------------------------------------
// MovementGo2Target::MovementGo2Target
// ----------------------------------------------------------------------------
MovementGo2Target::MovementGo2Target(Point      target,
                                     MoveGain   gain,
                                     MotorSpeed maxSpeed,
                                     Move*      move) :
    Movement(MOVE_GOTOTARGET_TYPE, "GotoTarget", maxSpeed, gain, move), 
    target_(target), totalLength_(0), totalLengthInit_(false)
{
}

// ----------------------------------------------------------------------------
// MovementGo2Target::periodicTask
// ----------------------------------------------------------------------------
void MovementGo2Target::periodicTask()
{
    assert(!endOfMovement_);
    Millimeter distToTarget = dist(target_, robotPosition_->pt());
    Point moveTargetPoint   = target_;
    MoveGain gain           = gain_;
    if (!totalLengthInit_) {
        totalLength_ = dist(target_, startingPoint_);
        totalLengthInit_ = true;
    }
    if (distToTarget > MOVE_NEAR_TARGET_DIST) { 
        // on est loin de la cible => on trouve un point cible plus proche
        // et on baisse le gain
        gain /= 2.;
        moveTargetPoint = target_ + 
            (startingPoint_ - target_) 
            * (distToTarget - MOVE_ATTRACTIVE_DIST * 10.) / totalLength_;
    }
    // on va au point cible
    if (moveToPoint(moveTargetPoint, gain, startingPoint_, totalLength_)) {
        stop();
	return;
    }
    if (Timer->time() > (startTime_+MVT_TIMEOUT))  stop();
}

// ----------------------------------------------------------------------------
// MovementGo2Target::txt
// ----------------------------------------------------------------------------
char* MovementGo2Target::txt()
{
    Millimeter distToTarget = dist(target_, robotPosition_->pt());
    snprintf(txt_, MOVEMENT_TXT_LENGHT,
             "%s: robot=%s, dist2Target=%d mm, currentTarget=%s",
             name(),
             robotPosition_->txt(),
             (int)distToTarget,
             target_.txt());
    return txt_; 
}

// ============================================================================
// class MovementTrajectory
// ============================================================================

// ----------------------------------------------------------------------------
// MovementTrajectory::MovementTrajectory
// ----------------------------------------------------------------------------
MovementTrajectory::MovementTrajectory(Trajectory const&   trajectory, 
                                       MoveTrajectoryMode  mode,
                                       MoveGain            gain,
                                       MotorSpeed          maxSpeed,
                                       Move*               move) :
    Movement(MOVE_TRAJECTORY_TYPE, "Trajectory", maxSpeed, gain, move), 
    trajectory_(trajectory), trajectoryMode_(mode), 
    currentIndex_(0.), lastIndex_(0), totalLength_(-1), lastLength_(0),
    realMaxSpeed_(maxSpeed)
{
    if (trajectory_.size() <= 1) {
        trajectory_.clear();
        trajectory_.push_back(ROBOT_POS->pt());
        trajectory_.push_back(ROBOT_POS->pt()+200.*Point(cos(ROBOT_POS->theta()),
                                                         sin(ROBOT_POS->theta()) ));
    }
}

// ----------------------------------------------------------------------------
// MovementTrajectory::getNextPoint
// ----------------------------------------------------------------------------
void MovementTrajectory::getNextPoint(Point &newPoint, 
                                      double &index)
{
    Point pt = newPoint;
    double maxIndex = trajectory_.size()-1;
    if (trajectory_.size()<1) return;
    if (trajectoryMode_ == TRAJECTORY_SPLINE) {
        Geometry2D::spline(trajectory_, index, newPoint);
    } else {
        Geometry2D::rectilinear(trajectory_, index, newPoint);
    }
    Millimeter length = Geometry2D::getSquareDistance(pt, newPoint);

    // if arriving near target point or distance to target point increase then pas to next point
    while (((fabs(lastIndex_-index) < MVT_TRAJECTORY_DELTA_INDEX
             && lastLength_ < length)
            || (length < MVT_TRAJECTORY_MIN_SQUARE_LENGTH)) 
           && (index < maxIndex)) {    
        index += MVT_TRAJECTORY_DELTA_INDEX;  
        if (trajectoryMode_ == TRAJECTORY_SPLINE) {
            Geometry2D::spline(trajectory_, index, newPoint);
        } else {
            Geometry2D::rectilinear(trajectory_, index, newPoint);
        }
        length = Geometry2D::getSquareDistance(pt, newPoint);
    }
    if ((int)lastIndex_ != (int)index) {
	startTime_ = Timer->time();
    }
    lastLength_ = length;
    lastIndex_  = index;

    if (index >= maxIndex) index = maxIndex;
    //printf("nextPt= %.2lf %s\n", index, newPoint.txt());
    
}

// -------------------------------------------------------------------------
// MovementTrajectory::distToVirage
// -------------------------------------------------------------------------
Millimeter MovementTrajectory::distToVirage(int currentIndex)
{

    Radian alpha=0;
    int indexPremierVirage=max(0, currentIndex-1);
    Millimeter distance=dist(trajectory_[indexPremierVirage],ROBOT_POS->pt());
    
    while((indexPremierVirage<(int)trajectory_.size()-2) 
	  && fabs(na2PI(alpha, -M_PI))< ANGLE_LIMIT_VIRAGE)
    {
	alpha =-dir(trajectory_[indexPremierVirage+1], 
		   trajectory_[indexPremierVirage+2])
	    +dir(trajectory_[indexPremierVirage], 
		 trajectory_[indexPremierVirage+1]);
	distance+=dist(trajectory_[indexPremierVirage],
		       trajectory_[indexPremierVirage+1]);
	indexPremierVirage++;
    }
    /*printf("%d %d %d\n", 
	   indexPremierVirage, 
	   trajectory_.size(), 
	   (int)distance);*/
    if(indexPremierVirage >= (int)trajectory_.size()-2) {
	return dist(trajectory_[trajectory_.size()-1],ROBOT_POS->pt());
    } else {
	return distance;
    }
}


   

// -------------------------------------------------------------------------
// MovementTrajectory::periodicTask
// -------------------------------------------------------------------------
void MovementTrajectory::periodicTask()
{
    assert(!endOfMovement_);
    MoveGain gain = gain_;
    Point moveTargetPoint = robotPosition_->pt();
    unsigned int lastIndex = (unsigned int)currentIndex_;
    unsigned int maxIndex = (unsigned int)(trajectory_.size() - 1);
    if (trajectory_.size()<1) return;
    // trouve le point cible
    getNextPoint(moveTargetPoint, currentIndex_);

    // si on est sur le dernier segment, on ne doit pas depasser le dernier point
    if ((unsigned int)currentIndex_ == maxIndex
       && lastIndex != currentIndex_) {
         beforeLastPoint_ = robotPosition_->pt();
         totalLength_ = dist(beforeLastPoint_, trajectory_[maxIndex]);
    }
    
    Millimeter distance=distToVirage((unsigned int)currentIndex_);
    if (distance>400) maxSpeed_ = realMaxSpeed_;
    else if (distance<100) maxSpeed_ = VITESSE_MAX_VIRAGE;
    else maxSpeed_ = (MotorSpeed)(realMaxSpeed_-(realMaxSpeed_-20)*(400.-distance)/300.);
    //printf("%d\n", maxSpeed_);	
    // on va au point cible
    if (moveToPoint(moveTargetPoint, gain, beforeLastPoint_, totalLength_)
        && (currentIndex_ >= maxIndex)) {
        stop();
	return;
    }
    if (Timer->time() > (startTime_+MVT_TIMEOUT))  stop();
}

// ----------------------------------------------------------------------------
// MovementTrajectory::txt
// ----------------------------------------------------------------------------
char* MovementTrajectory::txt()
{
    snprintf(txt_, MOVEMENT_TXT_LENGHT,
             "%s: robot=%s, index=%f, currentTarget=%s",
             name(),
             robotPosition_->txt(),
             (float)currentIndex_,
             trajectory_[max(1+(int)currentIndex_, (int)(trajectory_.size() - 1))].txt());
    return txt_;       
}

