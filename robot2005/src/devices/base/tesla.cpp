#include "devices/tesla.h"

BigTeslaCL* BigTeslaCL::tesla_=NULL; 

BigTeslaCL::BigTeslaCL(): RobotDeviceCL("Tesla", CLASS_TESLA) {
    assert(!tesla_);
    tesla_ = this;
}

BigTeslaCL::~BigTeslaCL()
{
    tesla_ = NULL;
}
