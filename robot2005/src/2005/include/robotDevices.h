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
#include "devices/servo.h"
#include "devices/env.h"
#include "devices/crane.h"
#include "devices/skittleDetector.h"
#include "devices/tesla.h"

class MotorOdom05;

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
    EnvDetectorCL*getEnv() const    { return env_; }
    ServoCL*    getServo() const    { return servo_; }
    CraneCL*    getCrane() const    { return crane_; }
    BigTeslaCL* getTesla() const    { return tesla_; }
    SkittleDetectorCL* getSkittle() const    { return skittle_; }

protected: // periodic
    friend void* RobotDevicesThreadBody(void*);
    void periodicTask();

    void startWatching(RobotDeviceCL* device);
    void stopWatching(RobotDeviceCL* device);

protected: // periodic fields
    pthread_t thread_;
    typedef std::set<RobotDeviceCL*> RobotDevicesSet;
    RobotDevicesSet watchedDevices_;

    void allocMotorOdom();
    void allocMotor();
    void allocOdometer();
    void allocLcd();
    void allocSound();
    void allocBumper();
    void allocEnv();
    void allocServo();
    void allocCrane();
    void allocSkittle();
    void allocTesla();

private:
    MotorOdom05*   motorOdom_;
    MotorCL*       motor_;
    OdometerCL*    odometer_;
    LcdCL*         lcd_;
    BumperCL*      bumper_;
    SoundCL*       sound_;
    EnvDetectorCL* env_;
    ServoCL*       servo_;
    CraneCL*       crane_;
    BigTeslaCL*    tesla_;
    SkittleDetectorCL* skittle_;
};

#endif
