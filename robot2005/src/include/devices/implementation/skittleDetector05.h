#pragma once

#include "skittleDetector.h"

class IoDevice;

class SkittleDetector05 : public SkittleDetectorCL {
 public:
  /** @brief Constructeur */
  SkittleDetector05();
  ~SkittleDetector05();
  /** @brief Retourne l'instance unique*/
  bool reset();
  bool exists() const { return true; }
  
  // envoie la grue a une certaine position, retourne la derniere
  // position detectee de quille, meme si la quille n'est plus detectee
  // actuellement il y a l'ancienne valeur
  bool getValue(SkittlePosition& pos);
  // active l'evenement de detection de la quille // par defaut a true
  void enableDetection();
  // desactive l'evenement de detection de la quille
  void disableDetection();
   // arrete les moteurs
  void periodicTask();

 private:
  bool SkittleDetector05::getBasicValue();

  IoDevice* device_;
  bool enableEvents_;
  SkittlePosition lastPos_;
  
};

