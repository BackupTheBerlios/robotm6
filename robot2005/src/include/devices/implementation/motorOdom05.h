/**
 * @file motorOdom05.h
 * 
 * @author Laurent Saint-Marcel
 *
 * Interface avec la carte HCTL/ODOM sur port serie.
 */

#pragma once

#include "robotDevice.h"

class IoDevice;

/**
 * @class Motor05Odom05
 * Gestion des moteurs des roues utilisant les HCTLs
 */
class MotorOdom05 : public RobotDeviceCL
{
 public:
    MotorOdom05();
    ~MotorOdom05();

  bool exists() const { return true; }

  /** Reset les hctl (gauche et droite) */
  bool reset(); 

  /** Desasservit les moteurs */
  bool idle();

  /** Defini la constante d'acceleration des moteurs */
  bool setAcceleration(MotorAcceleration acceleration);

  /** Specifie un consigne en vitesse pour les moteurs */
  bool setSpeed(MotorSpeed left, 
                MotorSpeed right);

  /** Retourne la position des codeurs */
  bool getMotorPosition(MotorPosition &left,
                        MotorPosition &right);
  
  /** Retourne la position des codeurs */
  bool getOdomPosition(CoderPosition &left,
                       CoderPosition &right);
  
  /** Retourne la consigne reellement envoyee au moteurs */
  bool getMotorPwm(MotorPWM &left,
                   MotorPWM &right);
  
  /** utilise un cache:ecrit la vitesse et recuppere tout de suite
      la position des codeurs et le pwm: ca evite de trop communiquer 
      sur la liaison serie */
  bool setSpeedAndCachePosition(MotorSpeed left, 
                                MotorSpeed right);
  
  void getCacheMotorPosition(MotorPosition &left,
                             MotorPosition &right); 

  void getCacheOdomPosition(CoderPosition &left,
                            CoderPosition &right);

  void getCacheMotorPwm(MotorPWM &left,
                        MotorPWM &right);
 private:
  IoDevice* device_;
  
  MotorPosition motorPosLeft_;
  MotorPosition motorPosRight_;
  CoderPosition odomPosLeft_;
  CoderPosition odomPosRight_;
  MotorPWM      motorPwmLeft_;
  MotorPWM      motorPwmRight_;
};

