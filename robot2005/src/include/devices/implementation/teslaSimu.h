#pragma once

#include "tesla.h"

class BigTeslaSimu : public BigTeslaCL {
 public:
  /** @brief Constructeur */
    BigTeslaSimu() {}
  /** @brief Retourne l'instance unique*/
  bool reset()  { return true; }
  bool exists() const { return true; }
  
  /** verification que le detecteur d'accrochage marche,
      a faire avait le debut du match */
  bool testDetector(bool& result) { result=true; return true; }
  /** ne demarrer le capteur qu'apres avoir demarre l'aimant */
  bool enableDetector() { return true; }
  /** arreter le detecteur avant d'arreter l'aimant */
  bool disableDetector() { return true; }
  /** arrete l'electro aimant */
  bool stopTesla();
  /** passer la carte alime dans le mode correspondant avant de 
      demarrer l'electroaimant! */
  bool startTesla(TeslaMode mode);
  /** tache peridoci qui verifie si on a accroche */
  void periodicTask(); 
};

