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
  
  // envoie la grue a une certaine position, retourne la derniere
  // position detectee de quille, meme si la quille n'est plus detectee
  // actuellement il y a l'ancienne valeur
  virtual bool getValue(SkittlePosition& pos){ false; }
  // active l'evenement de detection de la quille // par defaut a true
  virtual void enableDetection() {}
  // desactive l'evenement de detection de la quille
  virtual void disableDetection() {}
  // arrete les moteurs
  virtual void periodicTask() {}

 private:
  static SkittleDetectorCL*   skittleDetector_;
};

inline SkittleDetectorCL* SkittleDetectorCL::instance()
{
    assert(skittleDetector_);
    return skittleDetector_;
}
