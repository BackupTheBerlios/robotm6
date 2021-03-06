#include "robotDevices.h"
#include "robotConfig2005.h"

#include "implementation/motorSimu.h"
#include "implementation/odometerSimu.h"
#include "implementation/lcdSimu.h"
#include "implementation/bumperSimu.h"
#include "implementation/envSimu.h"
#include "implementation/servoSimu.h"
#include "implementation/soundSimu.h"
#include "implementation/alimSimu.h"
#include "implementation/craneSimu.h"
#include "implementation/skittleDetectorSimu.h"
#include "implementation/teslaSimu.h"

#include "implementation/odometer04.h" // 2004
#include "implementation/odometer05.h"
#ifndef GUMSTIX
#include "implementation/motorIsa.h" // 2004
#endif
#include "implementation/motor05.h"
#include "implementation/motorOdom05.h"
#include "implementation/lcd05.h"
#include "implementation/bumper05.h"
#include "implementation/env05.h"
#include "implementation/servo05.h"
#include "implementation/sound05.h"
#include "implementation/alim05.h"
#include "implementation/crane05.h"
#include "implementation/skittleDetector05.h"
#include "implementation/tesla05.h"

#include "io/ioManager.h"
#include "io/serialPort.h"
#include "log.h"

namespace {
    pthread_mutex_t repositoryLock = PTHREAD_MUTEX_INITIALIZER;
    void no_op(...){};
}
RobotDevicesCL* RobotDevicesCL::instance_=NULL;

void* RobotDevicesThreadBody(void* robotDevices)
{
    while(1)
    {
	usleep(LOOP_THREAD_SLEEP);
	static_cast<RobotDevicesCL*>(robotDevices)->periodicTask();
    }
    return NULL;
}

RobotDevicesCL::RobotDevicesCL() :
    RobotBase("Robot devices", CLASS_ROBOT_DEVICES),
    motorOdom_(NULL), motor_(NULL), odometer_(NULL), lcd_(NULL), bumper_(NULL),
    sound_(NULL), env_(NULL), servo_(NULL), crane_(NULL), tesla_(NULL),
    skittle_(NULL), alim_(NULL)
{
    instance_=this;
    LOG_FUNCTION();
    thread_ = 0;
    bool init = (MTHREAD_CREATE("RobotDevices Thread",
				&thread_,
				NULL,
				RobotDevicesThreadBody,
				this)
		 == 0);
    if (init) {
	LOG_OK("Initialisation Done\n");
    } else {
	LOG_ERROR("Can't run RobotDevices-thread.");
    }
}

RobotDevicesCL::~RobotDevicesCL()
{
    for (unsigned int i = 0; i < toDelete_.size(); ++i)
	delete toDelete_[i];
    
    if (motorOdom_){ delete motorOdom_;motorOdom_=NULL; }
    if (motor_)    { delete motor_;    motor_=NULL; }
    if (odometer_) { delete odometer_; odometer_=NULL; }
    if (lcd_)      { delete lcd_;      lcd_=NULL; }
    if (bumper_)   { delete bumper_;   stopWatching(bumper_); bumper_=NULL; }
    if (sound_)    { delete sound_;    sound_=NULL; }
    if (env_)      { delete env_;      stopWatching(env_); env_=NULL; }
    if (servo_)    { delete servo_;    servo_=NULL; }
    if (crane_)    { delete crane_;    sound_=NULL; }
    if (tesla_)    { delete tesla_;    stopWatching(tesla_); tesla_=NULL; }
    if (skittle_)  { delete skittle_;  stopWatching(skittle_); skittle_=NULL; }
    if (alim_)     { delete alim_;     alim_=NULL; }
    instance_=NULL;
}

/** @brief detecte et cree toutes les cartes */
void RobotDevicesCL::allocDevices()
{
    LOG_FUNCTION();
    toDelete_.push_back(new SerialPort(0, SERIAL_SPEED_38400));
#ifndef GUMSTIX
    toDelete_.push_back(new SerialPort(1, SERIAL_SPEED_38400));
#else
    toDelete_.push_back(new SerialPort(2, SERIAL_SPEED_38400));
    toDelete_.push_back(new SerialPort(3, SERIAL_SPEED_38400));
#endif
    for (unsigned int i = 0; i < toDelete_.size(); ++i)
	IoManager->submitIoHost(toDelete_[i]);

    allocMotorOdom();
    allocLcd();
    allocSound();
    allocBumper();
    allocEnv();
    allocServo();
    allocCrane();
    allocSkittle();
    allocTesla();
    allocAlim();
}

void RobotDevicesCL::rescan()
{
    IoManager->rescan();
    allocMotorOdom();
    allocLcd();
    allocSound();
    allocBumper();
    allocEnv();
    allocServo();
    allocCrane();
    allocSkittle();
    allocTesla();
    allocAlim();
}
void RobotDevicesCL::allocMotorOdom()
{
    if (IoManager->getIoDevice(IO_ID_MOTOR_ODOM_05)) {
        if (motorOdom_ && !motorOdom_->exists()) delete motorOdom_;
        motorOdom_ = new MotorOdom05();
    }
    allocMotor();
    allocOdometer();
}

void RobotDevicesCL::allocMotor()
{
    if (RobotConfig2005->hasMotor) {
        if (motor_ && motor_->exists()) return;
        if (RobotConfig2005->motorSimu) {
            if (motor_ && !motor_->exists()) delete motor_;
            motor_ = new MotorSimu(RobotConfig->automaticMotorReset);
        } else {
#ifndef GUMSTIX
            if (RobotConfig->isMotorISA) {
                if (motor_ && !motor_->exists()) delete motor_;
                motor_ = new MotorIsa(RobotConfig->automaticMotorReset);
            } else
#endif
	    {
                if (motorOdom_ != NULL) {
                    if (motor_ && !motor_->exists()) delete motor_;
                    motor_ = new Motor05(motorOdom_);
                } else {
                    if (!motor_) motor_ = new MotorCL(RobotConfig->automaticMotorReset);
                }
            }
        }
    }
}

void RobotDevicesCL::allocOdometer()
{
    if (RobotConfig2005->hasOdometer) { 
        if (odometer_ && odometer_->exists()) return; 
        if (RobotConfig2005->odometerSimu) {
            if (odometer_ && !odometer_->exists()) delete odometer_;
            odometer_ = new OdometerSimu();
        } else {
            if (motorOdom_ != NULL) {
                if (odometer_ && !odometer_->exists()) delete odometer_;
                odometer_ = new Odometer05(motorOdom_);
            }
            else if (IoManager->getIoDevice(IO_ID_ODOMETER_04)) {
                if (odometer_ && !odometer_->exists()) delete odometer_;
                odometer_ = new Odometer_04();
            } else {
                if(!odometer_) odometer_ = new OdometerCL();
            }
        }
    }
}
void RobotDevicesCL::allocLcd()
{
    if (RobotConfig2005->hasLcd) {  
        if (lcd_ && lcd_->exists()) return; 
        if (RobotConfig2005->lcdSimu) {
            if (lcd_ && !lcd_->exists()) delete lcd_; 
            lcd_   = new LcdSimu();
        } else {
            if (IoManager->getIoDevice(IO_ID_LCD_05)) {
                if (lcd_ && !lcd_->exists()) delete lcd_; 
                lcd_ = new Lcd_05();
            } else {
                if (!lcd_) lcd_ = new LcdCL();
            }
        }
    }

}

void RobotDevicesCL::allocSound()
{
    if (RobotConfig2005->hasSound) {
        if (sound_ && sound_->exists()) return; 
        if (RobotConfig2005->soundSimu) {
            if (sound_ && !sound_->exists()) delete sound_; 
            sound_   = new SoundSimu();
        } else {
            if (IoManager->getIoDevice(IO_ID_SOUND_05)) {
                if (sound_ && !sound_->exists()) delete sound_; 
                sound_ = new Sound05CL();
            } else {
                if (!sound_) sound_ = new SoundCL();
            }
        }
    }
}

void RobotDevicesCL::allocBumper()
{
    if (RobotConfig2005->hasBumper) {
        if (bumper_ && bumper_->exists()) return; 
        if (RobotConfig2005->bumperSimu) {
            if (bumper_ && !bumper_->exists()) delete bumper_; 
            bumper_ = new BumperSimu();
            startWatching(bumper_);
        } else {
            if (IoManager->getIoDevice(IO_ID_BUMPER_05)) {
                if (bumper_ && !bumper_->exists()) delete bumper_; 
                bumper_ = new Bumper05();
                startWatching(bumper_);
            } else {
                if (!bumper_) bumper_ = new BumperCL();
            }
        }
    }
}

void RobotDevicesCL::allocEnv()
{
    if (RobotConfig2005->hasEnv) {
        if (env_ && env_->exists()) return; 
        if (RobotConfig2005->envSimu) {
            if (env_ && !env_->exists()) delete env_; 
            env_ = new EnvDetectorSimu();
            startWatching(env_);
        } else {
            if (IoManager->getIoDevice(IO_ID_ENV_05)) {
                if (env_ && !env_->exists()) delete env_; 
                env_ = new EnvDetector05();
                startWatching(env_);
            } else {
                if (!env_) env_ = new EnvDetectorCL();
            }
        }
    }
}

void RobotDevicesCL::allocServo()
{
    if (RobotConfig2005->hasServo) {
        if (servo_ && servo_->exists()) return; 
        if (RobotConfig2005->servoSimu) {
            if (servo_ && !servo_->exists()) delete servo_; 
            servo_ = new ServoSimu();
        } else {
            if (IoManager->getIoDevice(IO_ID_SERVO_05)) {
                if (servo_ && !servo_->exists()) delete servo_; 
                servo_ = new Servo05();
            } else {
                if (!servo_) servo_ = new ServoCL();
            }
        }
    } 
}

void RobotDevicesCL::allocCrane()
{
    if (RobotConfig2005->hasCrane) {
        if (crane_ && crane_->exists()) return; 
        if (RobotConfig2005->craneSimu) {
            if (crane_ && !crane_->exists()) delete crane_; 
            crane_ = new CraneSimu();
        } else {
            if (IoManager->getIoDevice(IO_ID_CRANE_05)) {
                if (crane_ && !crane_->exists()) delete crane_; 
                crane_ = new Crane05();
            } else {
                if (!crane_) crane_ = new CraneCL();
            }
        }
    }
}

void RobotDevicesCL::allocSkittle()
{
    if (RobotConfig2005->hasSkittleDetector) {
        if (skittle_ && skittle_->exists()) return; 
        if (RobotConfig2005->skittleDetectorSimu) {
            if (skittle_ && !skittle_->exists()) delete skittle_;
            skittle_ = new SkittleDetectorSimu();
        } else {
            if (IoManager->getIoDevice(IO_ID_SKITTLE_DETECTOR_05)) {
                if (skittle_ && !skittle_->exists()) delete skittle_;
                skittle_ = new SkittleDetector05();
                startWatching(skittle_);
            } else {
                if (!skittle_) skittle_ = new SkittleDetectorCL();
            }
        }
    }
}

void RobotDevicesCL::allocTesla()
{
    if (RobotConfig2005->hasTesla) {
        if (tesla_ && tesla_->exists()) return; 
        if (RobotConfig2005->teslaSimu) {
            if (tesla_ && !tesla_->exists()) delete tesla_; 
            tesla_ = new BigTeslaSimu();
        } else {
            if (IoManager->getIoDevice(IO_ID_TESLA_05)) {
                if (tesla_ && !tesla_->exists()) delete tesla_; 
                tesla_ = new BigTesla05();
                startWatching(tesla_);
            } else {
                if (!tesla_) tesla_ = new BigTeslaCL();
            }
        }
    }
}

void RobotDevicesCL::allocAlim()
{
    if (RobotConfig2005->hasAlim) {
        if (alim_ && alim_->exists()) return; 
        if (RobotConfig2005->alimSimu) {
            if (alim_ && !alim_->exists()) delete alim_;
            alim_ = new AlimSimu();
        } else {
            if (IoManager->getIoDevice(IO_ID_ALIM_05)) {
                if (alim_ && !alim_->exists()) delete alim_;
                alim_ = new Alim05();
            } else {
                if (!alim_) alim_ = new AlimCL();
            }
        }
    }
}

/** @brief Envoie le signal d'arret d'urgence a toutes les cartes */
void RobotDevicesCL::emergencyStop()
{
    if (motorOdom_)motorOdom_->emergencyStop();
    if (motor_)    motor_->emergencyStop();
    if (odometer_) odometer_->emergencyStop();
    if (lcd_)      lcd_->emergencyStop();
    if (bumper_)   bumper_->emergencyStop();
    if (sound_)    sound_->emergencyStop();
    if (env_)      env_->emergencyStop();
    if (servo_)    servo_->emergencyStop();
    if (skittle_)  skittle_->emergencyStop();
    if (tesla_)    tesla_->emergencyStop();
    if (crane_)    crane_->emergencyStop();
    if (alim_)    alim_->emergencyStop();
}

/** @brief Envoie le signal de reset a toutes les cartes */
void RobotDevicesCL::reset()
{
    if (motorOdom_)motorOdom_->reset();
    if (motor_)    motor_->reset();
    if (odometer_) odometer_->reset();
    if (lcd_)      lcd_->reset();
    if (bumper_)   bumper_->reset();
    if (sound_)    sound_->reset();
    if (env_)      env_->reset();
    if (servo_)    servo_->reset();
    if (skittle_)  skittle_->reset();
    if (tesla_)    tesla_->reset();
    if (crane_)    crane_->reset();
    if (alim_)     alim_->reset();
}

void RobotDevicesCL::periodicTask()
{
    Lock localLock(&repositoryLock);
    RobotDevicesSet::iterator it;
    for (it = watchedDevices_.begin();
	 it != watchedDevices_.end();
	 ++it) {
	(*it)->periodicTask();
    }
}

void RobotDevicesCL::startWatching(RobotDeviceCL* device)
{
    Lock localLock(&repositoryLock);
    watchedDevices_.insert(device);
}

void RobotDevicesCL::stopWatching(RobotDeviceCL* device)
{
    Lock localLock(&repositoryLock);
    RobotDevicesSet::iterator it = watchedDevices_.find(device);
    if (it != watchedDevices_.end()) {
        watchedDevices_.erase(it);
    }
}
