#pragma once

#include "crane.h"


class IoDevice;

class Crane05 : public CraneCL {
 public:
  /** @brief Constructeur */
  Crane05();
  ~Crane05();
  /** @brief Retourne l'instance unique*/
  bool reset()  { return disableMotors(); }
  bool exists() const { return true; }
  // envoie la grue a une certaine position
  bool setPosition(CranePositionX x, 
                   CranePositionZ z);
  // arrete les moteurs
  bool disableMotors();
 private:
  IoDevice* device_;
};

