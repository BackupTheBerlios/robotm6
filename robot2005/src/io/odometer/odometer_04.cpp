#include <signal.h>
#include <stdio.h>
#include "log.h"
#include "uart.h"
#include "events.h"
#include "odometer.h"
#include "odometerCom_04.h"
#include "mthread.h"
#ifdef LSM_TODO
#include "robotPosition.h"
#endif
#include "robotTimer.h"

// =========================================================================
// Odometer04Filter
// =========================================================================
// Interface avec la carte odometre 2004
// =========================================================================

bool Odometer04Filter(UartByte byte)
{
    return ((Odometer_04*)ODOMETER)->filter(byte);
}

namespace {

  inline void data2Pos(unsigned char * data, 
		       CoderPosition & left, 
		       CoderPosition & right)
  {
      left = data[1]+256*data[0];
      right  = data[3]+256*data[2];
#if 0
         printf("%d %d - %2x %2x %2x %2x\n", 
	    (short)right, (short)left, data[0], data[1], data[2], data[3]);
#endif
  }
}

// -------------------------------------------------------------------------
// Odometer_04::Odometer_04
// -------------------------------------------------------------------------
Odometer_04::Odometer_04() : 
    Odometer(), uart_(NULL), left_(0), right_(0), mode_(0), 
    bufIndex_(-1), dataAvailable_(false)
{
    UartManager* uartMgr = UartManager::instanceNoCheck();
    if (!uartMgr) {
        uartMgr = new UartManager();
    }
    uart_ = (UartBuffer*)uartMgr->getUartById(UART_ODOMETER_04);
    if (uart_ != NULL) {
        uart_->registerFilterFunction(Odometer04Filter);
        init_ = true;
        LOG_OK("Initialization Done\n");
    } else {
        setMode(ODOMETER_MANUAL);
        init_ = false;
        LOG_ERROR("odometer device not found!\n");
    }
}

// -------------------------------------------------------------------------
// Odometer_04::~Odometer_04
// -------------------------------------------------------------------------
Odometer_04::~Odometer_04()
{
    setMode(ODOMETER_MANUAL);
}
 
// -------------------------------------------------------------------------
// Odometer_04::ping
// -------------------------------------------------------------------------
// Return true if the lcd board is responding
// -------------------------------------------------------------------------
bool Odometer_04::ping()
{
    if (uart_) {
        return uart_->ping();
    } else {
        LOG_ERROR("Odometer device not found and not pinging\n");
        return false;
    }
}

// -------------------------------------------------------------------------
// Odometer_04::reset
// -------------------------------------------------------------------------
// Reset the envDetector uart
// -------------------------------------------------------------------------
bool Odometer_04::reset()
{
    LOG_FUNCTION();
    if (!uart_) {
        uart_ = (UartBuffer*)UARTMGR->getUartById(UART_ODOMETER_04);
        if (uart_ != NULL) {
            init_ = true;
            uart_->registerFilterFunction(Odometer04Filter);
            LOG_OK("Initialization Done\n");
        } else {
            init_ = false;
            LOG_ERROR("Odometer device not found!\n");
        }
        return init_;
    } else {
        setMode(ODOMETER_MANUAL);
        init_ = uart_->reset();
        uart_->registerFilterFunction(Odometer04Filter);
        return init_;
    }
}

// -------------------------------------------------------------------------
// Odometer_04::getCoderPosition
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
    if (!uart_) return false;
    if (mode_ == ODOMETER_AUTOMATIC) {
        left = left_;
        right = right_;
        bool result = dataAvailable_;
        dataAvailable_ = false;
	/*   LOG_DEBUG("left = %d,  right = %d, available=%s\n", 
	     left, right, b2s(result));*/
        return result;
    } else {
        // ODOMETER_MANUAL
        unsigned char data[8];
        bool status = false;
        uart_->write(ODOMETER_GET_POSITION);
        unsigned int l=1;
	status = uart_->read(data, l);
        
        if (!status) {
            LOG_ERROR("Odometer getCoderPosition %s\n", 
                      status?"OK":"ERR");
            return false;
        }
        left=left_; 
	right=right_;
        LOG_DEBUG("left = %d,  right = %d\n", left, right);
        return true;
    }
}

// -------------------------------------------------------------------------
// Odometer_04::setMode
// -------------------------------------------------------------------------
//  Definit le mode de communication avec la carte
//  En mode automatique la carte envoie toute seule des donnees et 
//    getCoderPosition retourne true si la carte a mise a jour la 
//    position des codeurs depuis la derniere requete getCoderPosition
//  En mode manuel getCoderPosition va directement lire la valeur 
//    des codeurs et retourne true si la communication s'est bien passee.
//  By default it is manual
// Retourne FALSE en cas d'erreur de communication avec la carte
// -------------------------------------------------------------------------
bool Odometer_04::setMode(bool automatic)
{
    if (!uart_) return false;
    bool result = false;
    unsigned char data[2];
    if (automatic == ODOMETER_AUTOMATIC) {
        result = uart_->write(ODOMETER_SET_MODE_AUTOMATIC);
	result |= uart_->read(data);
   
        LOG_DEBUG("ODOMETER_SET_MODE_AUTOMATIC\n");
        if (!result || data[0] != ODOMETER_MODE_AUTOMATIC) {
	  LOG_ERROR("Odometer %s invalide set mode %d\n", 
		    result?"OK":"ERR", data[0]);
	} else {
	  mode_=1;
	}
    } else {
        result = uart_->write(ODOMETER_SET_MODE_MANUAL);
	result |= uart_->read(data);
   
        LOG_DEBUG("ODOMETER_SET_MODE_MANUAL\n");
        if (!result || data[0] != ODOMETER_MODE_MANUAL) {
	  LOG_ERROR("Odometer %s invalide set mode %d\n", 
		    result?"OK":"ERR", data[0]);
	} else {
	  mode_=0;
	}
    }
    return result;
}

// -------------------------------------------------------------------------
// Odometer_04::getMode
// -------------------------------------------------------------------------
// Retourne le mode dans lequel se trouve la carte
// -------------------------------------------------------------------------
bool Odometer_04::getMode(bool& automatic)
{
    if (!uart_) return false;
    unsigned char data[2];
    bool status = false;
    uart_->write(ODOMETER_GET_MODE);
    status = uart_->read(data);
    
    if (!status || (data[0] != ODOMETER_MODE_AUTOMATIC 
                    && data[0] != ODOMETER_MODE_MANUAL) ) {
        LOG_ERROR("Odometer %s invalide get mode %d\n", 
                  status?"OK":"ERR", data[0]);
        return false;
    }
    automatic = (data[0] == ODOMETER_MODE_AUTOMATIC) ? 
        ODOMETER_AUTOMATIC : ODOMETER_MANUAL;
    LOG_DEBUG("Mode: %s\n", automatic ? "AUTOMATIC":"MANUAL");
    return true;
}

//static int Odometer_04_periodCounter=0;
//static Millisecond Odometer_04_periodZeroTime=0;

// -------------------------------------------------------------------------
// Odometer_04::filter
// -------------------------------------------------------------------------
// Filtre les donnees qui arrivent en mode automatique
// -------------------------------------------------------------------------
bool Odometer_04::filter(UartByte byte)
{
  //printf("Odometer filter %2x\n", byte);
    static bool isRightData=true;
    static bool rightOK=true;
    switch((byte & 0xC0)) {
    case ODOMETER_BEGIN_AUTO_SEND:
        if ((byte & 0xE0) == ODOMETER_BEGIN_AUTO_SEND) {
            bufferPosition_[4] = ((byte & 0x1F)<<3);
            bufIndex_+=0x01;
        } else {
            if (bufIndex_ != 0x11) {
		LOG_ERROR("Bad data 0x%02x != 0x11, right=%s\n", 
			  bufIndex_, b2s(isRightData));
		if (isRightData) rightOK=false;
	    } else if (!isRightData && !rightOK) {
	    // on recoit la donnee de gauche et correcte mais la valeur de 
	    // droite etait incorrecte. On ne fait rien
            } else {
		// les 2 valeurs sont correctes
                bufferPosition_[5] += (byte & 0x1F);
                if (isRightData) {
                    rightOK=true;
                    bufferPosition_[2]=bufferPosition_[4];
                    bufferPosition_[3]=bufferPosition_[5];
                } else {
                    bufferPosition_[0]=bufferPosition_[4];
                    bufferPosition_[1]=bufferPosition_[5];
                    data2Pos(bufferPosition_, left_, right_);
                    dataAvailable_=true;
                    rightOK=false;
#ifdef LSM_TODO
		    if (mode_ == ODOMETER_AUTOMATIC)
			ROBOT_POS->updateOdometerPosition();
#endif
                }
            }
            bufIndex_=0x00;
        }
        
        break;
    case ODOMETER_AUTO_SEND_LEFT:
        isRightData=false;
        bufferPosition_[4] += ((byte & 0x38)>>3);
        bufferPosition_[5] = ((byte & 0x07)<<5);
        bufIndex_+=0x10;
        break;
    case ODOMETER_AUTO_SEND_RIGHT:
        isRightData=true;
        bufferPosition_[4] += ((byte & 0x38)>>3);
        bufferPosition_[5] = ((byte & 0x07)<<5);
        bufIndex_+=0x10;
        break; 
    case 0x80:
    default:
        return false;
    }
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
    if (odometer_) {
        odometer_->setMode(false);
    }
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


