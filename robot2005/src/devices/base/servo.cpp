#include "devices/servo.h"

ServoCL* ServoCL::servo_=NULL; 

ServoCL::ServoCL(): RobotDeviceCL("Servo", CLASS_SERVO) {
    assert(!servo_);
    servo_ = this;
}

ServoCL::~ServoCL()
{
    servo_ = NULL;
}
