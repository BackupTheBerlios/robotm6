#include "simulatorSkittle.h"
#include "simulatorRobot.h"
#include "simulatorGrsBall.h"

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
    newPos_.center.x = skittle_->center.x+speedX_;
    newPos_.center.y = skittle_->center.y+speedY_;
    if (fabs(speedX_)>0.2) speedX_/=1.1;
    else speedX_ = 0;
    if (fabs(speedY_)>0.2) speedY_/=1.1;
    else speedY_ = 0;
}
void SimulatorSkittle::setNewPositionValid()
{
    if (!skittle_) return;
}
void SimulatorSkittle::checkPosAndWall()
{
    if (!skittle_) return;
}
void SimulatorSkittle::checkPosAndBridge(BridgePosition const& bridge)
{
    if (!skittle_) return;
}
// do not check collision with bigger objects, they already did it
//bool checkPosAndOtherRobot(SimulatorRobot* other);
void SimulatorSkittle::checkPosAndSkittle(SimulatorSkittle* skittle)
{
    if (!skittle_) return;
}

void SimulatorSkittle::collisionWithRobot(Point& newCenter) 
{

}

bool SimulatorSkittle::getIntersection(Point const&   captor, 
                                       Segment const& captorVision, 
                                       Millimeter     zPosCaptor, 
                                       Point&         intersectionPt)
{
    return false;
}
