/**
 * @file env.h
 *
 * @author Laurent Saint-Marcel
 *
 */

#include "env.h"

EnvDetectorCL* EnvDetectorCL::envDetector_=NULL; 

EnvDetectorCL::EnvDetectorCL(): RobotDeviceCL("EnvDetector", CLASS_ENV_DETECTOR) {
    assert(!envDetector_);
    envDetector_ = this;
}

EnvDetectorCL::~EnvDetectorCL()
{
    envDetector_ = NULL;
}
