/**
 * @file simulatorServer.h
 *
 * @author Laurent Saint-Marcel
 *
 * Systeme de simulation du robot et de son environnement
 */
#include "simulator.h"
#include "simulatorRobot.h"
#include "simulatorRobot.h"
#include "simulatorGrsBall.h"

SimulatorRobot::SimulatorRobot() : 
    RobotBase("Simulated robot", CLASS_SIMULATOR),
    name_("NoName"), weight_(SIMU_WEIGHT_MEDIUM), model_(ROBOT_MODEL_ATTACK),
    brick_(false), D_(300), K_(0.0001), 
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
    simuMotorNoise_(false), simuPosFirst_(true), isValid_(true)
{
    setBorderRobotAttack();
}

SimulatorRobot::~SimulatorRobot() 
{

}

void SimulatorRobot::updatePosition()
{
    if (brick_) return;
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
        motorRightOld_ = motorRight_;
        motorLeftOld_  = motorLeft_;
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
    printf("%d %d %d%d %d\n",(int)motorRight_, (int)speedRight_, (int)KRight, (int)deltaRight, (int)realPos_.center.x);
    

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
    isValid_=true;
    setRealPos(realPos_);
}

#define SIMU_PWM_LIMIT 116
void SimulatorRobot::setNewPositionValid()
{
    if (brick_) return;
    if (!isValid_) {
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

void SimulatorRobot::setRealPos(Position const& pos) {
    realPos_=pos; 
    convertBorderToOrthogonal(realPos_);
}

void SimulatorRobot::checkPosAndWall() 
{
    Polygon gameArea(SimulatorCL::instance()->getWallPts(), SIMU_WALL_BORDER_PTS_NBR);
    if (!Geometry2D::isPointInPolygon(gameArea, realPos_.center)) {
        isValid_ = false;
        setRealPos(realPosOld_);
        return;
    }
    Point intersection;
    for(unsigned int i=0;i!=SIMU_WALL_BORDER_PTS_NBR;i++) {
        Segment wallBorder(borderRealPts_[i], borderRealPts_[((i+1)%SIMU_WALL_BORDER_PTS_NBR)]);
        if(checkSegmentIntersectionWithRobot(wallBorder, 30, intersection)){
            isValid_ = false;
            setRealPos(realPosOld_);
            return;
        }
    }
}

void SimulatorRobot::checkPosAndBridge(BridgePosition const& bridge)
{
    if (!isValid_) return;
}

void SimulatorRobot::checkPosAndOtherRobot(SimulatorRobot* other)
{

}

void SimulatorRobot::checkPosAndGRSBall(SimulatorGrsBall* ball)
{

}

void SimulatorRobot::checkPosAndSkittle(SimulatorSkittle* skittle)
{

}

bool SimulatorRobot::checkSegmentIntersectionWithRobot(Segment const& seg,
                                                       Millimeter z,
                                                       Point intersectionPt) 
{
    if (z>70) {
        for(unsigned int i=0;i!=4;i++) {
            Segment robotBorder(borderRealPts_[8+i], borderRealPts_[8+((i+1)%4)]);
            if(Geometry2D::getSegmentsIntersection(robotBorder, seg, intersectionPt)) return true;
        }
    } else {
        for(unsigned int i=0;i!=4;i++) {
            Segment robotBorder(borderRealPts_[i], borderRealPts_[(i+1)%4]);
            if(Geometry2D::getSegmentsIntersection(robotBorder, seg, intersectionPt)) return true;
        }
        for(unsigned int i=0;i!=4;i++) {
            Segment robotBorder(borderRealPts_[4+i], borderRealPts_[4+((i+1)%4)]);
            if(Geometry2D::getSegmentsIntersection(robotBorder, seg, intersectionPt)) return true;
        }
    }
    return false;
}

void SimulatorRobot::convertBorderToCylindric(Point const& center)
{
    for(unsigned int i=0;i<SIMU_ROBOT_PTS_NBR;i++) {
        Geometry2D::convertToCylindricCoord(center, borderPts_[i]);
    }
}

void SimulatorRobot::convertBorderToOrthogonal(Position const& pos)
{
    for(unsigned int i=0;i<SIMU_ROBOT_PTS_NBR;i++) {
        borderRealPts_[i] = borderPts_[i];
        Geometry2D::convertToOrthogonalCoord(pos.center, 
                                            pos.direction, 
                                            borderRealPts_[i]);
    }
    borderPol_[0] = Polygon(borderRealPts_, 4);
    borderPol_[1] = Polygon(&(borderRealPts_[4]), 4);
    borderPol_[2] = Polygon(&(borderRealPts_[8]), 4);
}

void SimulatorRobot::setBorderRobotAttack()
{
    // points en coordonnees relatives
    borderPts_[0] = Point(170, 140);
    borderPts_[1] = Point(170, -60);
    borderPts_[2] = Point(120, -60);
    borderPts_[3] = Point(120, 90);

    borderPts_[4] = Point(-170, 140);
    borderPts_[5] = Point(-170, -60);
    borderPts_[6] = Point(-120, -60);
    borderPts_[7] = Point(-120, 90);

    borderPts_[8]  = Point(-170, 140);
    borderPts_[9]  = Point(-170, -60);
    borderPts_[10] = Point(170, -60);
    borderPts_[11] = Point(170, 140);

    // passage des coordonnees en mode cylyndrique
    convertBorderToCylindric(Point(0,0));
}

void SimulatorRobot::setBorderRobotDefence()
{
    // points en coordonnees relatives
    borderPts_[0] = Point(170, 140);
    borderPts_[1] = Point(170, -60);
    borderPts_[2] = Point(120, -60);
    borderPts_[3] = Point(120, 90);

    borderPts_[4] = Point(-170, 140);
    borderPts_[5] = Point(-170, -60);
    borderPts_[6] = Point(-120, -60);
    borderPts_[7] = Point(-120, 90);

    borderPts_[8]  = Point(-170, 140);
    borderPts_[9]  = Point(-170, -60);
    borderPts_[10] = Point(170, -60);
    borderPts_[11] = Point(170, 140);

    // passage des coordonnees en mode cylyndrique
    convertBorderToCylindric(Point(0,0));
}

void SimulatorRobot::setBorderRobotBrick()
{
 // points en coordonnees relatives
    borderPts_[0]  = Point(-150, -150);
    borderPts_[1]  = Point(-150,  150);
    borderPts_[2]  = Point(150,   150);
    borderPts_[3]  = Point(150,  -150);

    borderPts_[4]  = Point(-150, -150);
    borderPts_[5]  = Point(-150,  150);
    borderPts_[6]  = Point(150,   150);
    borderPts_[7]  = Point(150,  -150);

    borderPts_[8]  = Point(-150, -150);
    borderPts_[9]  = Point(-150,  150);
    borderPts_[10] = Point(150,   150);
    borderPts_[11] = Point(150,  -150);

    // passage des coordonnees en mode cylyndrique
    convertBorderToCylindric(Point(0,0));
}
