/**
 * @file bumperSimu.h
 *
 * @author Laurent Saint-Marcel
 *
 */

#ifndef __BUMPER_SIMU_H__
#define __BUMPER_SIMU_H__

#include "devices/bumper.h"


class BumperSimu: public BumperCL {
 public:
  /** @brief Constructeur */
  BumperSimu();

  bool exists() const { return true; }
  
  bool getBridgeCaptors(BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR]);
  bool getEmergencyStop(bool& emergencyStop);
  bool getJackin(bool& jackin);
  bool getMatchSwitch(bool& match);
  bool getRebootSwitch(bool& reboot);

  /** @brief function that read all captors and run the corresponding events */
  void periodicTask();
  /** @brief read all captors status: do this before other get functions */
  bool getAllCaptors();

 private:
  unsigned char data_[3];
};

#endif // __BUMPER_SIMU_H__
