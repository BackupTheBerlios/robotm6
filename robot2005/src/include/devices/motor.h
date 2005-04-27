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

#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "robotDevice.h"

// ============================================================================
// ===============================  typedef   =================================
// ============================================================================

typedef void (*MotorAlertFunction)(MotorPWM left, MotorPWM right);

// ============================================================================
// ================================  const MOTOR_  ============================
// ============================================================================



// ============================================================================
// ================================  class Motor   ============================
// ============================================================================
/**
 * @class MotorCL
 * Class virtuelle qui définit les fonctionnalités des moteurs des roues
 */
class MotorCL : public RobotDeviceCL
{
 public: 
    /** Constructeur */
    MotorCL(bool               automaticReset = true, 
            MotorAlertFunction fn = NULL);
    virtual bool exists() const { return false; }
    virtual void emergencyStop() { reset(); }

    virtual void start() {};
    virtual bool isStarted() { return true; };
    /** Reset les hctl (gauche et droite) */
    virtual bool reset();

    virtual void dumpMotorStats() {}
    
    /** reset les compteurs qui verifie que les roues ne sont pas bloquees */
    void resetPwmAlert();

    /** Reset autimatiquement les hctl quand ils saturent trop longtemps */
    virtual void enableAutomaticReset(bool enable);

    /** Enregistre un callback appele quand les hctl saturent trop longtemps */
    virtual void registerPWMAlertFunction(MotorAlertFunction fn);

    /** Defini la constante d'acceleration des moteurs */
    virtual void setAcceleration(MotorAcceleration acceleration){}

    /** Specifie un consigne en vitesse pour les moteurs */
    virtual void setSpeed(MotorSpeed left, 
                          MotorSpeed right){}

    /** Retourne la position des codeurs */
    virtual void getPosition(MotorPosition &left,
                             MotorPosition &right){}

    /** Retourne la consigne reellement envoyee au moteurs */
    virtual void getPWM(MotorPWM &left,
                        MotorPWM &right){}
 
    /** Desasservit les moteurs */
    virtual void idle(){reset();}
    virtual void idleLeft(){reset();}
    virtual void idleRight(){ reset();}
    // asservi les moteurs
    virtual void unidle(){ setSpeed(0,0);} 

    /** enregistre un callback execute quand les moteurs sont resete */
    void registerResetCallback(FunctionPtr cb);
    /** Fonction a appeler periodiquement pour verfifier ques les roues ne sont
        pas bloquees.  Si c'est le cas l'evenement EVENTS_PWM_ALERT est raise */
    void periodicTask();

 private:
    MotorAlertFunction alertFunction_;
    bool enableAutomaticReset_;
    int counterLeft_, counterRight_;

 protected:
    FunctionPtr resetCallBack_;
};

#endif /* __MOTOR_H__ */
