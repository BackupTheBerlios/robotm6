#include "robotDevices.h"
#include "robotConfig2005.h"
#include "implementation/motorSimu.h"
#include "implementation/motorIsa.h"
#include "implementation/soundSimu.h"
#include "implementation/lcdSimu.h"
#include "implementation/odometerSimu.h"

RobotDevicesCL::RobotDevicesCL() :
    RobotBase("Robot devices", CLASS_ROBOT_DEVICES),
    motor_(NULL), odometer_(NULL), lcd_(NULL), bumper_(NULL), sound_(NULL)
                                   
{
    
}

RobotDevicesCL::~RobotDevicesCL()
{
    if (motor_)    { delete motor_;    motor_=NULL; }
    if (odometer_) { delete odometer_; odometer_=NULL; }
    if (lcd_)      { delete lcd_;      lcd_=NULL; }
    if (bumper_)   { delete bumper_;   bumper_=NULL; }
    if (sound_)    { delete sound_;    sound_=NULL; }
}

/** @brief detecte et cree toutes les cartes */
void RobotDevicesCL::allocDevices()
{
    // TODO

    if (RobotConfig2005->motorSimu) {
        motor_ = new MotorSimu(RobotConfig->automaticMotorReset);
    } else {
        if (RobotConfig->isMotorISA) {
            motor_ = new MotorIsa(RobotConfig->automaticMotorReset);
        } else {
            motor_ = new MotorCL(RobotConfig->automaticMotorReset);
        }
    }
    if (RobotConfig2005->lcdSimu) {
        lcd_   = new LcdSimu();
    }
    if (RobotConfig2005->soundSimu) {
        sound_   = new SoundSimu();
    }
    if (RobotConfig2005->odometerSimu) {
        odometer_ = new OdometerSimu();
    }
}

/** @brief Envoie le signal d'arret d'urgence a toutes les cartes */
void RobotDevicesCL::emergencyStop()
{
    if (motor_)    motor_->emergencyStop();
    if (odometer_) odometer_->emergencyStop();
    if (lcd_)      lcd_->emergencyStop();
    if (bumper_)   bumper_->emergencyStop();
    if (sound_)    sound_->emergencyStop();
}

/** @brief Envoie le signal de reset a toutes les cartes */
void RobotDevicesCL::reset()
{
    if (motor_)    motor_->reset();
    if (odometer_) odometer_->reset();
    if (lcd_)      lcd_->reset();
    if (bumper_)   bumper_->reset();
    if (sound_)    sound_->reset();
}
