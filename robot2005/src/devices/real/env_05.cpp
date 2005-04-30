#define ENV_DETECTOR_INFO
#include "devices/implementation/env05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/envDetectorCom_05.h"

#ifdef TEST_MAIN
#define LOG_DEBUG_ON
#endif
#include "log.h"

EnvDetector05::EnvDetector05() : device_(NULL), data_(0), firstRead_(true)
{
    reset();
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

bool EnvDetector05::reset() 
{
    for(unsigned int i=0; i< ENV_DETECTOR_NBR; i++) {
        assert(EnvDetectorMapping[i].id == (EnvDetectorId)i);
        skipCaptor_[i] = EnvDetectorMapping[i].enableAtReset;
    }
    firstRead_ = true;
    return EnvDetectorCL::reset();
}

bool EnvDetector05::getEnvDetector(unsigned int envId, 
                                   EnvDetectorDist& status) const
{
    status = (EnvDetectorDist)((data_>>(2*envId))&(0x03));
    return true;
}

void EnvDetector05::disableCaptor(unsigned int captorId) 
{
    skipCaptor_[captorId] = true;
    Events->unraise(EnvDetectorMapping[captorId].evt);
}

/** @brief function that read all captors and run the corresponding events */
void EnvDetector05::periodicTask()
{
    unsigned char newData=0;
    if (getAllCaptors(newData)) { 
        for(unsigned int i=0; i< ENV_DETECTOR_NBR; i++) {
            if (skipCaptor_[i]) continue;
            EnvDetectorDist oldVal, newVal;
            oldVal = (EnvDetectorDist)((data_>>EnvDetectorMapping[i].bit) & 0x03);
            newVal = (EnvDetectorDist)((data_>>EnvDetectorMapping[i].bit) & 0x03);	
            if (firstRead_) {
                // initialisation
                if (!EnvDetectorMapping[i].reversed) {
                    if (newVal >= EnvDetectorMapping[i].eventDistLevel) {
                        Events->raise(EnvDetectorMapping[i].evt);
                    } else {
                        Events->unraise(EnvDetectorMapping[i].evt);
                    }
                } else {
                    if (newVal <= EnvDetectorMapping[i].eventDistLevel) {
                        Events->raise(EnvDetectorMapping[i].evt);
                    } else {
                        Events->unraise(EnvDetectorMapping[i].evt);
                    }
                }
            } else if (oldVal != newVal) {
                // mode normal
                if (!EnvDetectorMapping[i].reversed) {
                    if (newVal >= EnvDetectorMapping[i].eventDistLevel
                        && oldVal < EnvDetectorMapping[i].eventDistLevel) {
                        Events->raise(EnvDetectorMapping[i].evt);
                    } else if (oldVal >= EnvDetectorMapping[i].eventDistLevel 
                               && newVal < EnvDetectorMapping[i].eventDistLevel) {
                        Events->unraise(EnvDetectorMapping[i].evt);
                    }
                } else {
                    if (newVal <= EnvDetectorMapping[i].eventDistLevel
                        && oldVal > EnvDetectorMapping[i].eventDistLevel) {
                        Events->raise(EnvDetectorMapping[i].evt);
                    } else if (oldVal <= EnvDetectorMapping[i].eventDistLevel 
                               && newVal > EnvDetectorMapping[i].eventDistLevel) {
                        Events->unraise(EnvDetectorMapping[i].evt);
                    }
                }
            }
        }
        data_=newData;
        firstRead_ = false;
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

////////////////////////////////////////////////////////////////////////////////////

#ifdef TEST_MAIN

#include "io/serialPort.h"
int main(int argc, char* argv[]) 
{
    IoManager->submitIoHost(new SerialPort(0));
#ifndef GUMSTIX
    IoManager->submitIoHost(new SerialPort(1));
#else
    IoManager->submitIoHost(new SerialPort(2));
    IoManager->submitIoHost(new SerialPort(3));
#endif
    EventsManagerCL* evtMgr = new EVENTS_MANAGER_DEFAULT();

    EnvDetector05 env;
    while(true) {
        env.periodicTask();
        usleep(100000);
    }

    delete evtMgr;
    return 0;
}

#endif
