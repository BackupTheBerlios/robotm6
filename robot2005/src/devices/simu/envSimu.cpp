#include "devices/implementation/envSimu.h"
/** @brief Constructeur */
EnvDetectorSimu::EnvDetectorSimu() {}


bool EnvDetectorSimu::getEnvDetector(unsigned int envId, 
                                     EnvDetectorDist& status) const
{
    return false;
}


/** @brief function that read all captors and run the corresponding events */
void EnvDetectorSimu::periodicTask()
{
    
}
/** @brief read all captors status: do this before other get functions */
bool EnvDetectorSimu::getAllCaptors()
{
    return false;
}
