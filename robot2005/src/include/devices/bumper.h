/**
 * @file sound.h
 *
 * @author Laurent Saint-Marcel
 *
 * Interface avec le lecteur MP3. La class Sound permet d'envoyer des
 * donnees au programme qui lit les MP3 par l'intermediare d'une shared memory
 */

#ifndef __BUMPER_H__
#define __BUMPER_H__

#include "robotDevice.h"

#define Bumper BumperCL::instance()

/**
 * @class BumperCL
 * Classe permettant de jouer des sons. La liste des sons a jouer est envoyee
 * a la carte lecteur MP3
 */

class BumperCL: public RobotDeviceCL {
 public:
  /** @brief Constructeur */
  BumperCL();
  virtual ~BumperCL();
  /** @brief Retourne l'instance unique de la class Sound */
  static BumperCL* instance();
  virtual bool reset()  { return true; }
  virtual bool exists() { return false; }

  //... todo

 private:
  static BumperCL*   bumper_;
};

// ---------------------------------------------------------------------------
// Sound::instance
// ---------------------------------------------------------------------------
inline BumperCL* BumperCL::instance()
{
    assert(bumper_);
    return bumper_;
}

#endif // __BUMPER_H__
