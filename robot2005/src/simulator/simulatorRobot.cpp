/**
 * @file simulatorServer.h
 *
 * @author Laurent Saint-Marcel
 *
 * Systeme de simulation du robot et de son environnement
 */

#include "simulatorRobot.h"

SimulatorRobot::SimulatorRobot() : 
    RobotBase("Simulated robot", CLASS_SIMULATOR),
    name_("NoName"), weight_(SIMU_WEIGHT_MEDIUM), model_(ROBOT_MODEL_ATTACK),
    brick_(false), D_(300), K_(1), 
    jackin_(false), emergencyStop_(false), lcdBtnYes_(false), lcdBtnNo_(false),
    lcdMessage_("Booting\nPlease wait..."), 
    realPos_(), estimatedPos_(),
    speedLeft_(0), speedRight_(0),
    pwmLeft_(0), pwmRight_(0),
    motorLeft_(0), motorRight_(0),
    needSendDisplayInfo_(false),
    matchStatus_(SIMU_STATUS_WAIT_START),

    motorLeftOld_(0), motorRightOld_(0),
    realPosOld_(), simuSpeed_(20.), 
    simuCoderSignRight_(1.), simuCoderSignLeft_(-1.),
    simuMotorNoise_(false), simuPosFirst_(true)
{
}

SimulatorRobot::~SimulatorRobot() 
{

}

void SimulatorRobot::updatePosition()
{
    // sauve l'etat courant au cas ou la nouvelle position soit invalide
    realPosOld_    = realPos_;
    motorLeftOld_  = motorLeft_;
    motorRightOld_ = motorRight_;

    // calcul variation des deplacements des moteurs
    motorRight_+= (int)(simuSpeed_*(simuCoderSignRight_*33.28*speedRight_
					  +(simuMotorNoise_?(10*rand()/(RAND_MAX+1.0)):0)));
    motorLeft_ += (int)(simuSpeed_*(simuCoderSignLeft_*33.28*speedLeft_
					  +(simuMotorNoise_?(10*rand()/(RAND_MAX+1.0)):0)));

    double deltaRight=0, deltaLeft=0;
    double deltaTheta=0, deltaSum=0;
    double KRight = simuCoderSignRight_*K_*motorRight_;
    double KLeft  = simuCoderSignLeft_*K_*motorLeft_;
 
    if (simuPosFirst_) {
	simuPosFirst_ = false;
    }
    deltaRight = KRight - simuCoderSignRight_*K_*motorRightOld_;
    deltaLeft  = KLeft  - simuCoderSignLeft_*K_*motorLeftOld_;
    deltaSum   = (deltaRight + deltaLeft)/2.;
    
    // calcul position reelle
    Radian oldDir = realPos_.direction;
    deltaTheta = (deltaRight-deltaLeft)/(D_);
    realPos_.direction += deltaTheta;
  
    if (fabs(deltaTheta) > 0.0001) {
	realPos_.center.x += deltaSum *
	    (sin(realPos_.direction)-sin(oldDir))/deltaTheta;
	realPos_.center.y += -deltaSum *
	    (cos(realPos_.direction)-cos(oldDir))/deltaTheta;
    } else {
	deltaTheta = (realPos_.direction +oldDir)/2.;
	realPos_.center.x += deltaSum * cos(deltaTheta);  
	realPos_.center.y += deltaSum * sin(deltaTheta);
    }

    // calcul position estimee
    Radian oldEDir = estimatedPos_.direction;
    deltaTheta = (deltaRight-deltaLeft)/(D_);
    estimatedPos_.direction += deltaTheta;
  
    if (fabs(deltaTheta) > 0.0001) {
	estimatedPos_.center.x += deltaSum *
	    (sin(estimatedPos_.direction)-sin(oldEDir))/deltaTheta;
	estimatedPos_.center.y += -deltaSum *
	    (cos(estimatedPos_.direction)-cos(oldEDir))/deltaTheta;
    } else {
	deltaTheta = (estimatedPos_.direction +oldEDir)/2.;
	estimatedPos_.center.x += deltaSum * cos(deltaTheta);  
	estimatedPos_.center.y += deltaSum * sin(deltaTheta);
    }
} 

#define SIMU_PWM_LIMIT 116
void SimulatorRobot::setNewPositionValid(bool isValid)
{
    if (!isValid) {
        realPos_    = realPosOld_;
        pwmLeft_=(MotorPWM)(1.5*pwmLeft_);
        pwmRight_=(MotorPWM)(1.5*pwmRight_);     
    } else {
        pwmLeft_ =(MotorPWM)(100*speedLeft_ /max(0.2, (double)fabs(motorLeft_  - motorLeftOld_ )/K_));
        pwmRight_=(MotorPWM)(100*speedRight_/max(0.2, (double)fabs(motorRight_ - motorRightOld_)/K_));
    }
    if (pwmLeft_>SIMU_PWM_LIMIT) pwmLeft_=SIMU_PWM_LIMIT;
    else if (pwmLeft_<-SIMU_PWM_LIMIT) pwmLeft_=-SIMU_PWM_LIMIT;
    if (pwmRight_>SIMU_PWM_LIMIT) pwmRight_=SIMU_PWM_LIMIT;
    else if (pwmRight_<-SIMU_PWM_LIMIT) pwmRight_=-SIMU_PWM_LIMIT;
} 

