#include "robotMain.h"
#include "strategyAttack.h"
#include "robotPosition.h"
#include "events.h"
#include "move.h"
#include "movementManager.h"
#include "lcd.h"
#include "log.h"
#include "geometry2D.h"
#include "bumperMapping.h"


// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre la fin d'un 
// mouvement en testant en plus si il y a un fosse devant
// ----------------------------------------------------------------------------
static bool evtEndMoveBridge(bool evt[])
{
    return evtEndMove(evt)
        || evt[EVENTS_GROUP_BRIDGE];
}


class StrategyAttackBadBumperCL : public StrategyAttackCL
{
public:
    StrategyAttackBadBumperCL(RobotMainCL* main, const char* menuName)
	: StrategyAttackCL(main, menuName)
	{
	}
    
    bool findAndCrossBridgeNoLeftBumper();
    bool findAndCrossBridgeNoRightBumper();
    bool gotoBridgeEntryRotateToSeeBridge(bool turnOnLeftWheel);
    
};

/**
 * crosses the bridge without left bumber.
 */
bool StrategyAttackBadBumperCL::findAndCrossBridgeNoLeftBumper() 
{
    const bool TURN_ON_LEFT_WHEEL = true;
    while (true) {
	bridge_ = BRIDGE_POS_MIDDLE_CENTER;
	Log->bridge(bridge_);
	bridgeDetectionByCenter_ = false;

	// try again, if failed...
	if (!gotoBridgeEntry()) continue;

	if (gotoBridgeEntryRotateToSeeBridge(TURN_ON_LEFT_WHEEL)) {
	    // use sioux, as the bridge is in the correct position.
	    bridge_ = BRIDGE_POS_CENTER;
	    bridgeDetectionByCenter_ = true;
	    Log->bridge(bridge_);
	    if (gotoBridgeEntry() &&
		crossBridge())
	    {
		skittleMiddleProcessed_ = true;
		return true;
	    }
	} else {
	    bridge_ = BRIDGE_POS_MIDDLE_BORDURE;
	    Log->bridge(bridge_);

	    unsigned int retries = 0;
	    while (retries < 5) {
		if (gotoBridgeEntry())
		    break;
		retries ++;
	    }
	    // if we failed 5 times try the whole thing again.
	    if (retries == 5) continue;
	    if (gotoBridgeEntryRotateToSeeBridge(TURN_ON_LEFT_WHEEL)) {
		if (crossBridge()) return true;
	    } else {
		bridge_ = BRIDGE_POS_BORDURE;
		Log->bridge(bridge_);
		if (gotoBridgeEntry() &&
		    crossBridge()) return true;
	    }
	}
	if (checkEndEvents()) return false;
    }
    return false;
}

/**
 * crosses the bridge without right bumper.
 */
bool StrategyAttackBadBumperCL::findAndCrossBridgeNoRightBumper() 
{
    bridgeDetectionByCenter_ = true;
    const bool TURN_ON_RIGHT_WHEEL = false;
    while (true) {
	bridge_ = BRIDGE_POS_CENTER;
	Log->bridge(bridge_);

	// try again, if failed...
	if (!gotoBridgeEntry()) continue;

	if (gotoBridgeEntryRotateToSeeBridge(TURN_ON_RIGHT_WHEEL)) {
	    // use sioux, as the bridge is in the correct position.
	    if (gotoBridgeEntry() &&
		crossBridge())
	    {
		skittleMiddleProcessed_ = true;
		return true;
	    }
	} else {
	    bridge_ = BRIDGE_POS_BORDURE;
	    Log->bridge(bridge_);

	    unsigned int retries = 0;
	    while (retries < 5) {
		if (gotoBridgeEntry())
		    break;
		retries ++;
	    }
	    // if we failed 5 times try the whole thing again.
	    if (retries == 5) continue;
	    if (gotoBridgeEntryRotateToSeeBridge(TURN_ON_RIGHT_WHEEL)) {
		if (crossBridge()) return true;
	    } else {
		bridge_ = BRIDGE_POS_MIDDLE_BORDURE;
		Log->bridge(bridge_);
		retries = 0;
		while (retries < 5) {
		    if (gotoBridgeEntry() &&
			crossBridge()) return true;
		    retries++;
		}
	    }
	}
	if (checkEndEvents()) return false;
    }
    return false;
}


// --------------------------------------------------------------------------
// Quand on est pret du pont, le robot tourne sur une roue pour voir s'il y 
// a vraiment un pont ou non, mais il ne met qu'une roue dans le trou! Ca 
// evite de tomber
// --------------------------------------------------------------------------
bool StrategyAttackBadBumperCL::gotoBridgeEntryRotateToSeeBridge(bool turnOnLeftWheel)
{
    bool result=true;
    if ((bridge_ == BRIDGE_POS_BORDURE)
        || (!bridgeDetectionByCenter_ 
            && bridge_ == BRIDGE_ENTRY_CENTER_Y)) {
        // il faut faire une rotation sur une roue pour voir si le pont 
        // et la et eviter de tomber comme une merde
        LOG_COMMAND("Rotate to detect Bridge\n");
	enableBridgeCaptors();
        MvtMgr->setRobotDirection(MOVE_DIRECTION_FORWARD); 
        Move->enableAccelerationController(true);
	if (turnOnLeftWheel)
	    Move->rotateOnWheel(+M_PI_4/1.8, true, -1, 15);
	else
	    Move->rotateOnWheel(-M_PI_4/1.8, false, -1, 15);
	bool dummyBumperEvt=true;
	while(dummyBumperEvt) {
	  Events->wait(evtEndMoveBridge);
	  Move->enableAccelerationController(false);
	  if (checkBridgeBumperEvent(dummyBumperEvt)) {
            result = false;
	    break;
	  } 
	}
	if (result && bridge_ == BRIDGE_POS_BORDURE) useBridgeBumpers_=false; // on sait que le pont est la
        if (checkEndEvents()) 
            return false;
	disableBridgeCaptors();
	Move->rotateOnWheel(0 , turnOnLeftWheel, 2, 30);
        Events->wait(evtEndMove);
        if (checkEndEvents()) 
            return false;
    }
    return result;
}
