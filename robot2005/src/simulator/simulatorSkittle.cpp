#include "simulatorSkittle.h"
#include "simulatorRobot.h"
#include "simulatorGrsBall.h"

SimulatorSkittle::SimulatorSkittle() : 
  RobotBase("SimulatorSkittle", CLASS_SIMULATOR), 
  skittle_(NULL), speedX_(0), speedY_(0){}
    
SimulatorSkittle::~SimulatorSkittle() {}
  
// -------------------------------------------------------------------
// update the simulated robot position
// -------------------------------------------------------------------
void SimulatorSkittle::updatePosition()
{
}
void SimulatorSkittle::setNewPositionValid()
{
}
void SimulatorSkittle::checkPosAndWall()
{
  
}
void SimulatorSkittle::checkPosAndBridge(BridgePosition const& bridge)
{
}
// do not check collision with bigger objects, they already did it
//bool checkPosAndOtherRobot(SimulatorRobot* other);
void SimulatorSkittle::checkPosAndSkittle(SimulatorSkittle* skittle)
{
}

