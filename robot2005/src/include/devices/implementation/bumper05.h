/**
 * @file bumper05.h
 *
 * @author Laurent Saint-Marcel
 *
 */

#ifndef __BUMPER_05_H__
#define __BUMPER_05_H__

#include "devices/bumper.h"
#include "bumperCom_05.h"
class IoDevice;

class Bumper05: public BumperCL {
 public:
  /** @brief Constructeur */
  Bumper05();
  ~Bumper05();

  bool reset();
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
  bool getAllCaptors(unsigned char data[BUMPER_DATA_NBR]);
 
 private: 
  bool getValue(int byte, int bit, bool reversed); 
  
  unsigned char data_[BUMPER_DATA_NBR]; 
  IoDevice* device_;
  bool      firstRead_;
};

#endif // __BUMPER_05_H__
