#include "motor.h"
#ifndef TELECOMMAND_MAIN
#include "log.h"
#endif
#include "HCTLControl.h"
#include "events.h"
#include <unistd.h>


static const int MOTOR_PWM_TIME_ALERT=80;

// 80 = toutes les 2s

// ============================================================================
// =================================  class Motor   ===========================
// ============================================================================

/** Retourne true si le module est valide*/
bool Motor::validate()
{
  int lapCount=0;
  while(lapCount++<10){
    setSpeed(10,10); 
    MotorPosition pos1L, pos1R;
    getPosition(pos1L, pos1R);
    sleep(3);
    setSpeed(0,0);
    sleep(1);
    setSpeed(-10,-10);
    sleep(3);
    setSpeed(0,0);
    sleep(1);
  }
  return true;
}

/** Reset les hctl (gauche et droite) */
bool Motor::reset()
{
  counterLeft_=0;
  counterRight_=0;
  init_=true; 
  if (resetCallBack_) resetCallBack_();
  return init_;
}

void Motor::resetPwmAlert()
{
    counterLeft_=0;
    counterRight_=0;
}

/** Reset autimatiquement les hctl quand ils saturent trop longtemps */
void Motor::enableAutomaticReset(bool enable)
{
  enableAutomaticReset_ = enable;
}

/** Enregistre un callback appele quand les hctl saturent trop longtemps */
void Motor::registerPWMAlertFunction(MotorAlertFunction fn)
{
  alertFunction_ = fn;
}

/** Tache periodique qui verifie que les pwm ne depassent pas la limite */
void Motor::checkMotorEvents()
{
  bool alert=false;
  MotorPWM left=0;
  MotorPWM right=0;
  static MotorPWM lastLeft=0;
  static MotorPWM lastRight=0;
#ifndef TELECOMMAND_MAIN
  // quand il y a l'arret d'urgence, on reset les hctl une seule fois
  static int emergencyResetCounter=0;
  static int emergencyResetCounter2=0;
  if (Events->check(EVENTS_EMERGENCY_STOP)) {
      if (enableAutomaticReset_
          && emergencyResetCounter2 == emergencyResetCounter) {
          LOG_WARNING("Motor emergency stop reset\n");
          emergencyResetCounter++;
          reset();
      }
      return;
  }
  emergencyResetCounter2 = emergencyResetCounter;
#endif

  getPWM(left, right);
  // test la roue gauche
  if ((lastLeft>110 && left>110) ||
      (lastLeft<-110 && left<-110)) {
    if (counterLeft_++ > MOTOR_PWM_TIME_ALERT) {
#ifndef TELECOMMAND_MAIN
      LOG_ERROR("La roue gauche est bloquée r=%d l=%d\n", right, left);
#endif
      alert=true;
#ifndef TELECOMMAND_MAIN
      Events->raise(EVENTS_PWM_ALERT_LEFT);
#endif
    }
  } else {
    counterLeft_=0;
  }
  // test la roue droite
  if ((lastRight>110 && right>110) ||
      (lastRight<-110 && right<-110)) {
    if (counterRight_++ > MOTOR_PWM_TIME_ALERT) {
#ifndef TELECOMMAND_MAIN
      LOG_ERROR("La roue droite est bloquée r=%d l=%d, cr=%d, cl=%d\n", right, left, counterRight_, counterLeft_);
#endif
      alert=true;
#ifndef TELECOMMAND_MAIN
      Events->raise(EVENTS_PWM_ALERT_RIGHT);
#endif
    }
  } else {
    counterRight_=0;
  }
  lastLeft=left;
  lastRight=right;
  // on a un probleme sur une roue
  if (alert) {
    counterLeft_=0;
    counterRight_=0;
    if (enableAutomaticReset_) {
      reset();
    }
    if (alertFunction_) {
      alertFunction_(left, right);
    }
  }
}

/** Constructeur */
Motor::Motor(bool   automaticReset, 
	     MotorAlertFunction fn) : 
  RobotIODevice("Motor", CLASS_MOTOR), 
  alertFunction_(fn), enableAutomaticReset_(automaticReset), 
  counterLeft_(0), counterRight_(0), resetCallBack_(NULL)
{
}

void Motor::registerResetCallback(FunctionPtr cb)
{
    resetCallBack_ = cb;
}

