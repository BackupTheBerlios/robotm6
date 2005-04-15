/**
 * @file crane.h
 *
 * @author Laurent Saint-Marcel
 *
 */

// grue de l'electro aimant

#pragma once

#include "robotDevice.h"

#define Crane CraneCL::instance()

typedef unsigned char CranePositionX;
typedef unsigned char CranePositionZ;

class CraneCL : public RobotDeviceCL {
 public:
  /** @brief Constructeur */
  CraneCL();
  virtual ~CraneCL();
  /** @brief Retourne l'instance unique*/
  static CraneCL* instance();
  virtual bool reset()  { return true; }
  virtual bool exists() const { return false; }
  virtual void emergencyStop() { disableMotors(); }
  // envoie la grue a une certaine position
  virtual bool setPosition(CranePositionX x, 
                           CranePositionZ z){ return false; }
  // arrete les moteurs
  virtual bool disableMotors() { return false; }
 private:
  static CraneCL*   crane_;
};

inline CraneCL* CraneCL::instance()
{
    assert(crane_);
    return crane_;
}

