#include "implementation/soundSimu.h"
#include "soundPlayer.h"

SoundSimu::SoundSimu()
{
    SoundPlayerCL::instance(); // init soundPlayer if necessary
}

void SoundSimu::play(SoundId sound, 
                       SoundPriority priority)
{
    SoundPlayerCL::instance()->play(sound, priority);
}

void SoundSimu::clearStack()
{
    SoundPlayerCL::instance()->clearStack();
}
