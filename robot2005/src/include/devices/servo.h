/**
 * @file servo.h
 *
 * @author Laurent Saint-Marcel
 *
 */

#ifndef __SERVO_H__
#define __SERVO_H__

#include "robotDevice.h"

#define  Servo ServoCL::instance()




/**
 * @class ServoCL
 * Classe gerant les servo moteurs des catapultes
 */

class ServoCL : public RobotDeviceCL {
 public:
  /** @brief Constructeur */
  ServoCL();
  virtual ~ServoCL();
  /** @brief Retourne l'instance unique*/
  static ServoCL* instance();
  virtual bool reset()  { disableAll(); return true; }
  virtual bool exists() const { return false; }
  virtual void emergencyStop() { disableAll(); }
  /** @brief envoie le servo a un position donnee et l'asservi */
  virtual bool setServoPosition(unsigned char servoId, 
                                unsigned char pos) { return false; }
  /** @brief desasservi tous les servos */
  virtual bool disableAll() { return false; }
  /** @brief asservi tous les servo */
  virtual bool enableAll() { return false; }

 private:
  static ServoCL*   servo_;
};

inline ServoCL* ServoCL::instance()
{
    assert(servo_);
    return servo_;
}

#endif // __SERVO_H__
