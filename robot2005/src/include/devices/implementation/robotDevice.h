#ifndef __ROBOT_DEVICE_H__
#define __ROBOT_DEVICE_H__

#include "robotBase.h"
/** @brief classe dont herite toutes les cartes du robot */
class RobotDeviceCL : public RobotBase {
 public:
    RobotDeviceCL(const char* name, ClassId classId);
    /** @brief Fonction appelée quand l'utilisateur a appuyé sur l'arret d'urgence */
    virtual void emergencyStop()=0;
    /** @brief Renvoie vrai si le composant est réel */
    virtual bool exists() const =0;
    /** @brief Appele quand on redemare une strategie */
    virtual void reset() {};
    /** @brief Tache periodic appelee par le robotDevices callback */
    virtual void periodicTask() {};
};

inline RobotDeviceCL::RobotDeviceCL(const char* name, ClassId classId) 
    : RobotBase(name, classId)
{
}

#endif
