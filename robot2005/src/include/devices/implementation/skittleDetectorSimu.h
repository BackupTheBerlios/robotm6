#pragma once

#include "skittleDetector.h"


class SkittleDetectorSimu : public SkittleDetectorCL {
 public:
  /** @brief Constructeur */
    SkittleDetectorSimu() : detectionEnabled_(false) {}
  /** @brief Retourne l'instance unique*/
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
  bool detectionEnabled_;
};

