/**
 * @file crane.h
 *
 * @author Laurent Saint-Marcel
 *
 */

// grue de l'electro aimant

#pragma once

#include "robotDevice.h"

#define SkittleDetector SkittleDetectorCL::instance()

typedef unsigned char SkittlePosition;

class SkittleDetectorCL : public RobotDeviceCL {
 public:
  /** @brief Constructeur */
  SkittleDetectorCL();
  virtual ~SkittleDetectorCL();
  /** @brief Retourne l'instance unique*/
  static SkittleDetectorCL* instance();
  virtual bool reset()  { return true; }
  virtual bool exists() const { return false; }
  
  // retourne la derniere
  // position detectee de quille, meme si la quille n'est plus detectee
  // actuellement il y a l'ancienne valeur
  virtual bool getLastValue(SkittlePosition& pos) { return false; }
  // mask pour dire quels sont les capteurs qui ne marchent pas au debut
  // du match = > on n'utilisera pas ces capteurs pour declencher l'event
  virtual void setMask(SkittlePosition mask) { mask_ = mask;}
  // renvoie la valeur actuelle du capteur
  virtual bool getBasicValue(SkittlePosition& pos) { return false; }
  // active l'evenement de detection de la quille // par defaut a true
  virtual void enableDetection() {}
  // desactive l'evenement de detection de la quille
  virtual void disableDetection() {}
  // arrete les moteurs
  virtual void periodicTask() {}

 private:
  static SkittleDetectorCL*   skittle_;
 protected:
  SkittlePosition mask_;
};

inline SkittleDetectorCL* SkittleDetectorCL::instance()
{
    assert(skittle_);
    return skittle_;
}
