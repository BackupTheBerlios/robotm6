#pragma once

#include "crane.h"



class CraneSimu : public CraneCL {
 public:
  /** @brief Constructeur */
  CraneSimu(){}
  /** @brief Retourne l'instance unique*/
  bool exists() const { return true; }
  // envoie la grue a une certaine position
  bool setPosition(CranePositionX x, 
                   CranePositionZ z);

};

