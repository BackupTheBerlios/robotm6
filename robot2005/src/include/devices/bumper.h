/**
 * @file bumper.h
 *
 * @author Laurent Saint-Marcel
 *
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
  /** @brief Retourne l'instance unique*/
  static BumperCL* instance();
  virtual bool reset()  { return true; }
  virtual bool exists() { return false; }

  //... todo

 private:
  static BumperCL*   bumper_;
};

inline BumperCL* BumperCL::instance()
{
    assert(bumper_);
    return bumper_;
}

#endif // __BUMPER_H__
