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

enum BridgeCaptorStatus {
    BRIDGE_NO=0,
    BRIDGE_DECTECTED=1
};

enum BridgeCaptors {
    BRIDGE_SHARP_LEFT,
    BRIDGE_SHARP_CENTER,
    BRIDGE_SHARP_RIGHT,
    BRIDGE_BUMPER_LEFT,
    BRIDGE_BUMPER_RIGHT,

    BRIDGE_CAPTORS_NBR
};

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
  virtual bool exists() const { return false; }

  //... todo
  virtual bool getBridgeCaptors(BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR]) { return false; }
  virtual bool getEmergencyStop(bool& emergencyStop) { return false; }
  virtual bool getJackin(bool& jackin) { return false; }
  virtual bool getMatchSwitch(bool& match) { return false; }
  virtual bool getRebootSwitch(bool& reboot) { return false; }

  /** @brief function that read all captors and run the corresponding events */
  virtual void periodicTask() {}
  /** @brief read all captors status: do this before other get functions */
  virtual bool getAllCaptors() { return false; }

 private:
  static BumperCL*   bumper_;
};

inline BumperCL* BumperCL::instance()
{
    assert(bumper_);
    return bumper_;
}

#endif // __BUMPER_H__
