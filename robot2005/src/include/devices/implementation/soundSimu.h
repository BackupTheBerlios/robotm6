/**
 * @file sound.h
 *
 * @author Laurent Saint-Marcel
 *
 * Interface avec le lecteur MP3. La class Sound permet d'envoyer des
 * donnees au programme qui lit les MP3 par l'intermediare d'une shared memory
 */

#ifndef __SOUND_SIMU_H__
#define __SOUND_SIMU_H__

#include "sound.h"

class SoundSimu: public SoundCL {
 public:
  /** @brief Constructeur */
  SoundSimu();

  bool exists() const { return true; }

  void play(SoundId sound, 
            SoundPriority priority=SND_PRIORITY_PUSH_BACK);
  void clearStack();
};

#endif // __SOUND_H__
