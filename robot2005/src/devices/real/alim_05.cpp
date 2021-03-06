#include "devices/implementation/alim05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/alimCom_05.h"
#include "log.h"

    /** @brief Constructeur */
Alim05::Alim05() : device_(NULL) 
{
    device_ = IoManager->getIoDevice(IO_ID_ALIM_05);
    if (device_ != NULL) {
	if (device_->open()) {
	    LOG_OK("Initialization Done\n");
	} else {
	    device_=NULL;
	    LOG_ERROR("device-open for alim 05 failed.\n");
	}
    } else {
        LOG_ERROR("alim 05 device not found!\n");
    } 
}
Alim05::~Alim05()
{
    if (device_) device_->close();
}

bool Alim05::getAllTension(Millivolt tension[4]) 
{
    if (!device_) return false;
    unsigned int l=5;
    unsigned char comData[5];
    bool status = device_->writeRead(ALIM_REQ_GET_TENSION, comData, l);
    if (!status) {
        // affiche le mesage d'erreur une seule fois
        LOG_ERROR("Alim05::getAllTension read error\n");
        return false;
    }
    unsigned char checkSum=comData[0]^comData[1]^comData[2]^comData[3];
    if (checkSum != comData[4]) {
      LOG_ERROR("getAllTension CheckSumError: read=0x%2.2x, expected=0x%2.2x\n",
		comData[4], checkSum);
      return false;
    }
    for(int i=0; i<4 && i<ALIM_TENSION_NBR; i++) {
        if (comData[i]==0) {
            LOG_ERROR("Tension battery %d: 0 V\n", i);
        } else if (comData[i]==1) {
            LOG_ERROR("Battery %d is not connected\n", i);
        } else {
            tension[i] = 7200 + 50*comData[i]; // TODO use fab conversion method
            if (tension[i]>14) {
	      LOG_OK("Battery %d: %2.2fV (0x%2.2x)\n", i, tension[i]/1000., comData[i]);
	    } else if (tension[i]>13) {
	      LOG_WARNING("Battery %d: %2.2fV (0x%2.2x)\n", i, tension[i]/1000., comData[i]);
	    } else {
	      LOG_ERROR("Battery %d: %2.2fV (Ox%2.2x)\n", i, tension[i]/1000., comData[i]);
	    }
        }
    }
    return true; 
}
/** @brief passe en mode 12V */
bool Alim05::mode12V()
{ 
  unsigned int l=5;
  unsigned char comData[5];
  bool status = device_->writeRead(ALIM_REQ_MODE_12V, comData, l);
  if (!status) {
    // affiche le mesage d'erreur une seule fois
    LOG_ERROR("Alim05::mode12V read error\n");
    return false;
  }
    return false;
}

/** @brief passe en mode 28V */
bool Alim05::mode28V()
{
    bool status = device_->write(ALIM_REQ_MODE_28V);
    if (!status) {
      // affiche le mesage d'erreur une seule fois
      LOG_ERROR("Alim05::mode28V write error\n");
      return false;
    }
    return status;
}
/** @brief passe en mode 42V */
bool Alim05::mode42V()
{ 
    bool status = device_->write(ALIM_REQ_MODE_42V);
    if (!status) {
      // affiche le mesage d'erreur une seule fois
      LOG_ERROR("Alim05::mode42V write error\n");
      return false;
    }
    return status;
}
/** @brief active le 12V 1*/
bool Alim05::enable12V_1()
{
    return false;
}
/** @brief active le 12V 1*/
bool Alim05::disable12V_1()
{
    return false;
}
/** @brief active le 12V 2*/
bool Alim05::enable12V_2()
{
    return false;
}
/** @brief active le 12V 2*/
bool Alim05::disable12V_2()
{
    return false;
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
    
    Alim05 alim;
    Millivolt tension[4];
    bool loop=true;
    int choice=0;
    while(loop) {
      printf("Menu: 0=getTension, 1=12V, 2=28V, 3=exit\n>");
      scanf("%d", &choice);
      switch(choice) {
      case 0:
	alim.getAllTension(tension);
	break;
      case 1:
	alim.mode12V();
	break;
      case 2:
	alim.mode28V();
	break;
      case 3:
	loop=false;
	break;
      default:
	break;
      }
    }
    printf("Bye\n");
    return 0;
}

#endif
