#include "devices/implementation/bumperSimu.h"
#include "simulatorClient.h"


BumperSimu::BumperSimu(){}

bool BumperSimu::getBridgeCaptors(BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR])
{
    return false;
}

bool BumperSimu::getEmergencyStop(bool& emergencyStop)
{
    return false;
}

bool BumperSimu::getJackin(bool& jackin)
{
    return false;
}

bool BumperSimu::getMatchSwitch(bool& match)
{
    return false;
}

bool BumperSimu::getRebootSwitch(bool& reboot)
{
    return false;
}


/** @brief function that read all captors and run the corresponding events */
void BumperSimu::periodicTask()
{
    
}

/** @brief read all captors status: do this before other get functions */
bool BumperSimu::getAllCaptors()
{
    return false;
}
