#include "devices/implementation/skittleDetector05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/skittleDetectorCom_05.h"
#include "events.h"

#ifdef TEST_MAIN
#define LOG_DEBUG_ON
#endif

#include "log.h"

/** @brief Constructeur */
SkittleDetector05::SkittleDetector05(): 
    device_(NULL), enableEvents_(true), lastPos_(0)
{
    device_ = IoManager->getIoDevice(IO_ID_SKITTLE_DETECTOR_05);
    if (device_ != NULL) {
	if (device_->open()) {
	    LOG_OK("Initialization Done\n");
	} else {
	    device_=NULL;
	    LOG_ERROR("Device-open for SkittleDetector05 failed.\n");
	}
    } else {
        LOG_ERROR("SkittleDetector05 device not found!\n");
    } 
}
SkittleDetector05::~SkittleDetector05()
{
    if (device_) device_->close();  
}
/** @brief Retourne l'instance unique*/
bool SkittleDetector05::reset()
{
    return true;
}

// envoie la grue a une certaine position, retourne la derniere
// position detectee de quille, meme si la quille n'est plus detectee
// actuellement il y a l'ancienne valeur
bool SkittleDetector05::getValue(SkittlePosition& pos)
{
    pos=lastPos_;
    return true;
}
// active l'evenement de detection de la quille // par defaut a true
void SkittleDetector05::enableDetection()
{
    enableEvents_=true;
}
// desactive l'evenement de detection de la quille
void SkittleDetector05::disableDetection()
{
    enableEvents_=false;
}

void SkittleDetector05::periodicTask()
{
    if (getBasicValue() && 
        enableEvents_ && 
        (lastPos_ != 0)) {
        LOG_INFO("Skittle detected at pos:%d\n", lastPos_);
        Events->raise(EVENTS_SKITTLE_DETECTED);
    }
}

bool SkittleDetector05::getBasicValue()
{
    if (!device_) return false;
    unsigned char buf[2];
    if (device_->writeRead(SKITTLE_DETECTOR_REQ_GET_BASIC_POSITION, buf)) {
        lastPos_ = buf[0];
        return true;
    } else {
        LOG_DEBUG("SkittleDetector05 COM error\n");
        return false;
    }
}

#ifdef TEST_MAIN

#include "io/serialPort.h"
int main(int argc, char* argv[]) 
{
    IoManager->submitIoHost(new SerialPort(0, false));
    IoManager->submitIoHost(new SerialPort(1, false));
    IoManager->submitIoHost(new SerialPort(2, false));
    IoManager->submitIoHost(new SerialPort(3, false));  
    EventsManagerCL* evtMgr = new EVENTS_MANAGER_DEFAULT();

    SkittleDetector05 skittle;
    bool loop=true;
    while(loop) {
        skittle.periodicTask();
        usleep(200000);
    }
    printf("Bye\n");
    delete evtMgr;
    return 0;
}

#endif