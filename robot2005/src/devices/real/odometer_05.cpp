
#include "devices/implementation/odometer05.h"
#include "devices/implementation/motorOdom05.h"

Odometer05::Odometer05(MotorOdom05* motorOdomImpl) : 
    motorOdomImpl_(motorOdomImpl), useCache_(true)
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
    if (useCache_) {
        motorOdomImpl_->getCacheOdomPosition(left, right);
        return true;
    } else {
        return motorOdomImpl_->getOdomPosition(left, right);
    }
}
