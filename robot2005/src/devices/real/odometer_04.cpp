#include <signal.h>
#include <stdio.h>
#define LOG_DEBUG_ON
#include "log.h"
//#include "uart.h"
#include "events.h"
#include "implementation/odometer04.h"
#include "odometerCom_04.h"
#include "mthread.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#ifdef LSM_TODO
#include "robotPosition.h"
#endif
#include "robotTimer.h"

// =========================================================================
// Odometer04Filter
// =========================================================================
// Interface avec la carte odometre 2004
// =========================================================================

namespace {

  inline void data2Pos(unsigned char* data, 
		       CoderPosition& left, 
		       CoderPosition& right)
  {
      left = data[1]+256*data[0];
      right  = data[3]+256*data[2];
#if 1
      LOG_DEBUG("%d %d - %2x %2x %2x %2x\n", 
	    (short)right, (short)left, data[0], data[1], data[2], data[3]);
#endif
  }
}

Odometer_04::Odometer_04() :
    OdometerCL(), device_(NULL), left_(0), right_(0), mode_(0), 
    bufIndex_(-1), dataAvailable_(false)
{
    device_ = IoManager->getIoDevice(IO_ID_ODOMETER_04);
    if (device_ != NULL) {
	if (device_->open()) {
	    LOG_OK("Initialization Done\n");
	} else {
	    device_=NULL;
	    LOG_ERROR("device-open for odometer failed.\n");
	}
    } else {
        LOG_ERROR("odometer device not found!\n");
    }
}

// -------------------------------------------------------------------------
// Odometer_04::~Odometer_04
// -------------------------------------------------------------------------
Odometer_04::~Odometer_04()
{
    if (device_ != NULL) {
	device_->close();
    }
}

/*
bool Odometer_05::ping()
{
    if (device_) {
	// TODO: where's the ping? should be abstracted. but where? [flo]
        //return uart_->ping();
	return true;
    } else {
        LOG_ERROR("Odometer device not found and not pinging\n");
        return false;
    }
}
*/

bool Odometer_04::reset()
{
    bool init=false;
    LOG_FUNCTION();
    if (!device_) {
        device_ = IoManager->getIoDevice(IO_ID_ODOMETER_04);
        if (device_ != NULL) {
	    init = true;
            LOG_OK("Initialization Done\n");
        } else {
	    init = false;
            LOG_ERROR("Odometer device not found!\n");
        }
    } else {
	init = device_->reset();
    }
    return init;
}

// -------------------------------------------------------------------------
// Met a jour la position des 2 codeurs (sans conversion) 
// En mode automatique la carte envoie toute seule des donnees et 
//   getCoderPosition retourne true si la carte a mise a jour la 
//   position des codeurs depuis la derniere requete getCoderPosition
// En mode manuel getCoderPosition va directement lire la valeur 
//   des codeurs et retourne true si la communication s'est bien passee.
// -------------------------------------------------------------------------
bool Odometer_04::getCoderPosition(CoderPosition &left,
                                   CoderPosition &right)
{
    if (!device_) return false;

    // ODOMETER_MANUAL (in 2005 only manual mode is available...)
    unsigned char data[8];
    bool status = false;

    status = device_->write(ODOMETER_GET_POSITION);
    if (status) {
	unsigned int l=6;
	usleep(20000);
	status = device_->read(data);
	if (status && data[0] == ODOMETER_GET_POSITION) {
	    status = device_->read(data, l);
	} else {
	    // TODO: hardcoded magic number
	    l = 8;
	    device_->read(data, l);
	    LOG_WARNING("Odometer desynchronization\n");
	    status = false;
	}
    }

    // printf("received data: 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x\n", data[0], data[1], data[2], data[3], data[4], data[5]);
    
    if (!status) {
	LOG_ERROR("Odometer getCoderPosition %s\n", 
		  status?"OK":"ERR");
	return true;
    }

    unsigned char bufferPosition[4];
    bufferPosition[0] = (data[0] & 0x1F) << 3;
    bufferPosition[0] += (data[1] & 0x38) >> 3;
    bufferPosition[1] = (data[1] & 0x07) << 5;
    bufferPosition[1] += (data[2] & 0x1F);

    bufferPosition[2] = (data[3] & 0x1F) << 3;
    bufferPosition[2] += (data[4] & 0x38) >> 3;
    bufferPosition[3] = (data[4] & 0x07) << 5;
    bufferPosition[3] += (data[5] & 0x1F);

    data2Pos(bufferPosition, left, right);
    
    LOG_DEBUG("left = %d,  right = %d\n", left, right);
    return true;
}

// ===========================================================================
// MAIN de test
// ===========================================================================

#ifdef MAKE_MAIN

#ifdef LSM_TODO
#include "movementManager.h"
#endif

static Odometer_04* odometer_=NULL;

// ---------------------------------------------------------------------------
// odometerSIGINT
// ---------------------------------------------------------------------------
extern "C" void odometerSIGINT(int sig)
{
    MTHREAD_KILL_ALL();
    if (odometer_) {
        delete odometer_;
    }
    exit(1);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char*argv[]) 
{
    printf("+-----------------------------------------------------+\n");
    printf("| Odometer_04 -- testeur --                           |\n");
    printf("+-----------------------------------------------------+\n");
    ClassConfig::find(CLASS_MTHREAD)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_ROBOT_TIMER)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_UART)->setVerboseLevel(VERBOSE_DEBUG);
    ClassConfig::find(CLASS_UART_MANAGER)->setVerboseLevel(VERBOSE_DEBUG);
    ClassConfig::find(CLASS_IO_MANAGER)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_ODOMETER)->setVerboseLevel(VERBOSE_DEBUG);
#ifdef LSM_TODO
    Log             log;
    RobotTimer      timer;
    EVENTS_MANAGER_DEFAULT   evt;
    Sound           snd;
    MovementManager mvt;
    odometer_ = new Odometer_04;
//   ROBOT_POS->setOdometerType(ODOMETER_UART_AUTOMATIC);
    timer.startMatch();
    // enregistre la detection du Ctrl+C et du callback correspondant qui 
    // stoppe le programme immediatement
    (void) signal(SIGINT, odometerSIGINT);
    while(1) {
      int order;
      printf("Menu: 0=exit, 1=setModeManual, 2=setModeAuto, 3=getMode, "
	     "4=getPos, 5=resetPos, 6=getPos, 7=reset, 9=ping\n>");
      if (scanf("%d", &order)!=1) {
          printf("Vous avez entre une valeur incorrecte.\n");
          goto Odometer04End;
          return 0;
      }
      switch(order) {
      case 1:
          ROBOT_POS->setOdometerType(ODOMETER_UART_MANUAL);
          break;
      case 2:
          ROBOT_POS->setOdometerType(ODOMETER_UART_AUTOMATIC);
          break;
      case 3: 
	{
            bool mode=false;
            odometer_->getMode(mode);
	}
	break;
      case 4: 
	{
            CoderPosition left=0, right=0;
            odometer_->getCoderPosition(left, right);
	}
	break;
      case 7:
          UARTMGR->scanAndAlloc();
          odometer_->reset();
          break; 
      case 5:
          ROBOT_POS->set(0,0,0);
          break; 
      case 6:
          ROBOT_POS->print();
          break;
      case 9:
          printf("Ping=%s\n", odometer_->ping()?"OK":"ERROR");
          break;
      case 0:
          goto Odometer04End;
          return 0;
      default:
          printf("Requete inconnue %d\n", order);
          break;
      } 
    }
 Odometer04End:
    printf("Bye\n");
    odometer_->setMode(false);
    MTHREAD_KILL_ALL();
    delete odometer_;
#endif
    return 0;
}

#endif
