#ifndef __ROBOT_DEVICE_H__
#define __ROBOT_DEVICE_H__

#include "robotBase.h"
/** @brief classe dont herite toutes les cartes du robot */
class RobotDeviceCL : public RobotBase {
 public:
    RobotDeviceCL(const char* name, ClassId classId);
    /** @brief Fonction appel�e quand l'utilisateur a appuy� sur l'arret d'urgence */
    virtual void emergencyStop() {}
    /** @brief Renvoie vrai si le composant est r�el */
    virtual bool exists() const = 0;
    /** @brief Appele quand on redemarre une strategie */
    virtual bool reset() { return true; }
    /** @brief Tache periodic appelee par le robotDevices callback */
    virtual void periodicTask() {}
};

inline RobotDeviceCL::RobotDeviceCL(const char* name, ClassId classId) 
    : RobotBase(name, classId)
{
}

#endif
