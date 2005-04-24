#ifndef __STRATEGY_DEFENCE_H__
#define __STRATEGY_DEFENCE_H__
 
#include "strategy2005.h"
#include "crane.h"

enum DefenceStrategyMode {
    DEFENCE_MODE_HOMOLOGATION,
    DEFENCE_MODE_FOLLOW_ATTACK,
    DEFENCE_MODE_FOLLOW_BORDER
};

/** 
 * @class StrategyDefenceCL
 * Strategy du robot de defence
 */
class StrategyDefenceCL : public Strategy2005CL
{
 public :
    StrategyDefenceCL(RobotMainCL* main);
    void run(int argc, char*argv[]);

 protected:
    bool preparePullupMechanism();
    bool autoCheck();

    bool gotoBridgeHomologation();
    bool gotoBridgeByLeft();
    bool gotoBridgeFollowBorder();

    bool doOneProtectionLoop();
    bool exploreAroundSkittles();
 private:
    DefenceStrategyMode modeBridgeDefence_;
};

static const CranePositionX CRANE_STARTING_POS_X=0;
static const CranePositionZ CRANE_STARTING_POS_Z=0;
static const Millisecond    CRANE_WAIT_GOTO_START_POS=5000;

static const Millisecond DEFENCE_DELAY_AT_START = 15000;
static const Millisecond DEFENCE_DELAY_HOMOLOGATION = 45000;

static const Millisecond DEFENCE_COLLISION_DELAY = 3000;
static const Millisecond DEFENCE_TIME_PROTECTION_BEFORE_EXPLORE = 60000;

#endif // __STRATEGY_DEFENCE_H__
