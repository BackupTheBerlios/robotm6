#include "devices/implementation/env05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/envDetectorCom_05.h"
#include "log.h"

EnvDetector05::EnvDetector05() : device_(NULL)
{
    device_ = IoManager->getIoDevice(IO_ID_ENV_05);
    if (device_ != NULL) {
	if (device_->open()) {
	    LOG_OK("Initialization Done\n");
	} else {
	    device_=NULL;
	    LOG_ERROR("device-open for alim 05 failed.\n");
	}
    } else {
        LOG_ERROR("alim 05 device not found!\n");
    } 
}
EnvDetector05::~EnvDetector05(){
    if (device_) device_->close();
}
bool EnvDetector05::getEnvDetector(int envId, 
                                   EnvDetectorDist& status) const
{
    return false;
}

/** @brief function that read all captors and run the corresponding events */
void EnvDetector05::periodicTask()
{

}
/** @brief read all captors status: do this before other get functions */
bool EnvDetector05::getAllCaptors()
{
    return false;
}
