#include "robotDevices.h"
#include "robotConfig2005.h"
#include "implementation/motorSimu.h"
#include "implementation/motorIsa.h"
#include "implementation/soundSimu.h"
#include "implementation/lcdSimu.h"
#include "implementation/odometerSimu.h"
#include "implementation/bumperSimu.h"
#include "implementation/envSimu.h"
#include "implementation/servoSimu.h"
#include "devices/implementation/odometer04.h"
#include "devices/implementation/lcd03.h"
#include "devices/implementation/lcd05.h"
#include "implementation/bumper05.h"
#include "implementation/env05.h"
#include "implementation/servo05.h"
#include "implementation/sound05.h"
#include "io/ioManager.h"
#include "io/serialPort.h"
#include "log.h"

namespace {
    // TODO: copied the name from ioManager. what's the meaning of the "4"? [flo]
    pthread_mutex_t repositoryLock4 = PTHREAD_MUTEX_INITIALIZER;
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
    motor_(NULL), odometer_(NULL), lcd_(NULL), bumper_(NULL), sound_(NULL),
    env_(NULL), servo_(NULL)
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
    if (motor_)    { delete motor_;    motor_=NULL; }
    if (odometer_) { delete odometer_; odometer_=NULL; }
    if (lcd_)      { delete lcd_;      lcd_=NULL; }
    if (bumper_)   { delete bumper_;   bumper_=NULL; }
    if (sound_)    { delete sound_;    sound_=NULL; }
    if (env_)      { delete env_;      env_=NULL; }
    if (servo_)    { delete servo_;    servo_=NULL; }
}

/** @brief detecte et cree toutes les cartes */
void RobotDevicesCL::allocDevices()
{
    LOG_FUNCTION();
    //TODO split alloc function
//    IoManager->submitIoHost(new SerialPort(0, false));
    // TODO: keep reference for later deletion of serialPort
    IoManager->submitIoHost(new SerialPort(1, false));
    
    if (RobotConfig2005->motorSimu) {
        motor_ = new MotorSimu(RobotConfig->automaticMotorReset);
    } else {
        if (RobotConfig->isMotorISA) {
            motor_ = new MotorIsa(RobotConfig->automaticMotorReset);
        } else {
	  if (IoManager->getIoDevice(IO_ID_MOTOR_ODOM_05)) {
	    // TODO
	    motor_ = new MotorCL(RobotConfig->automaticMotorReset);
	  } else {
            motor_ = new MotorCL(RobotConfig->automaticMotorReset);
	  }
        }
    }
    if (RobotConfig2005->lcdSimu) {
        lcd_   = new LcdSimu();
    } else {
      if (IoManager->getIoDevice(IO_ID_LCD_05)) {
	lcd_ = new Lcd_05();
      } else if (IoManager->getIoDevice(IO_ID_LCD_03)) {
	lcd_ = new Lcd_03();
      } else {
	lcd_ = new LcdCL();
      }
    }

    if (RobotConfig2005->soundSimu) {
        sound_   = new SoundSimu();
    } else {
      if (IoManager->getIoDevice(IO_ID_SOUND_05)) {
	sound_ = new Sound05CL();
      } else {
	sound_ = new SoundCL();
      }
    }

    if (RobotConfig2005->odometerSimu) {
	odometer_ = new OdometerSimu();
    } else {
      if (IoManager->getIoDevice(IO_ID_ODOMETER_04)) {
	odometer_ = new Odometer_04();
      } else {
    	odometer_ = new OdometerCL();
      }
    }
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

/** @brief Envoie le signal d'arret d'urgence a toutes les cartes */
void RobotDevicesCL::emergencyStop()
{
    if (motor_)    motor_->emergencyStop();
    if (odometer_) odometer_->emergencyStop();
    if (lcd_)      lcd_->emergencyStop();
    if (bumper_)   bumper_->emergencyStop();
    if (sound_)    sound_->emergencyStop();
    if (env_)      env_->emergencyStop();
    if (servo_)    servo_->emergencyStop();
}

/** @brief Envoie le signal de reset a toutes les cartes */
void RobotDevicesCL::reset()
{
    if (motor_)    motor_->reset();
    if (odometer_) odometer_->reset();
    if (lcd_)      lcd_->reset();
    if (bumper_)   bumper_->reset();
    if (sound_)    sound_->reset();
    if (env_)      env_->reset();
    if (servo_)    servo_->reset();
}

void RobotDevicesCL::periodicTask()
{
    Lock localLock(&repositoryLock4);
    RobotDevicesSet::iterator it;
    for (it = watchedDevices_.begin();
	 it != watchedDevices_.end();
	 ++it) {
	(*it)->periodicTask();
    }
}

void RobotDevicesCL::startWatching(RobotDeviceCL* device)
{
  Lock localLock(&repositoryLock4);
  watchedDevices_.insert(device);
}

void RobotDevicesCL::stopWatching(RobotDeviceCL* device)
{
  Lock localLock(&repositoryLock4);
  RobotDevicesSet::iterator it = watchedDevices_.find(device);
  if (it != watchedDevices_.end()) {
    watchedDevices_.erase(it);
  }
}
