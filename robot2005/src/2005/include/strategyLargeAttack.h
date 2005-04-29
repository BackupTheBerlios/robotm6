#pragma once 
#include "strategyAttack.h"

/**
 * Strategy large (deux cases par passe) du robot d'attaque
 */
class StrategyLargeAttackCL : public StrategyAttackCL
{
 public :
    StrategyLargeAttackCL(RobotMainCL* main, const char* menuName)
	: StrategyAttackCL(main, menuName) {}

    bool calcSupportCenterCollision(Point pos, Point* supportCenter) const;
    bool centerOnSupport(Point supportCenter);
    bool rotateOnSupport(Point supportCenter, Radian targetTheta);
    bool leaveSupport(Point supportCenter, Point target);
    bool preDefinedSkittleExploration1();
    bool preDefinedSkittleExploration2();

};
