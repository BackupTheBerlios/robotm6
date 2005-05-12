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

void BigTesla05::emergencyStop() 
{ 
    disableDetector();  
    stopTesla(); 
}

/** verification que le detecteur d'accrochage marche,
    a faire avait le debut du match */
bool BigTesla05::testDetector(bool& result)
{
    if (!device_) return false;
    // for now just return true;
    return true;
    /*
    unsigned char buf[2];
    if (device_->writeRead(TESLA_REQ_AUTOTEST, buf)) {
        result = (buf[0] == TESLA_OK);
        return true;
    } else {
        return false;
    }
    */
}
/** ne demarrer le capteur qu'apres avoir demarre l'aimant */
bool BigTesla05::enableDetector()
{
    if (!device_) return false;
    /*
    unsigned char buf[2];
    if (device_->writeRead(TESLA_REQ_ENABLE_DETECTOR, buf)) {
        detectorEnabled_ = (buf[0] == TESLA_OK);
        return detectorEnabled_;
    } else {
        return false;
    }
    */
    return false;
}
/** arreter le detecteur avant d'arreter l'aimant */
bool BigTesla05::disableDetector()
{
    detectorEnabled_ = true;
    if (!device_) return false;
    /*
    unsigned char buf[2];
    if (device_->writeRead(TESLA_REQ_DISABLE_DETECTOR, buf)) {
        return (buf[0] == TESLA_OK);
    } else {
        return false;
    }
    */
    return false;
}
/** arrete l'electro aimant */
bool BigTesla05::stopTesla() 
{
    if (!device_) return false;
/*
    unsigned char buf[2];
    if (device_->writeRead(TESLA_REQ_STOP_ELECTRO, buf)) {
        return (buf[0] == TESLA_OK);
    } else {
        return false;
    }
*/
    return device_->write(TESLA_REQ_STOP_ELECTRO);
}

/** passer la carte alime dans le mode correspondant avant de 
    demarrer l'electroaimant! */
bool BigTesla05::startTesla(TeslaMode mode)
{
    if (!device_) return false;
    /*
    unsigned char buf[2];
    if (device_->writeRead((unsigned char)(TESLA_REQ_START_ELECTRO+mode), 
                           buf)) {
        return (buf[0] == TESLA_OK);
    } else {
        return false;
    }
    */
    return device_->write(TESLA_REQ_START_ELECTRO + mode);
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
    /*
    unsigned char buf[2];
    if (device_->writeRead(TESLA_REQ_DETECT_VALUE, buf)) {
        return (buf[0] == TESLA_OK);
    } else {
        return false;
    }
    */
    // assume there's a skittle there...
    return true;
}

#ifdef TEST_MAIN
#include "mthread.h"
#include <signal.h>
BigTesla05* tesla_=NULL;

void* TeslaThreadBody(void*)
{
    while(1)
    {
	// TODO: remove magic number and replace with constant [flo]
	usleep(100000);
	if (tesla_) tesla_->periodicTask();
    }
    return NULL;
}
void teslaSIGINT(int sig) {
  if (tesla_) tesla_->reset();
  usleep(10000);
  exit(-1);
}

#include "io/serialPort.h"
int main(int argc, char* argv[]) 
{ 
    (void) signal(SIGINT, teslaSIGINT);

    IoManager->submitIoHost(new SerialPort(0, SERIAL_SPEED_38400));
#ifndef GUMSTIX
    IoManager->submitIoHost(new SerialPort(1, SERIAL_SPEED_38400));
#endif
    IoManager->submitIoHost(new SerialPort(2, SERIAL_SPEED_38400));
    IoManager->submitIoHost(new SerialPort(3, SERIAL_SPEED_38400));
    EventsManagerCL* evtMgr = new EVENTS_MANAGER_DEFAULT();
    MThreadId thread = 0;
    MTHREAD_CREATE("Tesla Thread",
		   &thread,
		   NULL,
		   TeslaThreadBody,
		   NULL);
    int choice;
    tesla_ = new BigTesla05(); 
    bool loop=true;
    while(loop) {
      printf("Menu: 0=stop, 1=start electro, 2=enable detector, 3=disable detector 4=exit\n>");
      scanf("%d", &choice);
      switch(choice) {
      case 0:
          tesla_->stopTesla(); 
          break;
      case 1: 
          {
              printf("Tesla mode:0=5V, 1=12V, 2=28V, 3=42V\n");
              scanf("%d", &choice);
              tesla_->startTesla((TeslaMode)choice);
          }
          break;
      case 2: 
          tesla_->enableDetector(); 
          break;
      case 3: 
          tesla_->disableDetector(); 
          break;
      case 4:
          tesla_->stopTesla(); 
          loop=false;
	break;
      }
    }
  
    printf("Bye\n");
    delete evtMgr;
    return 0;
}

#endif
