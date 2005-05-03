#include "devices/implementation/bumperSimu.h"
#include "simulatorClient.h"
#include "events.h"
#include "log.h"


BumperSimu::BumperSimu() : first_(true) {
    LOG_FUNCTION();
}

bool BumperSimu::getBridgeCaptors(BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR])
{
    captors[BRIDGE_SHARP_LEFT]   
        = (fabs(Simulator->getGroundDistance(300, 0.597, 0)) < 10) ? 
        BRIDGE_DETECTED:BRIDGE_NO;
    captors[BRIDGE_SHARP_RIGHT]  
        = (fabs(Simulator->getGroundDistance(300, -0.597, 0)) < 10) ? 
        BRIDGE_DETECTED:BRIDGE_NO;
    captors[BRIDGE_SHARP_CENTER] 
        = (fabs(Simulator->getGroundDistance(253, -0.159, 0)) < 10) ? 
        BRIDGE_DETECTED:BRIDGE_NO;

    captors[BRIDGE_BUMPER_LEFT] 
        = (fabs(Simulator->getGroundDistance(200, 0.996, 0)) < 10) ? 
        BRIDGE_DETECTED:BRIDGE_NO;
    captors[BRIDGE_BUMPER_RIGHT] 
        = (fabs(Simulator->getGroundDistance(200, -0.996, 0)) < 10) ? 
        BRIDGE_DETECTED:BRIDGE_NO;
    return true;
}

bool BumperSimu::getEmergencyStop(bool& emergencyStop)
{
    emergencyStop = Simulator->isEmergencyStop();
    return true;
}

bool BumperSimu::getJackin(bool& jackin)
{
    jackin = Simulator->isJackin();
    return true;
}

bool BumperSimu::getMatchSwitch(bool& match)
{
    match = true;
    return true;
}

bool BumperSimu::getRebootSwitch(bool& reboot)
{
    reboot = false;
    return true;
}


/** @brief function that read all captors and run the corresponding events */
void BumperSimu::periodicTask()
{
    bool jack=false;
    bool emergency=false;
    bool btnYes=false;
    bool btnNo=false; 
    BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR] = {BRIDGE_DETECTED, BRIDGE_DETECTED};
    usleep(50000);
    getBridgeCaptors(captors);
    //usleep(10000);
    getJackin(jack);
    //usleep(10000);
    getEmergencyStop(emergency);
    //usleep(10000);
    Simulator->getLcdButtonsState(btnYes, btnNo);
     
    if (!first_) {
        if (captors[BRIDGE_BUMPER_LEFT] == BRIDGE_NO && 
            oldBridge_[0] == BRIDGE_DETECTED) {
            Events->raise(EVENTS_NO_BRIDGE_BUMP_LEFT);
        } else if (captors[BRIDGE_BUMPER_LEFT] == BRIDGE_DETECTED && 
            oldBridge_[0] == BRIDGE_NO) {
            Events->raise(EVENTS_NO_BRIDGE_BUMP_LEFT);
        }
        
        if (captors[BRIDGE_BUMPER_RIGHT] == BRIDGE_NO && 
            oldBridge_[1] == BRIDGE_DETECTED) {
            Events->raise(EVENTS_NO_BRIDGE_BUMP_RIGHT);
        } else if (captors[BRIDGE_BUMPER_RIGHT] == BRIDGE_DETECTED && 
            oldBridge_[1] == BRIDGE_NO) {
            Events->raise(EVENTS_NO_BRIDGE_BUMP_RIGHT);
        }
        if (jack && !oldJack_) {
            Events->raise(EVENTS_JACKIN);
        } else if (!jack && oldJack_) {
            Events->unraise(EVENTS_JACKIN);
        } 
        if (emergency && !oldEmergency_) {
            Events->raise(EVENTS_EMERGENCY_STOP);
        } else if (!emergency && oldEmergency_) {
            Events->unraise(EVENTS_EMERGENCY_STOP);
        }  
        if (btnYes && !oldBtnYes_) {
            Events->raise(EVENTS_BUTTON_YES);
        } else if (!btnYes && oldBtnYes_) {
            Events->unraise(EVENTS_BUTTON_YES);
        } 
        if (btnNo && !oldBtnNo_) {
            Events->raise(EVENTS_BUTTON_NO);
        } else if (!btnNo && oldBtnNo_) {
            Events->unraise(EVENTS_BUTTON_NO);
        } 
    }

    oldBridge_[0] = captors[BRIDGE_BUMPER_LEFT];
    oldBridge_[1] = captors[BRIDGE_BUMPER_RIGHT];
    oldJack_      = jack;
    oldEmergency_ = emergency;
    oldBtnYes_    = btnYes;
    oldBtnNo_     = btnNo;
    first_        = false;
}

/** @brief read all captors status: do this before other get functions */
bool BumperSimu::getAllCaptors()
{
    return false;
}
