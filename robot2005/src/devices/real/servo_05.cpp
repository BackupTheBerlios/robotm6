#include "devices/implementation/servo05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/servoCom_05.h"

#ifdef TEST_MAIN
#define LOG_DEBUG_ON
#endif

#include "log.h"

Servo05::Servo05() : device_(NULL)
{
    device_ = IoManager->getIoDevice(IO_ID_SERVO_05);
    if (device_ != NULL) {
	if (device_->open()) {
	    LOG_OK("Initialization Done\n");
	} else {
	    device_=NULL;
	    LOG_ERROR("device-open for servo 05 failed.\n");
	}
    } else {
        LOG_ERROR("servo 05 device not found!\n");
    } 
}

Servo05::~Servo05()
{
    if (device_) device_->close();
}
/** @brief envoie le servo a un position donnee et l'asservi */
bool Servo05::setServoPosition(unsigned char servoId, 
			       unsigned char pos)
{
  if (!device_) return false;
  if (servoId > SERVO_ID_MAX) return false;
  IoByte buf[2];
  buf[0] = SERVO_REQ_SET_POS + servoId;
  buf[1] = max(min(pos, SERVO_POS_MAX), SERVO_ID_MIN);
  unsigned int l=2;
  if (device_->write(buf, l)) {
    LOG_DEBUG("setServoPosition(%d, 0x%2.2x)\n", servoId, buf[1]);
    return true;
  } else {
    LOG_ERROR("setServoPosition COM ERROR\n");
    return false;
  }
}
bool Servo05::enableAll() 
{
  return false; 
}

/** @brief desasservi tous les servos */
bool Servo05::disableAll()
{
  if (!device_) return false;
  if (device_->write(SERVO_REQ_DISABLE_ALL)) {
    LOG_DEBUG("disableAll\n");
    return true;
  } else {
    LOG_ERROR("disableAll COM ERROR\n");
    return false;
  }
}

////////////////////////////////////////////////////////////////////////////////////

#ifdef TEST_MAIN

#include "io/serialPort.h"
int main(int argc, char* argv[]) 
{
    IoManager->submitIoHost(new SerialPort(0, false));
    IoManager->submitIoHost(new SerialPort(1, false));
    IoManager->submitIoHost(new SerialPort(2, false));
    IoManager->submitIoHost(new SerialPort(3, false));
    int choice, servoId, pos;
    Servo05 servo;
    bool loop=true;
    while(loop) {
      printf("Menu: 0=disable ALL, 1=setPosition, 2=exit\n>");
      scanf("%d", &choice);
      switch(choice) {
      case 0:
	servo.disableAll();
	break;
      case 1:
        printf("  Entrer le numero du servo [0,3]\n  >");
	scanf("%d", &servoId);
	printf("  Entrer la position [%d,%d]\n  >", 
	       SERVO_POS_MIN, SERVO_POS_MAX);
	scanf("%d", &pos);
	servo.setServoPosition(servoId, pos);
	break;
      case 2:
	loop=false;
	break;
      }
    }
    printf("Bye\n");
    return 0;
}

#endif

