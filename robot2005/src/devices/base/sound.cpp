#include "devices/sound.h"

SoundCL* SoundCL::sound_=NULL;

SoundCL::SoundCL() : RobotDeviceCL("Sound", CLASS_SOUND)
{
    if (sound_) delete sound_;
    sound_=this;
}

SoundCL::~SoundCL()
{
    sound_ = NULL;
}
