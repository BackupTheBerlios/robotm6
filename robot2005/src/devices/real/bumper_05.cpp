#include "devices/implementation/bumper05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/bumperCom_05.h"
#include "log.h"

/** @brief Constructeur */
Bumper05::Bumper05() : device_(NULL) 
{
    device_ = IoManager->getIoDevice(IO_ID_BUMPER_05);
    if (device_ != NULL) {
	if (device_->open()) {
	    LOG_OK("Initialization Done\n");
	} else {
	    device_=NULL;
	    LOG_ERROR("device-open for bumper 05 failed.\n");
	}
    } else {
        LOG_ERROR("bumper 05 device not found!\n");
    } 
}

Bumper05::~Bumper05() 
{
    if (device_) device_->close();
}

bool  Bumper05::getBridgeCaptors(BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR])
{
    return false;
}

bool  Bumper05::getEmergencyStop(bool& emergencyStop)
{
    return false;
}

bool  Bumper05::getJackin(bool& jackin)
{
    return false;
}

bool  Bumper05::getMatchSwitch(bool& match)
{
    return false;
}

bool  Bumper05::getRebootSwitch(bool& reboot)
{
    return false;
}


/** @brief function that read all captors and run the corresponding events */
void  Bumper05::periodicTask()
{
    
}
/** @brief read all captors status: do this before other get functions */
bool  Bumper05::getAllCaptors()
{
    return false;
}

 
