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

#include "robotTypes.h"
#include "robotBase.h"

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
 * @class Motor
 * Class virtuelle qui définit les fonctionnalités des moteurs des roues
 */
class Motor : public RobotIODevice
{
 public:
    /** Retourne true si le module est valide*/
    virtual bool validate();

    /** Reset les hctl (gauche et droite) */
    virtual bool reset();
    
    
    /** reset les compteurs qui verifie que les roues ne sont pas bloquees */
    void resetPwmAlert();

    /** Reset autimatiquement les hctl quand ils saturent trop longtemps */
    virtual void enableAutomaticReset(bool enable);

    /** Enregistre un callback appele quand les hctl saturent trop longtemps */
    virtual void registerPWMAlertFunction(MotorAlertFunction fn);

    /** Defini la constante d'acceleration des moteurs */
    virtual void setAcceleration(MotorAcceleration acceleration)=0;

    /** Specifie un consigne en vitesse pour les moteurs */
    virtual void setSpeed(MotorSpeed left, 
                          MotorSpeed right)=0;

    /** Retourne la position des codeurs */
    virtual void getPosition(MotorPosition &left,
                             MotorPosition &right)=0;

    /** Retourne la consigne reellement envoyee au moteurs */
    virtual void getPWM(MotorPWM &left,
                        MotorPWM &right)=0;
 
    /** Desasservit les moteurs */
    virtual void idle()=0;

    /** enregistre un callback execute quand les moteurs sont resete */
    void registerResetCallback(FunctionPtr cb);
    /** Fonction a appeler periodiquement pour verfifier ques les roues ne sont pas bloquees. 
        Si c'est le cas l'evenement EVENTS_PWM_ALERT est raise */
    void checkMotorEvents();

 private:
    MotorAlertFunction alertFunction_;
    bool enableAutomaticReset_;
    int counterLeft_, counterRight_;

 protected:
    /** Constructeur */
    Motor(bool               automaticReset = true, 
          MotorAlertFunction fn = NULL);

    FunctionPtr resetCallBack_;
};

// ============================================================================
// ==============================  class MotorSimu   ==========================
// ============================================================================

/**
 * @class MotorSimu
 * Gestion des moteurs des roues pour le simulateur
 */
class MotorSimu : public Motor
{
 public:
  MotorSimu(bool   automaticReset = true, 
	    MotorAlertFunction fn = NULL);
  
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
  bool ping()   { return true; }
  bool isSimu() const { return true; }
};

// ============================================================================
// ==============================  class MotorReal   ==========================
// ============================================================================

/**
 * @class MotorReal
 * Gestion des moteurs des roues utilisant les HCTLs
 */
class MotorReal : public Motor
{
 public:
  MotorReal(bool   automaticReset = true, 
	    MotorAlertFunction fn = NULL);
  ~MotorReal();

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

  bool ping() { return true; }
  bool isSimu() const { return false; }
};

#endif /* __MOTOR_H__ */
