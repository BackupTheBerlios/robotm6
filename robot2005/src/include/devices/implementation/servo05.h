

#ifndef __SERVO_05_H__
#define __SERVO_05_H__

#include "servo.h"

class IoDevice;

class Servo05 : public ServoCL {
 public:
  /** @brief Constructeur */
  Servo05();
  ~Servo05();
  bool exists() const { return true; }
 

  /** @brief envoie le servo a un position donnee et l'asservi */
  bool setServoPosition(unsigned char servoId, 
                        unsigned char pos);
  /** @brief desasservi tous les servos */
  bool disableAll();
  /** @brief asservi tous les servo */
  bool enableAll();
 private :
  IoDevice* device_;
};


#endif
