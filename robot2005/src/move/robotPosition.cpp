#include <stdio.h>
#include <assert.h>

#include "robotPosition.h"
#include "log.h"
#include "movementManager.h"
#include "odometer.h"
//#include "sound.h" TODO: 2004? [flo]
#include "robotTimer.h"
#include "events.h"
#include "geometry2D.h"
#include "simulatorClient.h"
#include "robotConfig.h"

// ----------------------------------------------------------------------------
// Static members
// ----------------------------------------------------------------------------
RobotPosition* RobotPosition::position_=NULL;

// ----------------------------------------------------------------------------
// Position::Position
// ----------------------------------------------------------------------------
RobotPosition::RobotPosition():
    RobotComponent("Position", CLASS_ROBOT_POSITION), 
    rightHctlOld_(0), leftHctlOld_(0), firstHctl_(true), 
    rightOdomOld_(0), leftOdomOld_(0), firstOdom_(true),
    txtChanged_(true),
    odometerType_(ODOMETER_MOTOR), odometerIsAutomatic_(false),
    odometerErrorCount_(0),
    enableOdoHtclColliDetec_(true),
    oldPosHctl_(), oldPosOdom_(), sumDrOdom_(0), sumDrHctl_(0),
    sumDpOdom_(0), sumDpHctl_(0), timeToCheckPatinage_(0), firstSum_(true)
{
    assert(position_ == NULL);
    position_=this;
    reset();

    init_ = true;
    LOG_OK("Initialisation Terminée\n");
}

// ----------------------------------------------------------------------------
// RobotPosition::~RobotPosition
// ----------------------------------------------------------------------------
RobotPosition::~RobotPosition()
{
    position_=NULL;
}

// ----------------------------------------------------------------------------
// RobotPosition::txt
// ----------------------------------------------------------------------------
char* RobotPosition::txt()
{
    if (txtChanged_) {
        snprintf(txt_, POSITION_TEXT_LENGTH, "x=%dmm, y=%d mm, t=%ddeg", 
                 (int)x(), (int)y(), r2d(theta()));
        txtChanged_ = false;
    }
    return txt_;
}

// ----------------------------------------------------------------------------
// RobotPosition::set
// ----------------------------------------------------------------------------
void RobotPosition::set(Millimeter X, 
			Millimeter Y, 
			Radian T)
{
    pos_.center.x  = X;
    pos_.center.y  = Y;
    pos_.direction = T;
    posOdom_       = pos_;
    posHctl_       = pos_;
    clearOdoColliDetectBuffer();
    txtChanged_    = true;
    clearBufferPosition();
    Simulator->setRobotPosition(pos_);
}

// ----------------------------------------------------------------------------
// RobotPosition::theta
// ----------------------------------------------------------------------------
Radian RobotPosition::theta() const
{
    return pos_.direction 
        + (MVTMGR->getDirection() == MOVE_DIRECTION_FORWARD?0:M_PI);
}

// ----------------------------------------------------------------------------
// RobotPosition::clearBufferPosition
// ----------------------------------------------------------------------------
void RobotPosition::clearBufferPosition()
{
    for(int i=0; i < POS_BUFFER_SIZE; i++) {
	posBuf_[i]=pos_;
    }
    posBufIndex_=0;
}

// ----------------------------------------------------------------------------
// RobotPosition::addPositionToBuffer
// ----------------------------------------------------------------------------
void RobotPosition::addPositionToBuffer()
{
    posBuf_[posBufIndex_] = pos_;
    posBufIndex_=(posBufIndex_+1)%POS_BUFFER_SIZE;
}

// ----------------------------------------------------------------------------
// RobotPosition::getPosition
// ----------------------------------------------------------------------------
Position RobotPosition::getPosition(Millisecond deltaT)
{
    int delta=(int)(deltaT/POS_BUFFER_STEP_TIME);
    int index= (POS_BUFFER_SIZE+posBufIndex_-delta)%POS_BUFFER_SIZE;
    return posBuf_[index];
}

// ----------------------------------------------------------------------------
// RobotPosition::setOdometerType
// ----------------------------------------------------------------------------
// Definit le type d'odometre a utiliser
// ----------------------------------------------------------------------------
void RobotPosition::setOdometerType(OdometerType odometer)
{
    odometerType_ = odometer;
    if (odometerType_ == ODOMETER_UART_AUTOMATIC) {
        odometerErrorCount_ = 0;
	Odometer->setMode(ODOMETER_AUTOMATIC);
        LOG_WARNING("Set odometer mode: UART_AUTOMATIC\n");
        odometerIsAutomatic_ = true;
        odometerErrorCount_ = 0;
    } else {
        LOG_WARNING("Set odometer mode: MOTOR\n");
        Odometer->setMode(ODOMETER_MANUAL);
        odometerIsAutomatic_ = false;
	odometerErrorCount_ = 0;
    }
}
    
// ----------------------------------------------------------------------------
// RobotPosition::getOdometerType
// ----------------------------------------------------------------------------
// Retourne le type d'odometre utilise
// ----------------------------------------------------------------------------
OdometerType RobotPosition::getOdometerType() const
{
    return odometerType_;
}

// ----------------------------------------------------------------------------
// RobotPosition::print
// ----------------------------------------------------------------------------
void RobotPosition::print() 
{
    printf("Robot Position:%s\n", txt());
}

// ----------------------------------------------------------------------------
// RobotPosition::reset
// ----------------------------------------------------------------------------
bool RobotPosition::reset()
{
    LOG_FUNCTION();
    resetHctlCoders();
    resetOdomCoders();
    // TODO: I would prefer that RobotPosition doesn't know if there are several robots...
    // (for instance put the initial-pos into robotConfig).
    if (RobotConfig->isRobotAttack)
	set(ROBOT_A_INIT_X, ROBOT_A_INIT_Y, ROBOT_A_INIT_THETA);
    else
	set(ROBOT_D_INIT_X, ROBOT_D_INIT_Y, ROBOT_D_INIT_THETA);
    odometerType_ = ODOMETER_MOTOR; 
    odometerIsAutomatic_ = false;
    odometerErrorCount_ = 0;
    resetOdoColliDetection();
    clearBufferPosition();
    return true;
}

// ----------------------------------------------------------------------------
// RobotPosition::validate
// ----------------------------------------------------------------------------
bool RobotPosition::validate()
{
    // not tested
    return false;
}

// ----------------------------------------------------------------------------
// RobotPosition::getPosition
// ----------------------------------------------------------------------------
void RobotPosition::getPosition(Position&      posi,
                                CoderPosition  leftPos, 
                                CoderPosition  rightPos,
                                CoderPosition& leftPosOld, 
                                CoderPosition& rightPosOld,
                                Millimeter     KLeft,
                                Millimeter     KRight,
                                Millimeter     D,// distance entre les 2 codeurs
                                bool           first)
{
    static int counter=0;
    //printf("c=%d l=%d r=%d\n", counter, leftPosOld, rightPosOld);
    if (counter-- == 0) {
	leftPosOld  = leftPos;
        rightPosOld = rightPos;
    } else {
	if (counter<0) counter=-1;
    }
    if (first) {
	counter=5; // avant la valeur des codeurs change...
        // on a rester les codeurs, on fait comme si on n'avait pas bouger car les 
        // valeurs ***Old sont invalides
    } else if (counter<0) {
        // verifie qu'il n'y a pas un passage de -MAX a +MAX (overflow)
        if (leftPos - leftPosOld > SHRT_MAX) { // short max =unsigned short max /2
            leftPosOld += USHRT_MAX;   // unsigned short max
        } else if (leftPos - leftPosOld < SHRT_MIN) {
            leftPosOld -= USHRT_MAX;
        }
        if (rightPos - rightPosOld > SHRT_MAX) {
            rightPosOld += USHRT_MAX;
        } else if (rightPos - rightPosOld < SHRT_MIN) {
            rightPosOld -= USHRT_MAX;
        }

    Millimeter deltaKRight = KRight * (rightPos - rightPosOld);
    Millimeter deltaKLeft  = KLeft  * (leftPos  - leftPosOld);
    rightPosOld = rightPos;
    leftPosOld  = leftPos;

    // Calcul de la direction du robot
    Radian oldTheta   = posi.direction;
    Radian deltaTheta = Radian((deltaKRight - deltaKLeft) / D);
    posi.direction   += deltaTheta;

    // Calcul de la position du robot
    Millimeter deltaSum   = (deltaKRight + deltaKLeft) / 2.;
    if (fabs(deltaTheta) > 0.0001) {
        // Le robot tourne
        posi.center.x += (Millimeter)( deltaSum * (sin(posi.direction) - sin(oldTheta)) / deltaTheta);
        posi.center.y += (Millimeter)(-deltaSum * (cos(posi.direction) - cos(oldTheta)) / deltaTheta);
      /*
      double tempR = deltaSum/deltaTheta;
      posi.center.x += tempR*(1-cos(deltaTheta))*cos(oldTheta_) 
 	             - tempR * sin(deltaTheta) * sin (oldTheta_);
      posi.center.y += tempR*(1-cos(deltaTheta))*sin(oldTheta_) 
              	     + tempR * sin(deltaTheta) * cos (oldTheta_);
      */
    } else {
        // Le robot va tout droit
        deltaTheta = (posi.direction + oldTheta) / 2.;
        posi.center.x += deltaSum * cos(deltaTheta);  
        posi.center.y += deltaSum * sin(deltaTheta);
	// printf("voo %lf %lf %lf \n",deltaSum, deltaKRight, deltaKLeft );
    }
    }
}

// ----------------------------------------------------------------------------
// RobotPosition::updateHctlPosition
// ----------------------------------------------------------------------------
void RobotPosition::updateHctlPosition()
{
    CoderPosition left=0, right=0;
    MVTMGR->getCoderPosition(left, right);

    getPosition(posHctl_,
                left, 
                right,
                leftHctlOld_, 
                rightHctlOld_,
                POSITION_CODER_HCTL_SIGN_LEFT  * POSITION_ROBOT_HCTL_K * POSITION_ROBOT_HCTL_Cl,
                POSITION_CODER_HCTL_SIGN_RIGHT * POSITION_ROBOT_HCTL_K * POSITION_ROBOT_HCTL_Cr,
                POSITION_ROBOT_HCTL_D*POSITION_ROBOT_HCTL_Eb,
                firstHctl_);
    firstHctl_    = false;
}

// ----------------------------------------------------------------------------
// RobotPosition::isOdometerAlive
// ----------------------------------------------------------------------------
bool RobotPosition::isOdometerAlive()
{
    bool result = (odometerErrorCount_ < ODOMETER_ERROR_MAX);
    odometerErrorCount_++;
    return result;
}

// ----------------------------------------------------------------------------
// RobotPosition::setOdometerAliveStatus
// ----------------------------------------------------------------------------
void RobotPosition::setOdometerAliveStatus(bool alive)
{
    if (alive) {
        odometerErrorCount_ = 0;
    } else {
        odometerErrorCount_ = ODOMETER_ERROR_MAX;
    }
}

// ----------------------------------------------------------------------------
// RobotPosition::updateOdometerPosition
// ----------------------------------------------------------------------------
void RobotPosition::updateOdometerPosition()
{
    CoderPosition left=0, right=0;
    //printf("updateOdometerPosition\n");
    if (!Odometer->getCoderPosition(left, right)) {
        setOdometerAliveStatus(false);
        return;
    } else {
        setOdometerAliveStatus(true);
    }
    Position oldPos=posOdom_;
    getPosition(posOdom_,
                left, 
                right,
                leftOdomOld_, 
                rightOdomOld_,
                POSITION_CODER_ODOM_SIGN_LEFT  * POSITION_ROBOT_ODOM_K * POSITION_ROBOT_ODOM_Cl,
                POSITION_CODER_ODOM_SIGN_RIGHT * POSITION_ROBOT_ODOM_K * POSITION_ROBOT_ODOM_Cr,
                POSITION_ROBOT_ODOM_D * POSITION_ROBOT_ODOM_Eb,
                firstOdom_);

    if (!firstOdom_) {
	if ((fabs(oldPos.center.x - posOdom_.center.x) > 200)
	    || (fabs(oldPos.center.y - posOdom_.center.y) > 200)
	    || (fabs(na2PI(oldPos.direction - posOdom_.direction, -M_PI)) 
		> M_PI/2)) {
	    posOdom_ = oldPos;
	}
    }

    firstOdom_ = false;
}

// ----------------------------------------------------------------------------
// RobotPosition::enableColliDetectionWithOdometer
// ----------------------------------------------------------------------------
void RobotPosition::enableColliDetectionWithOdometer(bool enable)
{
    enableOdoHtclColliDetec_ = enable;
    resetOdoColliDetection();
    LOG_COMMAND("enableColliDetectionWithOdometer(%s)\n", b2s(enable));
}

// ----------------------------------------------------------------------------
// RobotPosition::clearOdoColliDetectBuffer
// ----------------------------------------------------------------------------
void RobotPosition::clearOdoColliDetectBuffer()
{
    LOG_FUNCTION();
    oldPosHctl_=posHctl_;
    oldPosOdom_=posOdom_;
    for(int i=0; i < ODOM_COLLI_DETECT_BUFFER_SIZE; i++) {
	drHctl_[i] = 0;
	drOdom_[i] = 0;
	dpHctl_[i] = 0;
	dpOdom_[i] = 0;
    }
    sumDrOdom_=0;
    sumDrHctl_=0;
    sumDpOdom_=0;
    sumDpHctl_=0;
}

void RobotPosition::resetOdoColliDetection()
{
    firstSum_=true;
    timeToCheckPatinage_=Timer->time()+300;
}
// ----------------------------------------------------------------------------
// RobotPosition::detectCollision
// ----------------------------------------------------------------------------
// compare la position des hctl et des odometres et si les odometres ne bougent
// pas alors que les hctl oui, on envoie le signal EVENTS_PWM qui est
// interprete comme un probleme de deplacement
// ----------------------------------------------------------------------------
void RobotPosition::detectCollision()
{
    if (odometerType_ == ODOMETER_MOTOR 
       || !enableOdoHtclColliDetec_) return;
    if (firstSum_) {
      clearOdoColliDetectBuffer();
      firstSum_=false;
      return;
    }
    static int index=0;
    dpHctl_[index] = dist(posHctl_.center, oldPosHctl_.center);
    drHctl_[index] = fabs(posHctl_.direction-oldPosHctl_.direction);
    dpOdom_[index] = dist(posOdom_.center, oldPosOdom_.center);
    drOdom_[index] = fabs(na2PI(posOdom_.direction-oldPosOdom_.direction,
				-M_PI));
    
    sumDrOdom_ += drOdom_[index];
    sumDrHctl_ += drHctl_[index];
    sumDpOdom_ += dpOdom_[index];
    sumDpHctl_ += dpHctl_[index];

    LOG_DEBUG("drO=%dd drH=%dd dpO=%dmm dpH=%dmm\n",
	      r2d(sumDrOdom_), r2d(sumDrHctl_), 
	      (int)sumDpOdom_, (int)sumDpHctl_);
    LOG_DEBUG("rO=%dd rH=%dd pO=%dmm pH=%dmm\n",
	      r2d(drOdom_[index]), r2d(drHctl_[index]), 
	      (int)dpOdom_[index], (int)dpHctl_[index]);
    if (sumDrOdom_ + POSITION_ODOM_ALERT_LEVEL_DR < sumDrHctl_) {
	LOG_ERROR("Les hctls tournent dans le vide (rotation) Odo=%d Hct=%d\n",
		  r2d(sumDrOdom_), r2d(sumDrHctl_));
	Events->raise(EVENTS_PWM_ALERT);
	resetOdoColliDetection();
	return;
    }
    if (sumDpOdom_ + POSITION_ODOM_ALERT_LEVEL_DP < sumDpHctl_) {
	LOG_ERROR("Les hctls tournent dans le vide (lineaire) Odo=%d Hct=%d\n",
		  (int)(sumDpOdom_), (int)(sumDpHctl_));
	Events->raise(EVENTS_PWM_ALERT);
	resetOdoColliDetection();
	return;
    }
    
    index = (index+1)%ODOM_COLLI_DETECT_BUFFER_SIZE;
  
    sumDrOdom_ -= drOdom_[index];
    sumDrHctl_ -= drHctl_[index];
    sumDpOdom_ -= dpOdom_[index];
    sumDpHctl_ -= dpHctl_[index];
    
    oldPosHctl_ = posHctl_;
    oldPosOdom_ = posOdom_;
}

#define DEBUG_ODOMETRE
// ----------------------------------------------------------------------------
// RobotPosition::periodicTask
// ----------------------------------------------------------------------------
void RobotPosition::periodicTask(Millisecond time)
{
    switch(odometerType_) {
    case ODOMETER_UART_MANUAL:
        // Il n'y a qu'en mode manuel qu'il faut faire un updateOdometerPosition
        // car dans le cas automatique la fonction est appelee depuis 
        // l'odometer_04::filter
        updateOdometerPosition(); 
    case ODOMETER_UART_AUTOMATIC:
        // odometer updatePosition is already done by odometer04.filter each 
        // time it receives a data
        if (!isOdometerAlive()) {
            // si la carte odometre ne repond pas, on passe en mode codeurs 
            // des roues motrices
            LOG_ERROR("ODOMETER_UART is not responding, switch to "
                      "ODOMETER_HCTL\n");
//          Sound->play(SOUND_ODOMETER_ALERT, SND_PRIORITY_URGENT); TODO: 2004? [flo]
            setOdometerType(ODOMETER_MOTOR);
        } else {
#ifndef DEBUG_ODOMETRE
            posHctl_ = getOdometerPosition();
#endif
        }
#ifndef DEBUG_ODOMETRE
        // en mode DEBUG_ODOMETRE on calcule la position avec les hctl et 
        // avec les odometres pour pouvoir les comparer
        break;
#endif
    case ODOMETER_MOTOR:
        updateHctlPosition();
        break;
    }

    if (odometerType_ == ODOMETER_MOTOR) {
        pos_ = getHctlPosition();
    } else {
        pos_ = getOdometerPosition();
    }

    if (timeToCheckPatinage_+PERIOD_CHECK_PATINAGE<time) { 
        detectCollision();
	timeToCheckPatinage_=time;
    }

    static Millisecond oldTimeBuffer=0;
    if (oldTimeBuffer+POS_BUFFER_STEP_TIME < time) {
	addPositionToBuffer();
    }
    // toutes les secondes on affiche la position estimee du robot
    static Millisecond oldTime=0;
    // TODO: depends on verbose-level now? [flo]
    //if (getClassConfig()->displayOdomPos 
    if (getClassConfig()->verbose() > 1 
        && oldTime+1000<time) { 
        LOG_INFO("Time=%ds, "
		 "%sOdometer: x=%d y=%d t=%d %s "
		 "-- %sHctl: x=%d y=%d t=%d %s\n",
                 time/1000,
                 (odometerType_!=ODOMETER_MOTOR)?KB_CYAN:KB_RESTORE,
                 (int)posOdom_.center.x, (int)posOdom_.center.y, 
                 r2d(posOdom_.direction),
                 KB_RESTORE,
                 (odometerType_==ODOMETER_MOTOR)?KB_CYAN:KB_RESTORE,
                 (int)posHctl_.center.x, (int)posHctl_.center.y, 
                 r2d(posHctl_.direction),
                 KB_RESTORE);
        oldTime=time;
    }

    txtChanged_ = true;
}
