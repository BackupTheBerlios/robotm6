#ifndef __ENV_DETECTOR__05_H__
#define __ENV_DETECTOR__05_H__

#include "env.h"

class IoDevice;

class EnvDetector05 : public EnvDetectorCL {
 public:
  /** @brief Constructeur */
  EnvDetector05();
  ~EnvDetector05();
  bool exists() const { return true; }

  bool getEnvDetector(int envId, 
                      EnvDetectorDist& status) const;

  /** @brief function that read all captors and run the corresponding events */
  void periodicTask();
  /** @brief read all captors status: do this before other get functions */
  bool getAllCaptors();

 private:
  IoDevice* device_;
};


#endif 
