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

#include "robotBase.h"
#include "soundList.h"
#include "robotConfig.h"

#define Sound SoundCL::instance()

typedef enum SoundPriority {
  /* @brief le son sera joue quand les autres qui ont ete ajoutes avant 
   * lui seront termines */
  SND_PRIORITY_PUSH_BACK,
  /** @brief efface la liste des sons, stop le son courant et joue ce 
   * nouveau son immediatement */
  SND_PRIORITY_URGENT
} SoundPriority;

/**
 * @class Sound
 * Classe permettant de jouer des sons. La liste des sons a jouer est envoyee
 * au player de son (un autre programme) par l'intermediaire d'une shared 
 * memory.
 * Le player de sons est un aute programme car comme il peut etre sensible
 * (risque de blocages ou de crashs et qu'il n'est pas indispensable, mieux
 *  vaux qu'il ne fasse pas parti du programme principal du robot
 */

class SoundCL: public RobotComponent {
 public:
  /** @brief Constructeur */
  SoundCL();
  virtual ~SoundCL();
  /** @brief Retourne l'instance unique de la class Sound */
  static SoundCL* instance();
  virtual bool reset(){return true;}
  virtual bool validate() {return false;}

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
   * @brief Dit la valeur d'un chiffre a virgule
   */
  virtual void sayNumber(double number){}

  /**
   * @brief Joue une musique de musicList
   */
  virtual void playMusic(int musicId){}

  /** 
   * @brief Attend que la liste de sons soit jouee et lance des 
   * musiques mp3 dans un ordre aleatoire
   */
  virtual void startMusic(){}
  /** 
   * @brief Attend que la liste de sons soit jouee et lance des 
   * musiques tahitiennes mp3 dans un ordre aleatoire
   */
  virtual void startMusicYukulele(){}
  /** 
   * @brief Arrete la musique
   * @see startMusic
   */
  virtual void stopMusic(){}
  /** 
   * @brief Arrete la music, les sons joues et efface la liste des sons a
   * jouer (sorte de reset)
   */
  virtual void clearStack(){}

 private:
  static SoundCL*   sound_;
};

class SoundSimuCL: public SoundCL {
 public:
  /** @brief Constructeur */
  SoundSimuCL();
  
  /** 
   * @brief Envoie un son dans la shared memory pour qu'il soit joue
   * @param priority SND_PRIORITY_PUSH_BACK: le son sera joue quand les 
   *        autres qui ont ete ajoutes avant lui seront termines\n
   *        SND_PRIORITY_URGENT: efface la liste des sons, stop le son
   *        courant et joue ce nouveau son immediatement
   */
  void play(SoundId sound, 
            SoundPriority priority=SND_PRIORITY_PUSH_BACK);
 
};

class Sound05CL: public SoundCL {
 public:
  /** @brief Constructeur */
    Sound05CL();
    bool reset(){return true;}
  /** 
   * @brief Envoie un son dans la shared memory pour qu'il soit joue
   * @param priority SND_PRIORITY_PUSH_BACK: le son sera joue quand les 
   *        autres qui ont ete ajoutes avant lui seront termines\n
   *        SND_PRIORITY_URGENT: efface la liste des sons, stop le son
   *        courant et joue ce nouveau son immediatement
   */
    void play(SoundId sound, 
              SoundPriority priority=SND_PRIORITY_PUSH_BACK);
 
};

// ---------------------------------------------------------------------------
// Sound::instance
// ---------------------------------------------------------------------------
inline SoundCL* SoundCL::instance()
{
    if (!sound_) {
        if (RobotConfig->soundSimu) sound_ = new SoundSimuCL();
        else sound_ = new Sound05CL();
    }
    return sound_;
}

#endif // __SOUND_H__
