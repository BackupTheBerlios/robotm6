/**
 * @file soundPlayer.h
 *
 * @author Laurent Saint-Marcel
 *
 * Interface avec le lecteur MP3. La class Sound permet d'envoyer des
 * donnees au programme qui lit les MP3 par l'intermediare d'une shared memory
 * utilise par le simulator et le player de logs
 */

#ifndef __SOUND_PLAYER_H__
#define __SOUND_PLAYER_H__

#include "soundList.h"

/**
 * @class SoundPlayerCL
 * Classe permettant de jouer des sons. La liste des sons a jouer est envoyee
 * au player de son (un autre programme) par l'intermediaire d'une shared 
 * memory.
 * Le player de sons est un aute programme car comme il peut etre sensible
 * (risque de blocages ou de crashs et qu'il n'est pas indispensable, mieux
 *  vaux qu'il ne fasse pas parti du programme principal du robot
 */
struct SoundShmStruct;

class SoundPlayerCL {
 public:
  /** @brief Constructeur */
  SoundPlayerCL();
  virtual ~SoundPlayerCL();
  /** @brief Retourne l'instance unique de la class Sound */
  static SoundPlayerCL* instance();

  /** 
   * @brief Envoie un son dans la shared memory pour qu'il soit joue
   * @param priority SND_PRIORITY_PUSH_BACK: le son sera joue quand les 
   *        autres qui ont ete ajoutes avant lui seront termines\n
   *        SND_PRIORITY_URGENT: efface la liste des sons, stop le son
   *        courant et joue ce nouveau son immediatement
   */
  void play(SoundId sound, 
            SoundPriority priority=SND_PRIORITY_PUSH_BACK);
  
  
  /** 
   * @brief Arrete les sons joues et efface la liste des sons a
   * jouer (sorte de reset)
   */
  void clearStack();

 private: 
  void write(SoundId sound, int data=0);

  static SoundPlayerCL*   soundPlayer_; 
  SoundShmStruct  *shm_;
  int shmIndex_;
};


// ---------------------------------------------------------------------------
// SoundPlayerCL::instance
// ---------------------------------------------------------------------------
inline SoundPlayerCL* SoundPlayerCL::instance()
{
    if (!soundPlayer_) {
        soundPlayer_ = new SoundPlayerCL();
    }
    return soundPlayer_;
}

#endif // __SOUND_PLAYER_H__
