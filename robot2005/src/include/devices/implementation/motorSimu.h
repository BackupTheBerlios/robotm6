/**
 * @file motor.h
 * 
 * @author Laurent Saint-Marcel
 *
 * Gestion des moteurs des roues motrices. Interface avec la carte HCTL 
 * du PC104.
 * Attention un seul thread peut acceder a ces commandes car on fait des 
 * acces direct sur le port ISA du PC et seul 1 thread peut y acceder, 
 * celui qui a cree l'instance de motor
 */

#ifndef __MOTOR_SIMU_H__
#define __MOTOR_SIMU_H__

#include "motor.h"


// ============================================================================
// ==============================  class MotorSimu   ==========================
// ============================================================================

/**
 * @class MotorSimu
 * Gestion des moteurs des roues pour le simulateur
 */
class MotorSimu : public MotorCL
{
 public:
  MotorSimu(bool   automaticReset = true, 
	    MotorAlertFunction fn = NULL);
  bool exists() const { return true; }
  /** Reset les hctl (gauche et droite) */
  bool reset();
  
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
     /** Desasservit les moteurs */
  void idle();  
};

#endif /* __MOTOR_SIMU_H__ */
