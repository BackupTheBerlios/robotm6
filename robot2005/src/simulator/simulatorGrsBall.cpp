#include "simulatorSkittle.h"
#include "simulatorRobot.h"
#include "simulatorGrsBall.h"

SimulatorGrsBall::SimulatorGrsBall() : 
  RobotBase("SimulatorGrsBall", CLASS_SIMULATOR), 
  ball_(NULL), speedX_(0), speedY_(0){}
    
SimulatorGrsBall::~SimulatorGrsBall() {}
  
// -------------------------------------------------------------------
// update the simulated robot position
// -------------------------------------------------------------------
void SimulatorGrsBall::updatePosition()
{
}
void SimulatorGrsBall::setNewPositionValid()
{
}
void SimulatorGrsBall::checkPosAndWall()
{
  
}
void SimulatorGrsBall::checkPosAndBridge(BridgePosition const& bridge)
{
}
// do not check collision with bigger objects, they already did it
//bool checkPosAndOtherRobot(SimulatorRobot* other);
void SimulatorGrsBall::checkPosAndGRSBall(SimulatorGrsBall* other)
{ 
}
void SimulatorGrsBall::checkPosAndSkittle(SimulatorSkittle* skittle)
{
}

bool SimulatorGrsBall::getIntersection(Point const&   captor, 
                                     Segment const& captorVision, 
                                     Millimeter     zPosCaptor, 
                                     Point&         intersectionPt)
{
    return false;
}
