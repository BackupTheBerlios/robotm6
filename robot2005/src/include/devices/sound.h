/**
 * @file sound.h
 *
 * @author Laurent Saint-Marcel
 *
 * Interface avec le lecteur MP3. La class Sound permet d'envoyer des
 * donnees au programme qui lit les MP3 par l'intermediare d'une shared memory
 */

#ifndef __SOUND_H__
#define __SOUND_H__

#include "robotDevice.h"
#include "soundList.h"

#define Sound SoundCL::instance()

/**
 * @class SoundCL
 * Classe permettant de jouer des sons. La liste des sons a jouer est envoyee
 * a la carte lecteur MP3
 */

class SoundCL: public RobotDeviceCL {
 public:
  /** @brief Constructeur */
  SoundCL();
  virtual ~SoundCL();
  /** @brief Retourne l'instance unique de la class Sound */
  static SoundCL* instance();
  virtual bool exists() const { return false; }
  virtual bool reset() { clearStack(); return true; }
  /** 
   * @brief Envoie un son dans la shared memory pour qu'il soit joue
   * @param priority SND_PRIORITY_PUSH_BACK: le son sera joue quand les 
   *        autres qui ont ete ajoutes avant lui seront termines\n
   *        SND_PRIORITY_URGENT: efface la liste des sons, stop le son
   *        courant et joue ce nouveau son immediatement
   */
  virtual void play(SoundId sound, 
                    SoundPriority priority=SND_PRIORITY_PUSH_BACK){}
  
  /** 
   * @brief Arrete la music, les sons joues et efface la liste des sons a
   * jouer (sorte de reset)
   */
  virtual void clearStack(){}

 private:
  static SoundCL*   sound_;
};

// ---------------------------------------------------------------------------
// Sound::instance
// ---------------------------------------------------------------------------
inline SoundCL* SoundCL::instance()
{
    assert(sound_);
    return sound_;
}

#endif // __SOUND_H__
