/**
 * @file motor.h
 * 
 * @author Laurent Saint-Marcel
 *
 * Gestion des moteurs des roues motrices. Interface avec la carte HCTL/ODOM
 * sur port serie.
 */

#pragma once

#include "motor.h"


// ============================================================================
// ==============================  class Motor05     ==========================
// ============================================================================

class MotorOdom05;

/**
 * @class Motor05
 * Gestion des moteurs des roues utilisant les HCTLs
 */
class Motor05 : public MotorCL
{
 public:
    Motor05(MotorOdom05* motorOdomImpl,
            bool   automaticReset = true, 
            MotorAlertFunction fn = NULL);
  ~Motor05();

  bool exists() const { return true; }

  void start() {}

  bool isStarted() { return motorOdomImpl_ != NULL; }
  /** Reset les hctl (gauche et droite) */
  bool reset();
  /** Desasservit les moteurs */
  void idle();
  /** If useCache=true, motor::setSpeed set the speed and get the 
      motor & odometer position. If set to false, it only set the speed */
  void useCache(bool useCache);
  /** Defini la constante d'acceleration des moteurs */
  void setAcceleration(MotorAcceleration acceleration);

  /** Specifie un consigne en vitesse pour les moteurs */
  void setSpeed(MotorSpeed left, 
                MotorSpeed right);

  /** Retourne la position des codeurs */
  void getPosition(MotorPosition &left,
                   MotorPosition &right);
  
  /** Retourne la consigne reellement envoyee au moteurs */
  void getPWM(MotorPWM &left,
              MotorPWM &right);
 
 private:
  MotorOdom05* motorOdomImpl_;
  bool useCache_;

};

