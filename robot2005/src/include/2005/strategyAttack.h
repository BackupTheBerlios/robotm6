#ifndef __STRATEGY_ATTACK_H__
#define __STRATEGY_ATTACK_H__
 
#include "strategy2005.h"

/** @class StrategyAttackCL
 * Strategy du robot d'attaque
 */
class StrategyAttackCL : public Strategy2005CL
{
 public :
    StrategyAttackCL(RobotMainCL* main);
    void run(int argc, char*argv[]);
};

#endif // __STRATEGY_ATTACK_H__
