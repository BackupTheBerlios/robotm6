#include "devices/crane.h"

CraneCL* CraneCL::crane_=NULL; 

CraneCL::CraneCL(): RobotDeviceCL("Crane", CLASS_CRANE) {
    assert(!crane_);
    crane_ = this;
}

CraneCL::~CraneCL()
{
    crane_ = NULL;
}
