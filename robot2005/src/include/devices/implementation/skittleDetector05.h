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
  
  // retourne la derniere
  // position detectee de quille, meme si la quille n'est plus detectee
  // actuellement il y a l'ancienne valeur
  bool getLastValue(SkittlePosition& pos);
  // renvoie la valeur actuelle du capteur
  bool getBasicValue(SkittlePosition& pos);
  // active l'evenement de detection de la quille // par defaut a true
  void enableDetection();
  // desactive l'evenement de detection de la quille
  void disableDetection();
   // arrete les moteurs
  void periodicTask();

 private:
  IoDevice* device_;
  bool enableEvents_;
  SkittlePosition lastPos_;
  
};

