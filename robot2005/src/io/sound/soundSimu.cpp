#include "sound.h"
#include "soundPlayer.h"

SoundSimuCL::SoundSimuCL() : SoundCL()
{
    SoundPlayerCL::instance(); // init soundPlayer if necessary
}

void SoundSimuCL::play(SoundId sound, 
                       SoundPriority priority)
{
    SoundPlayerCL::instance()->play(sound, priority);
}

void SoundSimuCL::clearStack()
{
    SoundPlayerCL::instance()->clearStack();
}
