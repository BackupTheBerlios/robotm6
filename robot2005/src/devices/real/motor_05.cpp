#include "devices/implementation/motor05.h"
#include "devices/implementation/motorOdom05.h"

Motor05::Motor05(MotorOdom05* motorOdomImpl,
                 bool   automaticReset, 
                 MotorAlertFunction fn) : 
    MotorCL(automaticReset, fn),
    motorOdomImpl_(motorOdomImpl),  useCache_(true)
{

}

Motor05::~Motor05()
{

}

bool Motor05::reset()
{
    if (!motorOdomImpl_) return false;
    return motorOdomImpl_->reset();
}
/** Desasservit les moteurs */
void Motor05::idle() 
{
    reset();
}

/** If useCache=true, motor::setSpeed set the speed and get the 
    motor & odometer position. If set to false, it only set the speed */
void Motor05::useCache(bool useCache)
{
    useCache_ = useCache;
}

/** Defini la constante d'acceleration des moteurs */
void Motor05::setAcceleration(MotorAcceleration acceleration)
{
    if (!motorOdomImpl_) return;
    motorOdomImpl_->setAcceleration(acceleration);
}

/** Specifie un consigne en vitesse pour les moteurs */
void Motor05::setSpeed(MotorSpeed left, 
                       MotorSpeed right)
{
    if (!motorOdomImpl_) return;
    if (useCache_) {
        motorOdomImpl_->setSpeedAndCachePosition(left, right);
    } else {
        motorOdomImpl_->setSpeed(left, right);
    }
}

/** Retourne la position des codeurs */
void Motor05::getPosition(MotorPosition &left,
                          MotorPosition &right)
{
    if (useCache_) {
        motorOdomImpl_->getCacheMotorPosition(left, right);
    } else {
        motorOdomImpl_->getMotorPosition(left, right);
    }
}

/** Retourne la consigne reellement envoyee au moteurs */
void Motor05::getPWM(MotorPWM &left,
                     MotorPWM &right)
{
    if (useCache_) {
        motorOdomImpl_->getCacheMotorPwm(left, right);
    } else {
        motorOdomImpl_->getMotorPwm(left, right);
    }
}
