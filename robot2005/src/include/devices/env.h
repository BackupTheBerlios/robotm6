/**
 * @file env.h
 *
 * @author Laurent Saint-Marcel
 *
 */

#ifndef __ENV_DETECTOR_H__
#define __ENV_DETECTOR_H__

#include "robotDevice.h"

#define  EnvDetector EnvDetectorCL::instance()

enum EnvDetectorDist {
    ENV_NO     = 0,
    ENV_FAR    = 1,
    ENV_MIDDLE = 2,
    ENV_NEAR   = 3
};


/**
 * @class EnvDetectorCL
 * Classe gerant les capteurs de distance sharps
 */

class EnvDetectorCL : public RobotDeviceCL {
 public:
  /** @brief Constructeur */
  EnvDetectorCL();
  virtual ~EnvDetectorCL();
  /** @brief Retourne l'instance unique*/
  static EnvDetectorCL* instance();
  virtual bool reset()  { return true; }
  virtual bool exists() const { return false; }

  // disable the events raised by a captor
  virtual void disableCaptor(unsigned int envId){}
  // return trhe captor distance
  virtual bool getEnvDetector(unsigned int envId, 
                              EnvDetectorDist& status) const { return false; }

  /** @brief function that read all captors and run the corresponding events */
  virtual void periodicTask(){}
  /** @brief read all captors status: do this before other get functions */
  virtual bool getAllCaptors() { return false; }

 private:
  static EnvDetectorCL*   envDetector_;
};

inline EnvDetectorCL* EnvDetectorCL::instance()
{
    assert(envDetector_);
    return envDetector_;
}

#endif // __ENV_DETECTOR_H__
