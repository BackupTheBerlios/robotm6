#include "devices/implementation/env05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/envDetectorCom_05.h"

//#define LOG_DEBUG_ON
#include "log.h"

EnvDetector05::EnvDetector05() : device_(NULL), data_(0)
{
    device_ = IoManager->getIoDevice(IO_ID_ENV_05);
    if (device_ != NULL) {
	if (device_->open()) {
	    LOG_OK("Initialization Done\n");
	} else {
	    device_=NULL;
	    LOG_ERROR("Device-open for env Detector 05 failed.\n");
	}
    } else {
        LOG_ERROR("Env detector 05 device not found!\n");
    } 
}
EnvDetector05::~EnvDetector05()
{
    if (device_) device_->close();
}
bool EnvDetector05::getEnvDetector(int envId, 
                                   EnvDetectorDist& status) const
{
  status = (EnvDetectorDist)((data_>>(2*envId))&(0x03));
  return true;
}

/** @brief function that read all captors and run the corresponding events */
void EnvDetector05::periodicTask()
{
  unsigned char newData=0;
  if (getAllCaptors(newData)) {
    
    data_=newData;
  }
}

/** @brief read all captors status: do this before other get functions */
bool EnvDetector05::getAllCaptors()
{
  return getAllCaptors(data_);
}

/** @brief read all captors status: do this before other get functions */
bool EnvDetector05::getAllCaptors(unsigned char& data)
{
  if (!device_) return false;
  static bool firstError=true;
  bool status = device_->writeRead(ENV_DETECTOR_GET_DATA, &data);
  if (!status && firstError) {
    LOG_ERROR("EnvDetector05::getAllCaptors COM error\n");
  }
  firstError = status;
  if (status) {
    LOG_DEBUG("EnvDetector05::getAllCaptors : 0x%2.2x\n", data);
  }
  return false;
}
