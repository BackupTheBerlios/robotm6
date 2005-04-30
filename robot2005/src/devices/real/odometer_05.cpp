
#include "devices/implementation/odometer05.h"
#include "devices/implementation/motorOdom05.h"
#include "log.h"

Odometer05::Odometer05(MotorOdom05* motorOdomImpl) : 
  motorOdomImpl_(motorOdomImpl), useCache_(true), oldLeft_(0), oldRight_(0)
{
    
}

 
bool Odometer05::reset()
{
    return true;
}

void Odometer05::useCache(bool useCache)
{
    useCache_ = useCache; 
}

bool Odometer05::getCoderPosition(CoderPosition &left,
                                  CoderPosition &right)
{
    if (!motorOdomImpl_) return false;
    bool status = true;
    if (useCache_) {
        motorOdomImpl_->getCacheOdomPosition(left, right);
    } else {
        status = motorOdomImpl_->getOdomPosition(left, right);
    } 
    if (!status) return false;
    //   LOG_INFO("left=%d, right=%d\n", oldLeft_-left, oldRight_-right);
    if (abs(oldLeft_-left) < notMovingSeuil_) {
      notMovingCounterLeft_++;
    } else {
      notMovingCounterLeft_ = 0;
    }
    if (abs(oldRight_-right) < notMovingSeuil_) {
      notMovingCounterRight_++;
    } else {
      notMovingCounterRight_ = 0;
    }
    oldLeft_=left;
    oldRight_=right;
    return true;
}
