/** Point d'entree de l'executable du robot d'attack */
#define LOG_CLASSID CLASS_DEFAULT
#include "log.h"

#include "strategy2005.h"
#include "robotConfig2005.h"
#include "robotMain2005.h"
//#include "strategyDefence.h"
#include "strategyHomoDefence.h"

int main(int argc, char* argv[])
{
  RobotConfigCL*  config;
  RobotMainCL*    robotMain=NULL;
  StrategyCL*     strategyDefence=NULL;

#ifndef SIMULATED
#define SIMULATED false
  LOG_INFO("MODE REAL\n");
#else
  LOG_INFO("SIMULATED\n");
#endif

  LOG_INFO("ROBOT_DEFENCE\n");
  config = new RobotConfigDefence2005CL(SIMULATED);

  robotMain = new RobotMain2005CL();
  //strategyDefence = new StrategyDefenceCL(robotMain);
  strategyDefence = new StrategyHomoDefenceCL(robotMain);

  //ClassConfig::find(CLASS_MOVE)->setVerboseLevel(VERBOSE_DEBUG);

  robotMain->run(strategyDefence, argc, argv); // traverse le pont

  while(1) {sleep(1);}
  delete strategyDefence;
  delete robotMain;
  delete config;

  return 0;
}
