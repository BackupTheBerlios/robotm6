

#ifndef __SERVO_SIMU_H__
#define __SERVO_SIMU_H__

#include "servo.h"


class ServoSimu : public ServoCL {
 public:
  /** @brief Constructeur */
  ServoSimu();

  bool exists() const { return true; }
 

  /** @brief envoie le servo a un position donnee et l'asservi */
  bool setServoPosition(unsigned char servoId, 
                        unsigned char pos);
  /** @brief desasservi tous les servos */
  bool disableAll() { return true; }
  /** @brief asservi tous les servo */
  bool enableAll() { return true; }
};


#endif
