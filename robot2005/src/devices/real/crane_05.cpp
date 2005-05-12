#include "devices/implementation/crane05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/craneCom_05.h"

#ifdef TEST_MAIN
#define LOG_DEBUG_ON
#endif

#include "log.h"


  /** @brief Constructeur */
Crane05::Crane05(): device_(NULL)
{
    device_ = IoManager->getIoDevice(IO_ID_CRANE_05);
    if (device_ != NULL) {
	if (device_->open()) {
	    LOG_OK("Initialization Done\n");
	} else {
	    device_=NULL;
	    LOG_ERROR("Device-open for Crane05 failed.\n");
	}
    } else {
        LOG_ERROR("Crane05 device not found!\n");
    } 
}
Crane05::~Crane05() 
{
    if (device_) device_->close();
}

// envoie la grue a une certaine position
bool Crane05::setPosition(CranePositionX x, 
                          CranePositionZ z) 
{
    if (!device_) return false;
    return false;
    if (x > 0x0F || z > 0x0F) {
	LOG_ERROR("position must be between 0 and 15 (given: %d, %d)\n", x, z);
	return false;
    }
    return device_->write(CRANE_REQ_SET_HORIZONTAL_POSITION + x)
	&& device_->write(CRANE_REQ_SET_VERTICAL_POSITION + z);
}
// arrete les moteurs
bool Crane05::disableMotors() 
{
    if (!device_) return false;
    return device_->write(CRANE_REQ_DISABLE);
}

#ifdef TEST_MAIN

#include "io/serialPort.h"
int main(int argc, char* argv[]) 
{
    IoManager->submitIoHost(new SerialPort(0, SERIAL_SPEED_38400));
#ifndef GUMSTIX
    IoManager->submitIoHost(new SerialPort(1, SERIAL_SPEED_38400));
#endif
    IoManager->submitIoHost(new SerialPort(2, SERIAL_SPEED_38400));
    IoManager->submitIoHost(new SerialPort(3, SERIAL_SPEED_38400));
    int choice, x, z;
    Crane05 crane;
    bool loop=true;
    while(loop) {
      printf("Menu: 0=disable ALL, 1=setPosition, 2=exit\n>");
      scanf("%d", &choice);
      switch(choice) {
      case 0:
	crane.disableMotors();
	break;
      case 1:
        printf("  Entrer la position en x\n  >");
	scanf("%d", &x);
	printf("  Entrer la position en z\n  >");
	scanf("%d", &z);
	crane.setPosition(x, z);
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

