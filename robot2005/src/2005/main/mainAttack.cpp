/** Point d'entree de l'executable du robot d'attack */
#define LOG_CLASSID CLASS_DEFAULT
#include "log.h"

#include "strategy2005.h"
#include "robotConfig2005.h"
#include "robotMain2005.h"
#include "strategyAttack.h"
#include "strategyLargeAttack.h"

int main(int argc, char* argv[])
{
  RobotConfigCL*  config;
  RobotMainCL*    robotMain=NULL;
  StrategyCL*     strategyAttack=NULL;
  StrategyCL*     strategyLargeAttack=NULL;

#ifndef SIMULATED
#define SIMULATED false
  LOG_INFO("MODE REAL\n");
#else
  LOG_INFO("SIMULATED\n");
#endif

  LOG_INFO("ROBOT_ATTACK\n");
  config = new RobotConfigAttack2005CL(SIMULATED);

  robotMain = new RobotMain2005CL();

  strategyAttack = new StrategyAttackCL(robotMain, "Char*");
  strategyLargeAttack = new StrategyLargeAttackCL(robotMain, "flo");
  
  robotMain->addStrategyToMenu(strategyAttack);
  robotMain->addStrategyToMenu(strategyLargeAttack);
  robotMain->menu(argc, argv);

  //ClassConfig::find(CLASS_MOVE)->setVerboseLevel(VERBOSE_DEBUG);

  //robotMain->run(strategyAttack, argc, argv); // traverse le pont

  while(1) {sleep(1);}
  delete strategyAttack;
  delete robotMain;
  delete config;

  return 0;
}
