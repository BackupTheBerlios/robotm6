/**
 * @file odometer.h
 * 
 * @author Laurent Saint-Marcel
 *
 * Interface avec la carte odometre qui est connectee sur un port serie et
 * qui donne la position du robot. En mode automatique, des qu'on recoit la 
 * donnee des deux codeurs, on fait appel aux methodes de robotPosition pour
 * mettre a jour la position du robot. Si la carte ne repond plus, 
 * robotPosition s'en rend compte et utilise les codeurs des roues motrices 
 * pour calculer la position du robot
 */

#pragma once

#include "odometer.h"

// forward-declarations
class IoDevice;

// ===========================================================================
// class Odometer_04
// ===========================================================================

/** 
 * @class Odometer_04
 * Implementation des fonctions de class Odometer dans le cas d'une carte sur 
 * l'UART. Protocole de communication avec la carte 2004
 */
class Odometer_04 : public OdometerCL
{
 public:
    Odometer_04();
  ~Odometer_04();

  bool exists() const { return true; }

  /** @brief Reset the Odometer uart */
  bool reset();

  /**
   * @brief Met a jour la position des 2 codeurs (sans conversion) 
   * En mode automatique la carte envoie toute seule des donnees et 
   *   getCoderPosition retourne true si la carte a mise a jour la 
   *   position des codeurs depuis la derniere requete getCoderPosition
   * En mode manuel getCoderPosition va directement lire la valeur 
   *   des codeurs et retourne true si la communication s'est bien passee.
   */
  bool getCoderPosition(CoderPosition &left,
                        CoderPosition &right);

  

 protected:
//  bool filter(Byte byte);
//  friend bool Odometer04Filter(Byte byte);

 private:
  IoDevice* device_;
  CoderPosition left_;
  CoderPosition right_;
  int mode_; // 1 = automatic, 0=manual
  int bufIndex_;
  unsigned char bufferPosition_[10];
  bool dataAvailable_;
};
