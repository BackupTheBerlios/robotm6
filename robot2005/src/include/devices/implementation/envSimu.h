#ifndef __ENV_DETECTOR_SIMU_H__
#define __ENV_DETECTOR_SIMU_H__

#include "env.h"


class EnvDetectorSimu : public EnvDetectorCL {
 public:
  /** @brief Constructeur */
  EnvDetectorSimu();
  bool exists() const { return true; }

  bool getEnvDetector(int envId, 
                      EnvDetectorDist& status) const;

  /** @brief function that read all captors and run the corresponding events */
  void periodicTask();
  /** @brief read all captors status: do this before other get functions */
  bool getAllCaptors();
};


#endif 
