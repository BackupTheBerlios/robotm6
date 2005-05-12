
// gestion de l'electroaimant

#pragma once

#include "robotDevice.h"

#define BigTesla BigTeslaCL::instance()
#define Tesla    BigTeslaCL::instance()

enum TeslaMode {
    TESLA_MODE_0V = 0,
//    TESLA_MODE_5V, // envoie du petit sur l'electro aimant
    TESLA_MODE_12V,
    TESLA_MODE_28V,
    TESLA_MODE_42V // envoie du tres gros! : la on doit attirer toutes les quilles a 3 m a la ronde !
};


class BigTeslaCL : public RobotDeviceCL {
 public:
  /** @brief Constructeur */
  BigTeslaCL();
  virtual ~BigTeslaCL();
  /** @brief Retourne l'instance unique*/
  static BigTeslaCL* instance();
  virtual bool reset()  { return true; }
  virtual bool exists() const { return false; }
  
  /** verification que le detecteur d'accrochage marche,
      a faire avait le debut du match */
  virtual bool testDetector(bool& result) { return false;}
  /** ne demarrer le capteur qu'apres avoir demarre l'aimant */
  virtual bool enableDetector(){ return false; }
  /** arreter le detecteur avant d'arreter l'aimant */
  virtual bool disableDetector(){ return false; }
  /** arrete l'electro aimant */
  virtual bool stopTesla() { return false; }
  /** passer la carte alime dans le mode correspondant avant de 
      demarrer l'electroaimant! */
  virtual bool startTesla(TeslaMode mode) { return false; }
  /** tache peridoci qui verifie si on a accroche */
  virtual void periodicTask() {}

 private:
  static BigTeslaCL*   tesla_;
};

inline BigTeslaCL* BigTeslaCL::instance()
{
    assert(tesla_);
    return tesla_;
}
