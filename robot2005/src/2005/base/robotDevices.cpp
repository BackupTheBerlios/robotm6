#include "robotDevices.h"
#include "robotConfig2005.h"

#include "implementation/motorSimu.h"
#include "implementation/odometerSimu.h"
#include "implementation/lcdSimu.h"
#include "implementation/bumperSimu.h"
#include "implementation/envSimu.h"
#include "implementation/servoSimu.h"
#include "implementation/soundSimu.h"
#include "implementation/craneSimu.h"
#include "implementation/skittleDetectorSimu.h"
#include "implementation/teslaSimu.h"

#include "implementation/odometer04.h" // 2004
#include "implementation/odometer05.h"
#include "implementation/motorIsa.h" // 2004
#include "implementation/motor05.h"
#include "implementation/motorOdom05.h"
#include "implementation/lcd05.h"
#include "implementation/bumper05.h"
#include "implementation/env05.h"
#include "implementation/servo05.h"
#include "implementation/sound05.h"
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

void* RobotDevicesThreadBody(void* robotDevices)
{
    while(1)
    {
	// TODO: remove magic number and replace with constant [flo]
	usleep(10000);
	static_cast<RobotDevicesCL*>(robotDevices)->periodicTask();
    }
    return NULL;
}

RobotDevicesCL::RobotDevicesCL() :
    RobotBase("Robot devices", CLASS_ROBOT_DEVICES),
    motorOdom_(NULL), motor_(NULL), odometer_(NULL), lcd_(NULL), bumper_(NULL),
    sound_(NULL), env_(NULL), servo_(NULL), crane_(NULL), tesla_(NULL),
    skittle_(NULL)
{
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
}

/** @brief detecte et cree toutes les cartes */
void RobotDevicesCL::allocDevices()
{
    LOG_FUNCTION();
    //TODO split alloc function
//    IoManager->submitIoHost(new SerialPort(0, false));
    // TODO: keep reference for later deletion of serialPort
    IoManager->submitIoHost(new SerialPort(1, false));

    allocMotorOdom();
}

void RobotDevicesCL::allocMotorOdom()
{
    if (IoManager->getIoDevice(IO_ID_MOTOR_ODOM_05)) {
        motorOdom_ = new MotorOdom05();
    }
    allocMotor();
    allocOdometer();
    allocLcd();
    allocSound();
    allocBumper();
    allocEnv();
    allocServo();
    allocCrane();
    allocSkittle();
    allocTesla();
}

void RobotDevicesCL::allocMotor()
{
    if (RobotConfig2005->hasMotor) { 
        if (RobotConfig2005->motorSimu) {
            motor_ = new MotorSimu(RobotConfig->automaticMotorReset);
        } else {
            if (RobotConfig->isMotorISA) {
                motor_ = new MotorIsa(RobotConfig->automaticMotorReset);
            } else {
                if (motorOdom_ != NULL) {
                    motor_ = new Motor05(motorOdom_);
                } else {
                    motor_ = new MotorCL(RobotConfig->automaticMotorReset);
                }
            }
        }
    }
}

void RobotDevicesCL::allocOdometer()
{
    if (RobotConfig2005->hasOdometer) {  
        if (RobotConfig2005->odometerSimu) {
            odometer_ = new OdometerSimu();
        } else {
            if (motorOdom_ != NULL) {
                odometer_ = new Odometer05(motorOdom_);
            }
            else if (IoManager->getIoDevice(IO_ID_ODOMETER_04)) {
                odometer_ = new Odometer_04();
            } else {
                odometer_ = new OdometerCL();
            }
        }
    }
}
void RobotDevicesCL::allocLcd()
{
    if (RobotConfig2005->hasLcd) {  
        if (RobotConfig2005->lcdSimu) {
            lcd_   = new LcdSimu();
        } else {
            if (IoManager->getIoDevice(IO_ID_LCD_05)) {
                lcd_ = new Lcd_05();
            } else {
                lcd_ = new LcdCL();
            }
        }
    }

}

void RobotDevicesCL::allocSound()
{
    if (RobotConfig2005->hasSound) {
        if (RobotConfig2005->soundSimu) {
            sound_   = new SoundSimu();
        } else {
            if (IoManager->getIoDevice(IO_ID_SOUND_05)) {
                sound_ = new Sound05CL();
            } else {
                sound_ = new SoundCL();
            }
        }
    }
}

void RobotDevicesCL::allocBumper()
{
    if (RobotConfig2005->hasBumper) {
        if (RobotConfig2005->bumperSimu) {
            bumper_ = new BumperSimu();
            startWatching(bumper_);
        } else {
            if (IoManager->getIoDevice(IO_ID_BUMPER_05)) {
                bumper_ = new Bumper05();
                startWatching(bumper_);
            } else {
                bumper_ = new BumperCL();
            }
        }
    }
}

void RobotDevicesCL::allocEnv()
{
    if (RobotConfig2005->hasEnv) {
        if (RobotConfig2005->envSimu) {
            env_ = new EnvDetectorSimu();
            startWatching(env_);
        } else {
            if (IoManager->getIoDevice(IO_ID_ENV_05)) {
                env_ = new EnvDetector05();
                startWatching(env_);
            } else {
                env_ = new EnvDetectorCL();
            }
        }
    }
}

void RobotDevicesCL::allocServo()
{
    if (RobotConfig2005->hasServo) {
        if (RobotConfig2005->servoSimu) {
            servo_ = new ServoSimu();
        } else {
            if (IoManager->getIoDevice(IO_ID_SERVO_05)) {
                servo_ = new Servo05();
            } else {
                servo_ = new ServoCL();
            }
        }
    } 
}

void RobotDevicesCL::allocCrane()
{
    if (RobotConfig2005->hasCrane) {
        if (RobotConfig2005->craneSimu) {
            crane_ = new CraneSimu();
        } else {
            if (IoManager->getIoDevice(IO_ID_CRANE_05)) {
                crane_ = new Crane05();
            } else {
                crane_ = new CraneCL();
            }
        }
    }
}

void RobotDevicesCL::allocSkittle()
{
    if (RobotConfig2005->hasSkittleDetector) {
        if (RobotConfig2005->skittleDetectorSimu) {
            skittle_ = new SkittleDetectorSimu();
        } else {
            if (IoManager->getIoDevice(IO_ID_SKITTLE_DETECTOR_05)) {
                skittle_ = new SkittleDetector05();
                startWatching(skittle_);
            } else {
                skittle_ = new SkittleDetectorCL();
            }
        }
    }
}

void RobotDevicesCL::allocTesla()
{
    if (RobotConfig2005->hasTesla) {
        if (RobotConfig2005->teslaSimu) {
            tesla_ = new BigTeslaSimu();
        } else {
            if (IoManager->getIoDevice(IO_ID_TESLA_05)) {
                tesla_ = new BigTesla05();
                startWatching(tesla_);
            } else {
                tesla_ = new BigTeslaCL();
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
