#include "devices/implementation/tesla05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/teslaCom_05.h"
#include "events.h"

#ifdef TEST_MAIN
#define LOG_DEBUG_ON
#endif

#include "log.h"


/** @brief Constructeur */
BigTesla05::BigTesla05(): device_(NULL), detectorEnabled_(false)
{
    device_ = IoManager->getIoDevice(IO_ID_TESLA_05);
    if (device_ != NULL) {
	if (device_->open()) {
	    LOG_OK("Initialization Done\n");
	} else {
	    device_=NULL;
	    LOG_ERROR("Device-open for Tesla05 failed.\n");
	}
    } else {
        LOG_ERROR("Tesla05 device not found!\n");
    } 
}

BigTesla05::~BigTesla05()
{
    if (device_) device_->close();
}

  
/** verification que le detecteur d'accrochage marche,
    a faire avait le debut du match */
bool BigTesla05::testDetector(bool& result)
{
    if (!device_) return false;
    unsigned char buf[2];
    if (device_->writeRead(TESLA_REQ_AUTOTEST, buf)) {
        result = (buf[0] == TESLA_OK);
        return true;
    } else {
        return false;
    }
}
/** ne demarrer le capteur qu'apres avoir demarre l'aimant */
bool BigTesla05::enableDetector()
{
    if (!device_) return false;
    unsigned char buf[2];
    if (device_->writeRead(TESLA_REQ_ENABLE_DETECTOR, buf)) {
        detectorEnabled_ = (buf[0] == TESLA_OK);
        return detectorEnabled_;
    } else {
        return false;
    } 
}
/** arreter le detecteur avant d'arreter l'aimant */
bool BigTesla05::disableDetector()
{
    detectorEnabled_ = true;
    if (!device_) return false;
    unsigned char buf[2];
    if (device_->writeRead(TESLA_REQ_DISABLE_DETECTOR, buf)) {
        return (buf[0] == TESLA_OK);
    } else {
        return false;
    } 
}
/** arrete l'electro aimant */
bool BigTesla05::stopTesla() 
{
    if (!device_) return false;
    unsigned char buf[2];
    if (device_->writeRead(TESLA_REQ_STOP_ELECTRO, buf)) {
        return (buf[0] == TESLA_OK);
    } else {
        return false;
    } 
}
/** passer la carte alime dans le mode correspondant avant de 
    demarrer l'electroaimant! */
bool BigTesla05::startTesla(TeslaMode mode)
{
    if (!device_) return false;
    unsigned char buf[2];
    if (device_->writeRead((unsigned char)(TESLA_REQ_START_ELECTRO+mode), 
                           buf)) {
        return (buf[0] == TESLA_OK);
    } else {
        return false;
    } 
}
/** tache peridoci qui verifie si on a accroche */
void BigTesla05::periodicTask()
{
    if (!device_) return;
    if (detectSkittleAttached()) {
        Events->raise(EVENTS_SKITTLE_ATTACHED);
    }
}

bool BigTesla05::detectSkittleAttached() 
{
    if (!device_ || !detectorEnabled_) return false;
    unsigned char buf[2];
    if (device_->writeRead(TESLA_REQ_DETECT_VALUE, buf)) {
        return (buf[0] == TESLA_OK);
    } else {
        return false;
    } 
}

#ifdef TEST_MAIN

#include "io/serialPort.h"
int main(int argc, char* argv[]) 
{
    IoManager->submitIoHost(new SerialPort(0, false));
    IoManager->submitIoHost(new SerialPort(1, false));
    IoManager->submitIoHost(new SerialPort(2, false));
    IoManager->submitIoHost(new SerialPort(3, false));  
    EventsManagerCL* evtMgr = new EVENTS_MANAGER_DEFAULT();

    int choice;
    BigTesla05 tesla;
    bool loop=true;
    while(loop) {
      printf("Menu: 0=disable ALL, 1=setPosition, 2=exit\n>");
      scanf("%d", &choice);
      switch(choice) {
      case 0:
	break;
      case 1:
        break;
      case 2:
	loop=false;
	break;
      }
    }
  
    printf("Bye\n");
    delete evtMgr;
    return 0;
}

#endif
