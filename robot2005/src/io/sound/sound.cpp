#include "sound.h"

SoundCL* SoundCL::sound_=NULL;

SoundCL::SoundCL() : RobotComponent("Sound", CLASS_SOUND)
{
    if (sound_) delete sound_;
    sound_=this;
}

SoundCL::~SoundCL()
{
    sound_ = NULL;
}
