#include "devices/implementation/servoSimu.h"
#include "simulatorClient.h"
#include "log.h"

ServoSimu::ServoSimu() {
    LOG_FUNCTION();
}


/** @brief envoie le servo a un position donnee et l'asservi */
bool ServoSimu::setServoPosition(unsigned char servoId, 
                                 unsigned char pos)
{
    return false;
}
