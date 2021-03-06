#include "devices/motor.h"
#ifndef TELECOMMAND_MAIN
#include "move.h"
#include "log.h"
#endif
#include "events.h"
#include <unistd.h>

// 80 = toutes les 2s

// ============================================================================
// =================================  class MotorCL   ===========================
// ============================================================================

/** Reset les hctl (gauche et droite) */
bool MotorCL::reset()
{
  resetPwmAlert();
  if (resetCallBack_) resetCallBack_();  
  return true;
}

void MotorCL::resetPwmAlert()
{
    counterLeft_=0;
    counterRight_=0;
}

void MotorCL::setPwmAlertTimeout(int timeout)
{
    pwmAlertTimeout_ = timeout;
}
void MotorCL::setPwmAlertSeuil(MotorPWM seuil)
{
    pwmAlertSeuil_ = seuil;
}

/** Reset autimatiquement les hctl quand ils saturent trop longtemps */
void MotorCL::enableAutomaticReset(bool enable)
{
  enableAutomaticReset_ = enable;
}

/** Enregistre un callback appele quand les hctl saturent trop longtemps */
void MotorCL::registerPWMAlertFunction(MotorAlertFunction fn)
{
  alertFunction_ = fn;
}

/** Tache periodique qui verifie que les pwm ne depassent pas la limite */
void MotorCL::periodicTask()
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
  if ((lastLeft > pwmAlertSeuil_ && left > pwmAlertSeuil_) ||
      (lastLeft < -pwmAlertSeuil_ && left < -pwmAlertSeuil_)) {
    if (counterLeft_++ > pwmAlertTimeout_) {
#ifndef TELECOMMAND_MAIN
      LOG_ERROR("La roue gauche est bloqu�e r=%d l=%d, cr=%d, cl=%d\n", 
                right, left, counterRight_, counterLeft_);
#endif
      alert=true;
#ifndef TELECOMMAND_MAIN
      Events->raise(EVENTS_PWM_ALERT_LEFT);
      if (enableAutomaticReset_) {
	Move->idleMotorLeft();
      }
#endif
    }
  } else {
    counterLeft_=0;
  }
  // test la roue droite
  if ((lastRight > pwmAlertSeuil_ && right > pwmAlertSeuil_) ||
      (lastRight < -pwmAlertSeuil_ && right < -pwmAlertSeuil_)) {
    if (counterRight_++ > pwmAlertTimeout_) {
#ifndef TELECOMMAND_MAIN
      LOG_ERROR("La roue droite est bloqu�e r=%d l=%d, cr=%d, cl=%d\n",
                right, left, counterRight_, counterLeft_);
#endif
      alert=true;
#ifndef TELECOMMAND_MAIN
      Events->raise(EVENTS_PWM_ALERT_RIGHT);
      if (enableAutomaticReset_) {
	Move->idleMotorLeft();
      }
#endif
    }
  } else {
    counterRight_=0;
  }
  lastLeft=left;
  lastRight=right;
  // on a un probleme sur une roue
  if (alert) {
    resetPwmAlert();
    if (alertFunction_) {
      alertFunction_(left, right);
    }
  }
}

/** Constructeur */
MotorCL::MotorCL(bool   automaticReset, 
                 MotorAlertFunction fn) : 
  RobotDeviceCL("Motor", CLASS_MOTOR), 
  alertFunction_(fn), enableAutomaticReset_(automaticReset), 
  counterLeft_(0), counterRight_(0), resetCallBack_(NULL), 
  pwmAlertTimeout_(MOTOR_PWM_ALERT_TIMEOUT), 
  pwmAlertSeuil_(MOTOR_PWM_ALERT_SEUIL)
{
}

void MotorCL::registerResetCallback(FunctionPtr cb)
{
    resetCallBack_ = cb;
}

