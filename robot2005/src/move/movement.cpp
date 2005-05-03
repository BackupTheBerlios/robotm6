#include <assert.h>

#include "movement.h"
#include "movementManager.h"
#include "geometry2D.h"
#include "log.h"
#include "robotTimer.h"
#include "robotConfig.h"
#define LOG_DEBUG_ON

// ============================================================================
// private namespace
// ============================================================================

namespace {
    const unsigned int MOVE_INTEGRAL_MEMORY_SIZE = 10;
    static const Millisecond MVT_TIMEOUT = 32000;
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

// ----------------------------------------------------------------------------
// Movement::Movement
// ----------------------------------------------------------------------------
Movement::Movement(MoveType    type, 
                   const char* Name,
                   MotorSpeed  maxSpeed,
                   MoveGain    gain,
                   MoveCL*     move):
    RobotBase("Movement", CLASS_MOVEMENT), 
    maxSpeed_(maxSpeed), gain_(gain),
    startTime_(TIME_INFINITE),
    move_(move), type_(type), nextMovement_(NULL)
{
    assert(move_);
    if (Name) {
        strncpy(name_, Name, MOVEMENT_NAME_LENGHT);
    }
    ++countMovementAllocated_;
    LOG_OK("Movement %s créé\n", name());
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
  //maxSpeed_=10;
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
bool Movement::moveToPoint(Point      target, 
                           MoveGain   gain,
                           Point      startingPoint, 
                           Millimeter maxSquareDist)
{
    MoveGain kl = gain;
    MoveGain kr = 5*gain;
    bool forward = (MvtMgr->getDirection() == MOVE_DIRECTION_FORWARD);
    // attractive points coordinates 
    Point attractivePoint = RobotPos->pt();
    attractivePoint.x += MOVE_ATTRACTIVE_DIST*cos(RobotPos->theta()
                                                  + (forward?0:M_PI));
    attractivePoint.y += MOVE_ATTRACTIVE_DIST*sin(RobotPos->theta()
                                                  + (forward?0:M_PI));
    //attractivePoint.print();
    
    // end ?= current point=target point 
    // or length from starting point > distance to final point 
    if (squareDist(attractivePoint, target) < MOVE_XY_SQUARE_EPSILON
        || (maxSquareDist>0 
            && (squareDist(startingPoint, RobotPos->pt()) > maxSquareDist))) {
        return true;
    }

    // compute speed to go to the target
    double linearSpeed    = (- (kl*(attractivePoint.x-target.x))*cos(RobotPos->theta())
                             - (kl*(attractivePoint.y-target.y))*sin(RobotPos->theta()));  
    double angularSpeed   =  (kr/POSITION_ROBOT_HCTL_D
                              * ((  attractivePoint.x-target.x)*sin(RobotPos->theta())
                                 - (attractivePoint.y-target.y)*cos(RobotPos->theta())));

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
    startingPoint_ = RobotPos->pt();
    MvtMgr->resetPwmAlert();
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
                                 MoveCL*    move) :
    Movement(MOVE_FORWARD_TYPE, "Forward", maxSpeed, MOVE_GAIN_DEFAULT, move), 
    squareDistance_(dist*dist)
{
   
}
    
// ----------------------------------------------------------------------------
// MovementForward::periodicTask
// ----------------------------------------------------------------------------
void MovementForward::periodicTask()
{
    Point currentPoint = RobotPos->pt();
    assert(!endOfMovement_);
    Millimeter sqDistToTarget = squareDistance_ - squareDist(startingPoint_, currentPoint);
    if (sqDistToTarget > 0) {
        double linearSpeed = maxSpeed_;
        if (sqDistToTarget < MOVE_LINEAR_REDUCE_SPEED_SQUARE_DIST) {
            linearSpeed *= sqDistToTarget/MOVE_LINEAR_REDUCE_SPEED_SQUARE_DIST;
        }
        if (MvtMgr->getDirection() == MOVE_DIRECTION_FORWARD) {
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
    Millimeter distToTarget = squareDistance_ - squareDist(startingPoint_, RobotPos->pt());
    snprintf(txt_, MOVEMENT_TXT_LENGHT,
             "%s: robot=%s, dist2target=%d mm",
             name(),
             RobotPos->txt(),
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
                                   MoveCL*      move) :
    Movement(MOVE_BACKWARD_TYPE, "Backward", maxSpeed, MOVE_GAIN_DEFAULT, move), 
    squareDistance_(dist*dist)
{
}
    
// ----------------------------------------------------------------------------
// MovementBackward::periodicTask
// ----------------------------------------------------------------------------
void MovementBackward::periodicTask()
{
    Point currentPoint = RobotPos->pt();
    assert(!endOfMovement_);
    Millimeter sqDistToTarget = squareDistance_ - squareDist(startingPoint_, currentPoint);
    if (sqDistToTarget > 0) {
        double linearSpeed = -maxSpeed_;
        if (sqDistToTarget < MOVE_LINEAR_REDUCE_SPEED_SQUARE_DIST) {
            linearSpeed *= sqDistToTarget/MOVE_LINEAR_REDUCE_SPEED_SQUARE_DIST;
        }
        if (MvtMgr->getDirection() == MOVE_DIRECTION_FORWARD) {
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
    Millimeter distToTarget = squareDistance_ 
                            - squareDist(startingPoint_, RobotPos->pt());
    snprintf(txt_, MOVEMENT_TXT_LENGHT,
             "%s: robot=%s, dist2target=%d mm",
             name(),
             RobotPos->txt(),
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
                               MoveCL*      move) :
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
    Radian error = theta_ - na2PI(RobotPos->theta(),
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
             RobotPos->txt(),
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
                                                 MoveCL*      move) :
    Movement(MOVE_ROTATE_FROM_ANGLE_TYPE,"RotateDelta", 
             max(maxSpeed, MOVE_MAX_ROTATION_SPEED), gain, move)
{
    theta_ = RobotPos->theta() + deltaTheta;
}
    
// ----------------------------------------------------------------------------
// MovementRotateFromAngle::periodicTask
// ----------------------------------------------------------------------------
void MovementRotateFromAngle::periodicTask()
{
    assert(!endOfMovement_);
    // compute the angle between current direction and target direction 
    Radian error = theta_ - RobotPos->theta();
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
             RobotPos->txt(),
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
                                     MoveCL*      move) :
    Movement(MOVE_GOTOTARGET_TYPE, "GotoTarget", maxSpeed, gain, move), 
    target_(target), totalSquareLength_(0), totalLengthInit_(false)
{
}

// ----------------------------------------------------------------------------
// MovementGo2Target::periodicTask
// ----------------------------------------------------------------------------
void MovementGo2Target::periodicTask()
{
    assert(!endOfMovement_);
    Millimeter sqDistToTarget = squareDist(target_, RobotPos->pt());
    Point moveTargetPoint   = target_;
    MoveGain gain           = gain_;
    if (!totalLengthInit_) {
        totalSquareLength_ = squareDist(target_, startingPoint_);
        totalLengthInit_ = true;
	target_=startingPoint_+(target_-startingPoint_)*(totalSquareLength_+MOVE_ATTRACTIVE_DIST*MOVE_ATTRACTIVE_DIST)/totalSquareLength_; // formule approximative mais bon c'est plus rapide a calculer
    }
    if (sqDistToTarget > MOVE_NEAR_TARGET_SQUARE_DIST) { 
        // on est loin de la cible => on trouve un point cible plus proche
        // et on baisse le gain
        gain /= 2.;
        moveTargetPoint = target_ + 
            (startingPoint_ - target_) 
	  * (sqDistToTarget - (MOVE_ATTRACTIVE_DIST * MOVE_ATTRACTIVE_DIST * 100.)) / totalSquareLength_;
    }
    // on va au point cible
    if (moveToPoint(moveTargetPoint, gain, startingPoint_, totalSquareLength_)) {
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
    Millimeter distToTarget = squareDist(target_, RobotPos->pt());
    snprintf(txt_, MOVEMENT_TXT_LENGHT,
             "%s: robot=%s, dist2Target=%d mm, currentTarget=%s",
             name(),
             RobotPos->txt(),
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
                                       MoveCL*             move) :
    Movement(MOVE_TRAJECTORY_TYPE, "Trajectory", maxSpeed, gain, move), 
    trajectory_(trajectory), trajectoryMode_(mode), 
    currentIndex_(0.), lastIndex_(0), totalSquareLength_(-1), lastSquareLength_(0),
    realMaxSpeed_(maxSpeed), first_(true)
{
    if (trajectory_.size() <= 1) {
        trajectory_.clear();
        trajectory_.push_back(RobotPos->pt());
        trajectory_.push_back(RobotPos->pt()+200.*Point(cos(RobotPos->theta()),
                                                        sin(RobotPos->theta()) ));
    }
    LOG_INFO("Trajectory\n");
    for(unsigned int i=0; i<trajectory_.size(); i++) {
      trajectory_[i].print();
    }

    // a virtual last point but stop the robot when arriving to the user
    // last point so the robot does not rotate on itself at the end of the
    // trajectory
    Point lastPoint2;
    if (trajectory_.size()>=2) {
        lastPoint2 = trajectory_[trajectory_.size()-2];
    } else {
        lastPoint2 = RobotPos->pt();
    }
    Point lastPoint = trajectory_[trajectory_.size()-1];
    Millimeter d = dist(lastPoint2, lastPoint); // do not set square dist
    lastPoint = lastPoint2+(lastPoint-lastPoint2)*((d+400.)/d);
    trajectory_.push_back(lastPoint);
    trajectory_[trajectory_.size()-1].print();
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
    Millimeter squareLength = squareDist(pt, newPoint);

    // if arriving near target point or distance to target point increase then pass to next point
    while (((fabs(lastIndex_-index) < MVT_TRAJECTORY_DELTA_INDEX
             && lastSquareLength_ < squareLength)
            || (squareLength < MVT_TRAJECTORY_MIN_SQUARE_LENGTH)) 
           && (index < maxIndex)) {    
        index += MVT_TRAJECTORY_DELTA_INDEX;  
        if (trajectoryMode_ == TRAJECTORY_SPLINE) {
            Geometry2D::spline(trajectory_, index, newPoint);
        } else {
            Geometry2D::rectilinear(trajectory_, index, newPoint);
        }
        squareLength = squareDist(pt, newPoint);
    }
    if ((int)lastIndex_ != (int)index) {
	startTime_ = Timer->time();
    }
    lastSquareLength_ = squareLength;
    lastIndex_  = index;

    if (index >= maxIndex) index = maxIndex;
    printf("nextPt= %.2lf %s\n", index, newPoint.txt());
    
}

// -------------------------------------------------------------------------
// MovementTrajectory::distToVirage
// -------------------------------------------------------------------------
Millimeter MovementTrajectory::distToVirage(int currentIndex)
{

    Radian alpha=0;
    int indexPremierVirage=max(0, currentIndex-1);
    Millimeter sqDistance=squareDist(trajectory_[indexPremierVirage], 
                                     RobotPos->pt());
    
    while((indexPremierVirage<(int)trajectory_.size()-2) 
	  && fabs(na2PI(alpha, -M_PI))< ANGLE_LIMIT_VIRAGE)
    {
	alpha =-dir(trajectory_[indexPremierVirage+1], 
                    trajectory_[indexPremierVirage+2])
	    +dir(trajectory_[indexPremierVirage], 
		 trajectory_[indexPremierVirage+1]);
	sqDistance+=squareDist(trajectory_[indexPremierVirage],
                             trajectory_[indexPremierVirage+1]);
	indexPremierVirage++;
    }
    if(indexPremierVirage >= (int)trajectory_.size()-2) {
	return squareDist(trajectory_[trajectory_.size()-1], RobotPos->pt());
    } else {
	return sqDistance;
    }
}


   

// -------------------------------------------------------------------------
// MovementTrajectory::periodicTask
// -------------------------------------------------------------------------
void MovementTrajectory::periodicTask()
{
    assert(!endOfMovement_);
    MoveGain gain = gain_;
    Point moveTargetPoint = RobotPos->pt();
    unsigned int maxIndex = (unsigned int)(trajectory_.size() - 2);
    if (trajectory_.size()<1) return;
    // trouve le point cible
    getNextPoint(moveTargetPoint, currentIndex_);
    move_->setTrajectoryCurrentIndex((unsigned int)currentIndex_);
    // si on est sur le dernier segment, on ne doit pas depasser le dernier 
    // point utilisateur le movement a rajouter un point bidon pour etre sur
    // de ne pas tourner en fin de trajectoire
    if ((unsigned int)currentIndex_ >= maxIndex && first_) {
         first_=false;
         beforeLastPoint_ = RobotPos->pt();
         totalSquareLength_ = 4*MOVE_ATTRACTIVE_DIST*MOVE_ATTRACTIVE_DIST; //dist(beforeLastPoint_, trajectory_[maxIndex]);
    }
    if (!first_) {
      gain_ *= 1-0.2*((squareDist(beforeLastPoint_, RobotPos->pt())) / (MOVE_ATTRACTIVE_DIST*MOVE_ATTRACTIVE_DIST));
    }
    Millimeter sqDistance=distToVirage((unsigned int)currentIndex_);
    if (sqDistance>160000) maxSpeed_ = realMaxSpeed_; //400 mm
    else if (sqDistance<10000) maxSpeed_ = VITESSE_MAX_VIRAGE; // 100 mm
    else maxSpeed_ = (MotorSpeed)(realMaxSpeed_-(realMaxSpeed_-20)*(160000.-sqDistance)/90000.); // 300
    //printf("%d\n", maxSpeed_);	
    // on va au point cible
    if (moveToPoint(moveTargetPoint, gain, beforeLastPoint_, totalSquareLength_)
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
             RobotPos->txt(),
             (float)currentIndex_,
             trajectory_[max(1+(int)currentIndex_, (int)(trajectory_.size() - 1))].txt());
    return txt_;       
}


// ============================================================================
// class MovementRotateOnWheel
// ============================================================================

// ----------------------------------------------------------------------------
// MovementRotateOnWheel::MovementRotateOnWheel
// ----------------------------------------------------------------------------
MovementRotateOnWheel::MovementRotateOnWheel(bool       stopLeftWheel,
                                             Radian     theta,
                                             MoveGain   gain,
                                             MotorSpeed maxSpeed,
                                             MoveCL*    move) :
    Movement(MOVE_ROTATE_ON_WHEEL_TYPE, "RotateOnWheel", 
             max(maxSpeed, MOVE_MAX_ROTATION_SPEED), gain, move), 
    theta_(theta), leftWheel_(stopLeftWheel)
{
}

// ----------------------------------------------------------------------------
// MovementRotateOnWheel::periodicTask
// ----------------------------------------------------------------------------
void MovementRotateOnWheel::periodicTask()
{
    assert(!endOfMovement_);
    // compute the angle between current direction and target direction 
    Radian error = theta_ - na2PI(RobotPos->theta(),
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
    
    double speedRight=0, speedLeft=0, speed=0;
    if (!endOfMovement_) {
        // Convertir (rotationSpeed) en (speedLeft, speedRight)
        speed =  POSITION_ROBOT_HCTL_D * angularSpeed / 5.;
        // Augmenter la consigne si elle est trop petite
        while ((fabs(angularSpeed) > 0) /* consigne non nulle*/
               && (fabs(speed) < (double)(MOVE_MIN_SPEED) )) {
            // la vitesse envoyee aux moteurs est trop faible
            speed *= 1.5;
        }
        // limiter la consigne si necessaire
        if (fabs(speed) > maxSpeed_) {
            speed = sign(speed) * maxSpeed_;
        }
        if (leftWheel_) {
            // la roue gauche ne doit pas bouger!
            speedRight =  speed;
            speedLeft  = 0;
        } else {
            // la roue droite ne doit pas bouger!
            speedRight = 0;
            speedLeft  = -speed;
        }
    }
    LOG_DEBUG("%d %d\n", (int)speedLeft, (int)speedRight);
    move_->setSpeed((MotorSpeed)speedLeft, 
                    (MotorSpeed)speedRight);

    if (Timer->time() > (startTime_+MVT_TIMEOUT))  stop();
}

// ----------------------------------------------------------------------------
// MovementRotateOnWheel::txt
// ----------------------------------------------------------------------------
char* MovementRotateOnWheel::txt()
{
    snprintf(txt_, MOVEMENT_TXT_LENGHT,
             "%s: robot=%s, target=%d deg",
             name(),
             RobotPos->txt(),
             r2d(theta_));
    return txt_;
}
// ============================================================================
// class MovementRealign
// ============================================================================

// ----------------------------------------------------------------------------
// MovementRealign::MovementRealign
// ----------------------------------------------------------------------------
MovementRealign::MovementRealign(Millimeter distMaxWheel,
                                 Radian     theta,
                                 MoveGain   gain,
                                 MotorSpeed maxSpeed,
                                 MoveCL*    move) :
    Movement(MOVE_REALIGN_TYPE, "MovementRealign", 
             max(maxSpeed, MOVE_MAX_ROTATION_SPEED), gain, move), 
    theta_(theta), leftWheel_(true), squareDistMax_(distMaxWheel*distMaxWheel)
{
    // est ce qu'on doit bloquer la roue gauche?
    leftWheel_ = (na2PI(theta-RobotPos->thetaAbsolute(), -M_PI)<0);
    blockedWheelPoint_ = getStopWheelPoint();
}

Point MovementRealign::getStopWheelPoint()
{
    Point pt = RobotPos->pt();
    Radian theta = RobotPos->thetaAbsolute() + (leftWheel_?(M_PI/2):(-M_PI/2));
    pt.x += RobotConfig->getMotorD()*cos(theta);
    pt.y += RobotConfig->getMotorD()*sin(theta);
    return pt;
}

// ----------------------------------------------------------------------------
// MovementRealign::periodicTask
// ----------------------------------------------------------------------------
void MovementRealign::periodicTask()
{
    assert(!endOfMovement_);
    // compute the angle between current direction and target direction 
    Radian error = theta_ - na2PI(RobotPos->theta(),
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
    Point wheelPoint = getStopWheelPoint();
    bool canMoveStopWheel = (squareDist(wheelPoint, blockedWheelPoint_) < squareDistMax_);
    double speedRight=0, speedLeft=0, speed=0;
    if (!endOfMovement_) {
        // Convertir (rotationSpeed) en (speedLeft, speedRight)
        speed =  POSITION_ROBOT_HCTL_D * angularSpeed / 5.;
        // Augmenter la consigne si elle est trop petite
        while ((fabs(angularSpeed) > 0) /* consigne non nulle*/
               && (fabs(speed) < (double)(MOVE_MIN_SPEED) )) {
            // la vitesse envoyee aux moteurs est trop faible
            speed *= 1.5;
        }
        // limiter la consigne si necessaire
        if (fabs(speed) > maxSpeed_) {
            speed = sign(speed) * maxSpeed_;
        }
        if (leftWheel_) {
            // la roue gauche ne doit pas bouger!
            speedRight =  speed;
            if (canMoveStopWheel) speedLeft  = speedLeft/2;
            else speedLeft  = 0;
            if (speed > 0) {
                stop();
                return;
            }
        } else {
            // la roue droite ne doit pas bouger!
            speedLeft  = -speed;
            if (canMoveStopWheel) speedRight = speedRight/2;
            else speedRight  = 0;
            if (speed > 0) {
                stop();
                return;
            }
        }
    }
    LOG_DEBUG("%d %d\n", (int)speedLeft, (int)speedRight);
    move_->setSpeed((MotorSpeed)speedLeft, 
                    (MotorSpeed)speedRight);

    if (Timer->time() > (startTime_+MVT_TIMEOUT))  stop();
}

// ----------------------------------------------------------------------------
// MovementRealign::txt
// ----------------------------------------------------------------------------
char* MovementRealign::txt()
{
    snprintf(txt_, MOVEMENT_TXT_LENGHT,
             "%s: robot=%s, target=%d deg",
             name(),
             RobotPos->txt(),
             r2d(theta_));
    return txt_;
}

// ----------------------------------------------------------------------------
// MovementSetSpeed::MovementSetSpeed
// ----------------------------------------------------------------------------
MovementSetSpeed::MovementSetSpeed(Millimeter dist,
                                   MotorSpeed speedLeft,
                                   MotorSpeed speedRight,
                                   MoveCL*    move) :
    Movement(MOVE_SET_SPEED_TYPE, "SetSpeed", max(abs(speedLeft), abs(speedRight)), 
             MOVE_GAIN_DEFAULT, move), 
    squareDistance_(dist*dist), speedLeft_(speedLeft), speedRight_(speedRight)
{
   
}
    
// ----------------------------------------------------------------------------
// MovementSetSpeed::periodicTask
// ----------------------------------------------------------------------------
void MovementSetSpeed::periodicTask()
{
    Point currentPoint = RobotPos->pt();
    assert(!endOfMovement_);
    Millimeter sqDistToTarget = squareDistance_ - squareDist(startingPoint_, currentPoint);
    if (sqDistToTarget > 0) {    
        move_->setSpeed((MotorSpeed)speedLeft_, 
                        (MotorSpeed)speedRight_);
    } else {
        stop();
	return;
    }
    if (Timer->time() > (startTime_+MVT_TIMEOUT))  stop();
}

// ----------------------------------------------------------------------------
// MovementSetSpeed::txt
// ----------------------------------------------------------------------------
char* MovementSetSpeed::txt()
{
    Millimeter sqDistToTarget = squareDistance_ - squareDist(startingPoint_, RobotPos->pt());
    snprintf(txt_, MOVEMENT_TXT_LENGHT,
             "%s: robot=%s, dist2target=%d mm",
             name(),
             RobotPos->txt(),
             (int)sqDistToTarget);
    return txt_;
}
