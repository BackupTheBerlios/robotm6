/**
 * @file alim.h
 *
 * @author Laurent Saint-Marcel
 *
 */

#ifndef __ALIM_H__
#define __ALIM_H__

#include "robotDevice.h"

#define  Alim AlimCL::instance()




/**
 * @class AlimCL
 * Classe gerant la carte alim
 */

class AlimCL : public RobotDeviceCL {
 public:
  /** @brief Constructeur */
  AlimCL();
  virtual ~AlimCL();
  /** @brief Retourne l'instance unique*/
  static AlimCL* instance();
  virtual bool reset()  { return true; }
  virtual bool exists() const { return false; }
  virtual void emergencyStop() {  }
  /** @brief envoie le servo a un position donnee et l'asservi */
  virtual bool getAllTension(Millivolt tension[4]) { return false; }
  /** @brief passe en mode 12V */
  virtual bool mode12V() { return false; }
  /** @brief passe en mode 28V */
  virtual bool mode28V() { return false; }
  /** @brief passe en mode 42V */
  virtual bool mode42V() { return false; }
  /** @brief active le 12V 1*/
  virtual bool enable12V_1() { return false; }
  /** @brief active le 12V 1*/
  virtual bool disable12V_1() { return false; }
  /** @brief active le 12V 2*/
  virtual bool enable12V_2() { return false; }
  /** @brief active le 12V 2*/
  virtual bool disable12V_2() { return false; }

 private:
  static AlimCL*   alim_;
};

inline AlimCL* AlimCL::instance()
{
    assert(alim_);
    return alim_;
}

#endif // __SERVO_H__
