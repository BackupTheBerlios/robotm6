#include "devices/implementation/servo05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/servoCom_05.h"
#include "log.h"
Servo05::Servo05() : device_(NULL)
{
    device_ = IoManager->getIoDevice(IO_ID_SERVO_05);
    if (device_ != NULL) {
	if (device_->open()) {
	    LOG_OK("Initialization Done\n");
	} else {
	    device_=NULL;
	    LOG_ERROR("device-open for servo 05 failed.\n");
	}
    } else {
        LOG_ERROR("servo 05 device not found!\n");
    } 
}

Servo05::~Servo05()
{
    if (device_) device_->close();
}
/** @brief envoie le servo a un position donnee et l'asservi */
bool Servo05::setServoPosition(unsigned char servoId, 
                                 unsigned char pos)
{
    return false;
}

/** @brief desasservi tous les servos */
bool Servo05::disableAll()
{
    return false;
}
/** @brief asservi tous les servo */
bool Servo05::enableAll()
{
    return false;
}
