/**
 * @file simulatorServer.h
 *
 * @author Laurent Saint-Marcel
 *
 * Systeme de simulation du robot et de son environnement
 */
#include "simulator.h"
#include "simulatorRobot.h"
#include "simulatorGrsBall.h"
#include "simulatorSkittle.h"

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
    realPosOld_(), 
    odomK_(1), odomSpeed_(1), odomLeft_(0), odomRight_(0),
    simuSpeed_(20.), 
    simuCoderSignRight_(1.), simuCoderSignLeft_(-1.),
    simuMotorNoise_(false), simuPosFirst_(true), isValid_(true), isDead_(0)
{
    setBorderRobotAttack();
}

SimulatorRobot::~SimulatorRobot() 
{

}

void SimulatorRobot::updateOdometer()
{
    if (dist(realPosOld_.center, realPos_.center) < 100) {
        double simuSpeed = odomSpeed_*SimulatorCL::instance()->getSimulationSpeed();
        Point odoLeftOld=odomLeftPt_, odoLeft=odomLeftPt_;
        Point odoRightOld=odomRightPt_, odoRight=odomRightPt_;
        Geometry2D::convertToOrthogonalCoord(realPos_.center, realPos_.direction, 
                                             odoLeft);
        Geometry2D::convertToOrthogonalCoord(realPos_.center, realPos_.direction, 
                                             odoRight);
        Geometry2D::convertToOrthogonalCoord(realPosOld_.center, realPosOld_.direction, 
                                             odoLeftOld);
        Geometry2D::convertToOrthogonalCoord(realPosOld_.center, realPosOld_.direction, 
                                             odoRightOld);
        odomLeft_ += (CoderPosition)(simuSpeed*dist(odoLeft, odoLeftOld)/odomK_*(((na2PI(dir(odoLeftOld, odoLeft)-realPos_.direction,-M_PI/2))<(M_PI/2))?1:-1)); 
        odomRight_ += (CoderPosition)(simuSpeed*dist(odoRight, odoRightOld)/odomK_*(((na2PI(dir(odoRightOld, odoRight)-realPos_.direction,-M_PI/2))<(M_PI/2))?1:-1));
        if (odomLeft_<SHRT_MIN) odomLeft_+=USHRT_MAX;
        if (odomLeft_>SHRT_MAX) odomLeft_-=USHRT_MAX;
        if (odomRight_<SHRT_MIN) odomRight_+=USHRT_MAX;
        if (odomRight_>SHRT_MAX) odomRight_-=USHRT_MAX;
    }
}

void SimulatorRobot::updatePosition()
{
    if (isDead_) return;
    if (brick_) return;
    // sauve l'etat courant au cas ou la nouvelle position soit invalide
    realPosOld_    = realPos_;

    double simuSpeed = SimulatorCL::instance()->getSimulationSpeed();
   
    // Calcule de la position des odometres
    motorLeftOld_  = motorLeft_;
    motorRightOld_ = motorRight_;

    // calcul variation des deplacements des moteurs
    motorRight_+= (int)((simuCoderSignRight_*simuSpeed*speedRight_
					  +(simuMotorNoise_?(10*rand()/(RAND_MAX+1.0)):0)));
    motorLeft_ += (int)((simuCoderSignLeft_*simuSpeed*speedLeft_
					  +(simuMotorNoise_?(10*rand()/(RAND_MAX+1.0)):0)));

    double deltaRight=0, deltaLeft=0;
    double deltaTheta=0, deltaSum=0;
    double KRight = simuCoderSignRight_*K_*motorRight_;
    double KLeft  = simuCoderSignLeft_*K_*motorLeft_;
    if (simuPosFirst_) {
        motorRightOld_ = motorRight_;
        motorLeftOld_  = motorLeft_;
	simuPosFirst_ = false;
    } else {
        if (motorLeft_ - motorLeftOld_ > SHRT_MAX) { // short max =unsigned short max /2
            motorLeftOld_ += USHRT_MAX;   // unsigned short max
        } else if (motorLeft_ - motorLeftOld_  < SHRT_MIN) {
            motorLeftOld_ -= USHRT_MAX;
        }
        if (motorRight_ - motorRightOld_ > SHRT_MAX) { // short max =unsigned short max /2
            motorRightOld_ += USHRT_MAX;   // unsigned short max
        } else if (motorRight_ - motorRightOld_ < SHRT_MIN) {
            motorRightOld_ -= USHRT_MAX;
        } 
    }
    deltaRight = simuSpeed_*(KRight - simuCoderSignRight_*K_*motorRightOld_);
    deltaLeft  = simuSpeed_*(KLeft  - simuCoderSignLeft_*K_*motorLeftOld_);
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
 /*   printf("%lf %lf %d %d %d %lf %lf %lf\n", K_, D_, (int)simuCoderSignRight_,
                    (int)motorRight_, (int)speedRight_, KRight, 
                    deltaRight, realPos_.center.x);
    */

/*
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
*/
    isValid_=true;
    setRealPos(realPos_);
}

#define SIMU_PWM_LIMIT 116
void SimulatorRobot::setNewPositionValid()
{
    if (isDead_) return;
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
    if (isDead_) return;
    Polygon gameArea(SimulatorCL::instance()->getWallPts(), SIMU_WALL_BORDER_PTS_NBR);
    if (!Geometry2D::isPointInPolygon(gameArea, realPos_.center)) {
        isValid_ = false;
        setRealPos(realPosOld_);
        return;
    }
    Point intersection;
    Point* wallPts=SimulatorCL::instance()->getWallPts();
    for(unsigned int i=0;i!=SIMU_WALL_BORDER_PTS_NBR;i++) {
        Segment wallBorder(wallPts[i], wallPts[((i+1)%SIMU_WALL_BORDER_PTS_NBR)]);
        if(checkSegmentIntersectionWithRobot(wallBorder, 30, intersection)){
            isValid_ = false;
            setRealPos(realPosOld_);
            return;
        }
    }
}

void SimulatorRobot::checkPosAndBridge(BridgePosition const& bridge)
{
    if (isDead_) return;
    // si on est dans la riviere on est mort!
    if (SimulatorCL::instance()->isInRiver(wheelRealPts_[0]) ||
        SimulatorCL::instance()->isInRiver(wheelRealPts_[1]) ||
        (SimulatorCL::instance()->isInRiver(wheelRealPts_[2]) &&
         SimulatorCL::instance()->isInRiver(wheelRealPts_[3]))) {
        isValid_ = false;
        if (SimulatorCL::instance()->isInRiver(wheelRealPts_[0])) {
            isDead_=1;
        } else if (SimulatorCL::instance()->isInRiver(wheelRealPts_[1])) {
            isDead_=2;
        } else isDead_=3;
        needSendDisplayInfo_=true;
        setRealPos(realPosOld_);
        return;
    }

    // collision avec les murs
    if (!isValid_) return;
    Point intersection;
    Point* bridgePts=SimulatorCL::instance()->getBridgePts();
    for(unsigned int i=0;i+1<SIMU_BRIDGE_BORDER_PTS_NBR;i+=2) {
        Segment bridgeBorder(bridgePts[i], bridgePts[i+1]);
        if(checkSegmentIntersectionWithRobot(bridgeBorder, 30, intersection)){
          /*  printf("Intersection:"); 
            intersection.print();
            bridgePts[i].print();
            bridgePts[i+1].print();
          */
            isValid_ = false;
            setRealPos(realPosOld_);
            return;
        }
    }
    // pas besoin de detecter la collision avec les murs riviere car ils sont trop bas
}

void SimulatorRobot::checkPosAndOtherRobot(SimulatorRobot* other)
{

}

void SimulatorRobot::checkPosAndGRSBall(SimulatorGrsBall* ball)
{
    if (!ball || !ball->ball_) return;
    Circle circle(ball->ball_->center, BALLE_GRS_RAYON);
    Point intersection;
    if (checkCircleIntersectionWithRobot(circle, BALLE_GRS_RAYON, intersection)) {
        Point newCenter = intersection+(ball->ball_->center-intersection)* BALLE_GRS_RAYON/dist(ball->ball_->center,intersection);
        ball->centerAfterCollision(newCenter);
    }
}

void SimulatorRobot::checkPosAndSkittle(SimulatorSkittle* skittle)
{
    if (!skittle || !skittle->skittle_ || realPos_.center.x < 10) return;
    // meme si la quille est tombee on la gere comme si elle etait ronde, c'est plus facile!
    Circle circle(skittle->skittle_->center, QUILLE_RAYON);
    if (skittle->skittle_->status == SKITTLE_DOWN) {
        circle.radius=2*QUILLE_RAYON;
    } 
/*
    Point intersection;
    if (checkCircleIntersectionWithRobot(circle, 80, intersection)) {
        Point newCenter = intersection+(skittle->newPos_.center-intersection)* QUILLE_RAYON/dist(skittle->newPos_.center,intersection);
        skittle->centerAfterCollision(newCenter);
    } 
*/
    if (Geometry2D:: isCircleInPolygon(borderPol_[0], circle) ||
        Geometry2D:: isCircleInPolygon(borderPol_[1], circle) ||
        Geometry2D:: isCircleInPolygon(borderPol_[2], circle)) {
        Point newCenter =  realPos_.center+(skittle->newPos_.center-realPos_.center)*400./dist(skittle->newPos_.center, realPos_.center);
        skittle->centerAfterCollision(newCenter);

    }
}
bool SimulatorRobot::getIntersection(Point const&   captor, 
                                     Segment const& captorVision, 
                                     Millimeter     zPosCaptor, 
                                     Point&         intersectionPt)
{
    Point inter;
    Millimeter minDistance=INFINITE_DIST;
    if (zPosCaptor>70) {
        for(unsigned int i=0;i!=4;i++) {
            Segment robotBorder(borderRealPts_[8+i], borderRealPts_[8+((i+1)%4)]);
            if(Geometry2D::getSegmentsIntersection(robotBorder, captorVision, inter)) {
                Millimeter distance=dist(inter, captor);
                if (minDistance<0 || minDistance>distance) {
                    minDistance = distance;
                    intersectionPt = inter;
                } 
            }
        }
    } else {
        for(unsigned int i=0;i!=4;i++) {
            Segment robotBorder(borderRealPts_[i], borderRealPts_[(i+1)%4]);
            if(Geometry2D::getSegmentsIntersection(robotBorder, captorVision, inter)) {
                Millimeter distance=dist(inter, captor);
                if (minDistance<0 || minDistance>distance) {
                    minDistance = distance;
                    intersectionPt = inter;
                } 
            }
        }
        for(unsigned int i=0;i!=4;i++) {
            Segment robotBorder(borderRealPts_[4+i], borderRealPts_[4+((i+1)%4)]);
            if(Geometry2D::getSegmentsIntersection(robotBorder, captorVision, inter)) {
                Millimeter distance=dist(inter, captor);
                if (minDistance<0 || minDistance>distance) {
                    minDistance = distance;
                    intersectionPt = inter;
                } 
            }
        }
    }
    
    return (minDistance>0);  
}
bool SimulatorRobot::checkSegmentIntersectionWithRobot(Segment const& seg,
                                                       Millimeter z,
                                                       Point& intersectionPt) 
{
    if (z>60) {
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

bool SimulatorRobot::checkCircleIntersectionWithRobot(Circle const& circle,
                                                      Millimeter z,
                                                      Point& intersectionPt) 
{
    Point intersectionPt1;
    Point intersectionPt2;
    int npts=0;
    if (z>60) {
        for(unsigned int i=0;i!=4;i++) {
            Segment robotBorder(borderRealPts_[8+i], borderRealPts_[8+((i+1)%4)]);
            npts=Geometry2D::getSegmentCircleIntersection(robotBorder, circle,
                                                          intersectionPt1, intersectionPt2);
            if (npts==1) {
                intersectionPt = intersectionPt1;
                return true;
            } else if (npts==2) {
                intersectionPt = (intersectionPt1+intersectionPt2)/2.;
                return true;
            } 
        }
    } else {
        for(unsigned int i=0;i!=4;i++) {
            Segment robotBorder(borderRealPts_[i], borderRealPts_[(i+1)%4]);
            npts=Geometry2D::getSegmentCircleIntersection(robotBorder, circle,
                                                          intersectionPt1, intersectionPt2);
            if (npts==1) {
                intersectionPt = intersectionPt1;
                return true;
            } else if (npts==2) {
                intersectionPt = (intersectionPt1+intersectionPt2)/2.;
                return true;
            } 
        }
        for(unsigned int i=0;i!=4;i++) {
            Segment robotBorder(borderRealPts_[4+i], borderRealPts_[4+((i+1)%4)]);
            npts=Geometry2D::getSegmentCircleIntersection(robotBorder, circle,
                                                          intersectionPt1, intersectionPt2);
            if (npts==1) {
                intersectionPt = intersectionPt1;
                return true;
            } else if (npts==2) {
                intersectionPt = (intersectionPt1+intersectionPt2)/2.;
                return true;
            } 
        }
    }
    return false;
}

void SimulatorRobot::convertBorderToCylindric(Point const& center)
{
    for(unsigned int i=0;i<SIMU_ROBOT_PTS_NBR;i++) {
        Geometry2D::convertToCylindricCoord(center, borderPts_[i]);
    }
    for(unsigned int i=0;i<SIMU_ROBOT_WHEEL_PTS_NBR;i++) {
        Geometry2D::convertToCylindricCoord(center, wheelPts_[i]);
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
    for(unsigned int i=0;i<SIMU_ROBOT_WHEEL_PTS_NBR;i++) {
        wheelRealPts_[i] = wheelPts_[i];
        Geometry2D::convertToOrthogonalCoord(pos.center, 
                                             pos.direction, 
                                             wheelRealPts_[i]);
    }
    borderPol_[0] = Polygon(borderRealPts_, 4);
    borderPol_[1] = Polygon(&(borderRealPts_[4]), 4);
    borderPol_[2] = Polygon(&(borderRealPts_[8]), 4);
}

void SimulatorRobot::setBorderRobotAttack()
{
    // points en coordonnees relatives 
    wheelPts_[0] = Point(0, 160);
    wheelPts_[1] = Point(0, -160);
    wheelPts_[2] = Point(90, 140);
    wheelPts_[3] = Point(90, -140);
    
    borderPts_[0] = Point(140, 170);
    borderPts_[1] = Point(-60, 170);
    borderPts_[2] = Point(-60, 120);
    borderPts_[3] = Point(90, 120);

    borderPts_[4] = Point(140, -170);
    borderPts_[5] = Point(-60, -170);
    borderPts_[6] = Point(-60, -120);
    borderPts_[7] = Point(90, -120);

    borderPts_[8]  = Point(140, -170);
    borderPts_[9]  = Point(-60, -170);
    borderPts_[10] = Point(-60, 170);
    borderPts_[11] = Point(140, 170);

    // passage des coordonnees en mode cylyndrique
    convertBorderToCylindric(Point(0,0));
}

void SimulatorRobot::setBorderRobotDefence()
{
    // points en coordonnees relatives
    wheelPts_[0] = Point(0, 160);
    wheelPts_[1] = Point(0, -160);
    wheelPts_[2] = Point(90, 140);
    wheelPts_[3] = Point(90, -140);
    
    borderPts_[0] = Point(140, 170);
    borderPts_[1] = Point(-60, 170);
    borderPts_[2] = Point(-60, 120);
    borderPts_[3] = Point(90, 120);

    borderPts_[4] = Point(140, -170);
    borderPts_[5] = Point(-60, -170);
    borderPts_[6] = Point(-60, -120);
    borderPts_[7] = Point(90, -120);

    borderPts_[8]  = Point(140, -170);
    borderPts_[9]  = Point(-60, -170);
    borderPts_[10] = Point(-60, 170);
    borderPts_[11] = Point(140, 170);


    // passage des coordonnees en mode cylyndrique
    convertBorderToCylindric(Point(0,0));
}

void SimulatorRobot::setBorderRobotBrick()
{
 // points en coordonnees relatives
    wheelPts_[0] = Point(0, 140);
    wheelPts_[1] = Point(0, -140);
    wheelPts_[2] = Point(140, 0);
    wheelPts_[3] = Point(-140, 0);

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
