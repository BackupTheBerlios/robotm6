#ifndef __STRATEGY_DEFENCE_H__
#define __STRATEGY_DEFENCE_H__
 
#include "strategy2005.h"

/** 
 * @class StrategyDefenceCL
 * Strategy du robot de defence
 */
class StrategyDefenceCL : public Strategy2005CL
{
 public :
    StrategyDefenceCL(RobotMainCL* main);
    void run(int argc, char*argv[]);
};

#endif // __STRATEGY_DEFENCE_H__
