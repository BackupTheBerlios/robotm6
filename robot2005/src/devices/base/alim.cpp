#include "devices/alim.h"

AlimCL* AlimCL::alim_=NULL; 

AlimCL::AlimCL(): RobotDeviceCL("Alim", CLASS_ALIM) {
    assert(!alim_);
    alim_ = this;
}

AlimCL::~AlimCL()
{
    alim_ = NULL;
}
