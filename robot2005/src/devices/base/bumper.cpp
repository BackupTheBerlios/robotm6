/**
 * @file bumper.h
 *
 * @author Laurent Saint-Marcel
 *
 */

#include "bumper.h"

BumperCL* BumperCL::bumper_=NULL; 

BumperCL::BumperCL(): RobotDeviceCL("Bumper", CLASS_BUMPER) {
    assert(!bumper_);
    bumper_ = this;
}

BumperCL::~BumperCL()
{
    bumper_ = NULL;
}
