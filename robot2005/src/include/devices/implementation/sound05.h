/**
 * @file sound.h
 *
 * @author Laurent Saint-Marcel
 *
 * Interface avec le lecteur MP3. La class Sound permet d'envoyer des
 * donnees au programme qui lit les MP3 par l'intermediare d'une shared memory
 */

#ifndef __SOUND_05_H__
#define __SOUND_05_H__

#include "sound.h"

class IoDevice;

class Sound05CL: public SoundCL {
 public:
  /** @brief Constructeur */
    Sound05CL();
    ~Sound05CL();

    bool exists() const { return true; }

    /** 
     * @brief Envoie un son dans la shared memory pour qu'il soit joue
     * @param priority SND_PRIORITY_PUSH_BACK: le son sera joue quand les 
     *        autres qui ont ete ajoutes avant lui seront termines\n
     *        SND_PRIORITY_URGENT: efface la liste des sons, stop le son
     *        courant et joue ce nouveau son immediatement
     */
    void play(SoundId sound, 
              SoundPriority priority=SND_PRIORITY_PUSH_BACK);
    void clearStack();

 private:
    IoDevice* device_;
};


#endif // __SOUND_H__
