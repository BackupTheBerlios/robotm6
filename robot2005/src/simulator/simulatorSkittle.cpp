#include "simulatorSkittle.h"
#include "simulatorRobot.h"
#include "simulatorGrsBall.h"
#include "simulator.h"

SimulatorSkittle::SimulatorSkittle() : 
  RobotBase("SimulatorSkittle", CLASS_SIMULATOR), 
  skittle_(NULL), speedX_(0), speedY_(0), isValid_(true), hasMoved_(false), newPos_(){}
    
SimulatorSkittle::~SimulatorSkittle() {}
/*
   Point         center;
    Millimeter    altitude;  // le bas de la quille
    Radian        direction; // direction (des vis vers le reflechissant) quand la quille est renversee
    SkittleStatus status; 
    RobotColor    color; // COLOR_RED / COLOR_GREEN
*/
// -------------------------------------------------------------------
// update the simulated robot position
// -------------------------------------------------------------------
void SimulatorSkittle::updatePosition()
{
    if (!skittle_) return;
    if (skittle_->center.x<0 || skittle_->center.x>TERRAIN_X) skittle_->center.x=10;
    if (skittle_->center.y<0 || skittle_->center.y>TERRAIN_Y) skittle_->center.y=10;
    newPos_.center.x = skittle_->center.x+speedX_;
    newPos_.center.y = skittle_->center.y+speedY_;
    if (fabs(speedX_)>0.05) speedX_/=1.01;
    else speedX_ = 0;
    if (fabs(speedY_)>0.05) speedY_/=1.01;
    else speedY_ = 0;
    if (speedX_!=0 || speedY_!=0) {
        Point pt(speedX_, speedY_);
        skittle_->direction = pt.angle()+M_PI/2;
    }
}
void SimulatorSkittle::setNewPositionValid()
{
    hasMoved_= (fabs(speedX_)>0.05) || (fabs(speedY_)>0.05);
    skittle_->center = newPos_.center;
}
void SimulatorSkittle::checkPosAndWall()
{
    if (!hasMoved_) return;
    // bord du terrain
    if (newPos_.center.x<QUILLE_RAYON) {
        if (speedX_<0) speedX_=-speedX_;
        newPos_.center.x = QUILLE_RAYON;
    } else if (newPos_.center.x > TERRAIN_X - QUILLE_RAYON) {
        if (speedX_>0) speedX_=-speedX_;
        newPos_.center.x = TERRAIN_X - QUILLE_RAYON;
    }
    if (newPos_.center.y<QUILLE_RAYON) {
        if (speedY_<0) speedY_=-speedY_;
        newPos_.center.y = QUILLE_RAYON;
    } else if (newPos_.center.y > TERRAIN_Y - QUILLE_RAYON) {
        if (speedY_>0) speedY_=-speedY_;
        newPos_.center.y = TERRAIN_Y - QUILLE_RAYON;
    }
}
void SimulatorSkittle::checkPosAndBridge(BridgePosition const& bridge)
{
    if (SimulatorCL::instance()->isInRiver(skittle_->center)) {
        // la quille ne doit pas sortire de la riviere une fois qu'elle y est tombee!
        if (!SimulatorCL::instance()->isInRiver(newPos_.center)) {
            newPos_.center = skittle_->center;
            speedX_=0;
            speedY_=0;
        }
        skittle_->altitude=QUILLE_RAYON-TERRAIN_RIVIERE_PROFONDEUR;
    }
}
// do not check collision with bigger objects, they already did it
//bool checkPosAndOtherRobot(SimulatorRobot* other);
void SimulatorSkittle::checkPosAndSkittle(SimulatorSkittle* other)
{
    if (!skittle_ || !other || !other->skittle_ || other ==this) return;
    Millimeter radius1=(skittle_->status==SKITTLE_DOWN)?2*QUILLE_RAYON:QUILLE_RAYON;
    Millimeter radius2=(other->skittle_->status==SKITTLE_DOWN)?2*QUILLE_RAYON:QUILLE_RAYON;
    if (fabs(skittle_->altitude - other->skittle_->altitude)<QUILLE_RAYON || 
        hasMoved_) {
        if (dist(newPos_.center, other->newPos_.center) < radius1 + radius2) {
            Point newCenter=newPos_.center+(other->newPos_.center-newPos_.center)*(radius1 + radius2)/dist(newPos_.center, other->newPos_.center);
            other->centerAfterCollision(newCenter);
            skittle_->status=SKITTLE_DOWN;
            skittle_->altitude=QUILLE_RAYON;
        }
    }
}

void SimulatorSkittle::centerAfterCollision(Point& newCenter) 
{
    if (newCenter.x<0 || newCenter.x>TERRAIN_X || newCenter.y<0 || newCenter.y>TERRAIN_Y) return;
    newPos_.center = newCenter;
    speedX_=(newPos_.center.x-skittle_->center.x)/5.;
    speedY_=(newPos_.center.y-skittle_->center.y)/5.; 
    Millimeter S = sqrt(speedX_*speedX_+speedY_*speedY_);
    if (S==0) return;
    if (S<0.2) {
        speedX_*=0.2/S;
        speedY_*=0.2/S;
    };
    if (S >0.6) {
        speedX_*=0.6/S;
        speedY_*=0.6/S;
    }
    skittle_->status=SKITTLE_DOWN;
    skittle_->altitude=QUILLE_RAYON;
}

bool SimulatorSkittle::getIntersection(Point const&   captor, 
                                       Segment const& captorVision, 
                                       Millimeter     zPosCaptor, 
                                       Point&         intersectionPt)
{
    return false;
}
