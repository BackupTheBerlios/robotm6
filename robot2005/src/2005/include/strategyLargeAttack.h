#pragma once 
#include "strategyAttack.h"

enum CollisionEnum {
    COLLISION_NONE,
    COLLISION_LEFT,
    COLLISION_RIGHT,
    COLLISION_BOTH
};

/**
 * Strategy large (deux cases par passe) du robot d'attaque
 */
class StrategyLargeAttackCL : public StrategyAttackCL
{
 public :
    StrategyLargeAttackCL(RobotMainCL* main, const char* menuName)
	: StrategyAttackCL(main, menuName) {}

    bool centerOnSupport(Point supportCenter);
    bool rotateOnSupport(Point supportCenter, Radian targetTheta);
    bool leaveSupport(Point supportCenter, Point target);
    bool handleCollision(CollisionEnum collision,
			 Point lineStart,
			 Point lineEnd);
    bool preDefinedSkittleExploration1();
    bool preDefinedSkittleExploration2();

};
