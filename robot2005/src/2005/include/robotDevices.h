#ifndef __ROBOT_DEVICES_H__
#define __ROBOT_DEVICES_H__

#include "mthread.h"
#include <set>
#include "robotBase.h"
#include "devices/lcd.h"
#include "devices/odometer.h"
#include "devices/motor.h"
#include "devices/bumper.h"
#include "devices/sound.h"

/** @brief Initialisation des cartes electroniques */
class RobotDevicesCL : public RobotBase {
 public:
    RobotDevicesCL();
    virtual ~RobotDevicesCL();

    /** @brief detecte et cree toutes les cartes */
    void allocDevices();

    /** @brief Envoie le signal d'arret d'urgence a toutes les cartes */
    void emergencyStop();

    /** @brief reset all devices */
    void reset();

    MotorCL*    getMotor() const    { return motor_; }
    OdometerCL* getOdometer() const { return odometer_; }
    LcdCL*      getLcd() const      { return lcd_; }
    BumperCL*   getBumper() const   { return bumper_; }
    SoundCL*    getSound() const    { return sound_; }

protected: // periodic
    friend void* RobotDevicesThreadBody(void*);
    void periodicTask();

    void startWatching(RobotDeviceCL* device);
    void stopWatching(RobotDeviceCL* device);

protected: // periodic fields
    pthread_t thread_;
    typedef std::set<RobotDeviceCL*> RobotDevicesSet;
    RobotDevicesSet watchedDevices_;
    
private:
    MotorCL*    motor_;
    OdometerCL* odometer_;
    LcdCL*      lcd_;
    BumperCL*   bumper_;
    SoundCL*    sound_;
};

#endif
