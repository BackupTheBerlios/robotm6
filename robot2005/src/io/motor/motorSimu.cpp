#include "motor.h"
#include "log.h"
#include "simulatorClient.h"

// ============================================================================
// ==============================  class MotorSimu   ==========================
// ============================================================================

/** Reset les hctl (gauche et droite) */
bool MotorSimu::reset()
{
  LOG_FUNCTION();
  Motor::reset();
  
  init_=true;
  return init_;
}

/** Defini la constante d'acceleration des moteurs */
void MotorSimu::setAcceleration(MotorAcceleration acceleration)
{

}

/** Specifie un consigne en vitesse pour les moteurs */
void MotorSimu::setSpeed(MotorSpeed left, 
			 MotorSpeed right)
{
  LOG_DEBUG("set speed(%d, %d)\n", (int)left, (int)right);
  //Simulator::instance()->setSpeed(left, right);
}

/** Retourne la position des codeurs */
void MotorSimu::getPosition(MotorPosition &left,
                            MotorPosition &right)
{
  //Simulator::instance()->getMotorPosition(left, right);
}

/** Retourne la consigne reellement envoyee au moteurs */
void MotorSimu::getPWM(MotorPWM &left,
		       MotorPWM &right)
{
  //Simulator::instance()->getPwm(left, right);
}

/** desasserrvit les moteurs */
void MotorSimu::idle()
{
}

MotorSimu::MotorSimu(bool   automaticReset, 
		     MotorAlertFunction fn) :
  Motor(automaticReset, fn)
{
  LOG_FUNCTION();
  reset();
  LOG_OK("Initialisation termin�e\n");
}

