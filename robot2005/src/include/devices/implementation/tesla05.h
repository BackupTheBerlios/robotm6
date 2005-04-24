#pragma once

#include "tesla.h"

class IoDevice;

class BigTesla05 : public BigTeslaCL {
 public:
  /** @brief Constructeur */
  BigTesla05();
  ~BigTesla05();
  /** @brief Retourne l'instance unique*/
  bool reset()  { return true; }
  bool exists() const { return true; }
  void emergencyStop();
  /** verification que le detecteur d'accrochage marche,
      a faire avait le debut du match */
  bool testDetector(bool& result);
  /** ne demarrer le capteur qu'apres avoir demarre l'aimant */
  bool enableDetector();
  /** arreter le detecteur avant d'arreter l'aimant */
  bool disableDetector();
  /** arrete l'electro aimant */
  bool stopTesla(); 
  /** passer la carte alime dans le mode correspondant avant de 
      demarrer l'electroaimant! */
  bool startTesla(TeslaMode mode);
  /** tache peridoci qui verifie si on a accroche */
  void periodicTask(); 

 private:
  bool detectSkittleAttached(); 
  IoDevice* device_;
  bool detectorEnabled_;
};

