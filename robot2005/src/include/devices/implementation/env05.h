#ifndef __ENV_DETECTOR__05_H__
#define __ENV_DETECTOR__05_H__

#include "env.h"
#include "envDetectorMapping.h"

class IoDevice;

class EnvDetector05 : public EnvDetectorCL {
 public:
  /** @brief Constructeur */
  EnvDetector05();
  ~EnvDetector05();
  bool exists() const { return true; }
  bool reset();

  bool getEnvDetector(unsigned int envId, 
                      EnvDetectorDist& status) const;

  void disableCaptor(unsigned int envId);
  /** @brief function that read all captors and run the corresponding events */
  void periodicTask();
  /** @brief read all captors status: do this before other get functions */
  bool getAllCaptors();

 private:  
  bool getAllCaptors(unsigned char& data);

  IoDevice* device_;
  unsigned char data_;  
  bool      firstRead_;
  bool      skipCaptor_[ENV_DETECTOR_NBR];
};


#endif 
