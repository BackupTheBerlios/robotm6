#include "devices/skittleDetector.h"

SkittleDetectorCL* SkittleDetectorCL::skittle_=NULL; 

SkittleDetectorCL::SkittleDetectorCL(): RobotDeviceCL("Skittle Detector", CLASS_SKITTLE_DETECTOR), mask_(0) {
    assert(!skittle_);
    skittle_ = this;
}

SkittleDetectorCL::~SkittleDetectorCL()
{
    skittle_ = NULL;
}
