#include "uart.h"
#include "log.h"
#include "robotTimer.h"
#include "ioManager.h"
#include "robotConfig.h"

#include "lcd.h"
#include "odometer.h"
// ============================================================================
// =============================  class UartManager   =========================
// ============================================================================

/** 
 * @class UartManager
 * Gestion de la carte Uart: autodetection des cartes presentes et
 * auto-assignation des cartes sur le port surlequel elles sont connectees
 */
UartManager* UartManager::uartManager_=NULL;

// -------------------------------------------------------------------------
// UartManager::checkMSerieIsAlive
// -------------------------------------------------------------------------
bool UartManager::checkMSerieIsAlive()
{
    for(int i=0;i<UART_PORT_NBR; ++i) {
        if (uartListByPort_[i] != NULL) {
	    if (uartListByPort_[i]->checkMSerieIsAlive()) {
	       return true;
	    }
        }
    }
    return false;
}

// -------------------------------------------------------------------------
// UartManager::UartManager
// -------------------------------------------------------------------------
UartManager::UartManager(bool dontScan) : 
  RobotComponent("uartManager", CLASS_UART_MANAGER), 
  lcd_(NULL), odometer_(NULL)
{
    // verifie que les conposants de base existent
    RobotTimerCL::instance(); // construit le robotimer si necessaire
    IoManagerCL* ioManager = IoManagerCL::instanceNoCheck();
    if (!ioManager) {
        // desactive l'allocation dynamique de toutes les classes de ioManager
        // car on est en mode test unitaire
        RobotConfig->ioManagerAlloc = false;
        ioManager = new IoManagerCL();
    }
    // allocation des 8 uarts en fonction de leur type
    assert(uartManager_==NULL);
    uartManager_=this;
    int i;
    for(i=0;i<UART_NBR; ++i) {
        switch(uartInfos_[i].type) {
	case UART_BASIC:
	  uartListById_[i] = new Uart();
	  break;
	case UART_FILTER:
	  uartListById_[i] = new UartBuffer();
	  break;
	}
    }
    for(i=0;i<UART_PORT_NBR; ++i) {
        uartListByPort_[i] = NULL;
    }

    // scan les uarts pour trouver ou sont connectees les cartes
    if (!dontScan) {
        scanAndAlloc();
        init_=true;
    } else {
        init_=false;
    }
    LOG_OK("Initialisation Done\n");
}

// -------------------------------------------------------------------------
// UartManager::~uartManager
// -------------------------------------------------------------------------
UartManager::~UartManager()
{
    LOG_WARNING("Destructing UartManager\n");
    if (lcd_)         { delete lcd_;         lcd_        = NULL; }
    if (odometer_)    { delete odometer_;    odometer_   = NULL; }
    int i;
    for(i=0;i<UART_NBR; ++i) {
        delete uartListById_[i];
    }
    close();
    uartManager_=NULL;
}

// -------------------------------------------------------------------------
// UartManager::isOpened
// -------------------------------------------------------------------------
bool UartManager::reset()
{
    LOG_FUNCTION();
    init_ = true;
    if (lcd_)         { init_ &= lcd_->reset();         }
    if (odometer_)    { init_ &= odometer_->reset();    }
    return init_;;
}

// -------------------------------------------------------------------------
// UartManager::emergencyStop
// -------------------------------------------------------------------------
void UartManager::emergencyStop()
{
    if (odometer_)  { odometer_->setMode(ODOMETER_MANUAL); } 
}

// -------------------------------------------------------------------------
// UartManager::validate
// -------------------------------------------------------------------------
bool UartManager::validate()
{
    UartInfo infos[UART_PORT_NBR];
    scan(infos);
    return true;
}

// -------------------------------------------------------------------------
// UartManager::scanAndAlloc
// -------------------------------------------------------------------------
void UartManager::scanAndAlloc()
{
    searchAndOpen();
    allocDevices();
    listConnected();
}

// -------------------------------------------------------------------------
// UartManager::listConnected
// -------------------------------------------------------------------------
void UartManager::listConnected()
{
    bool atLeastOneConnected=false;
    for(int i=0; i<UART_PORT_NBR; i++) {
        if (uartListByPort_[i] != NULL){
            LOG_INFO("%s is connected on port %d\n",
                     uartListByPort_[i]->name(), i);
            atLeastOneConnected=true;
        }
    }
    if (!atLeastOneConnected) {
        LOG_INFO("No UART device dectected\n");
    }
}


// -------------------------------------------------------------------------
// UartManager::scan
// -------------------------------------------------------------------------
// Regarde ce qui est connecte sur les ports non ouverts
// -------------------------------------------------------------------------
void UartManager::scan(UartInfo infos[UART_PORT_NBR]) 
{
    for(int i=0; i<UART_PORT_NBR; i++) {
        infos[i].id = UART_NONE;
        if (uartListByPort_[i] != NULL){
            continue;
	}
        Uart uart;
	if (uart.open((UartPort)i)) {
            uart.ping();
            infos[i]=uart.getInfo();
            if (infos[i].id != UART_NONE) {
                LOG_DEBUG("Port %d: %s is connected\n", i, infos[i].name);
            } else {
                LOG_DEBUG("Port %d: no device\n", i);
            }
            uart.close();
	} else {
            LOG_DEBUG("Port %d: cannot be open... It is probably used by another application\n", i);
	}
    }
}

// -------------------------------------------------------------------------
// UartManager::searchAndOpen
// -------------------------------------------------------------------------
// Regarde les ports non ouverts et si une carte est connectee, le port 
// reste ouvert
// -------------------------------------------------------------------------
int  UartManager::searchAndOpen()
{
     UartInfo infos[UART_PORT_NBR];

     scan(infos);
    
    int found = 0;
    for(int i=0; i<UART_PORT_NBR; i++) {
        if (infos[i].id != UART_NONE) {
	    uartListByPort_[i] = uartListById_[infos[i].id];
	    uartListById_[infos[i].id]->open((UartPort)i, infos[i]);
        } 
    }
    return found;
}

// -------------------------------------------------------------------------
// UartManager::allocLcd
// -------------------------------------------------------------------------
// start the lcd board
// -------------------------------------------------------------------------
void UartManager::allocLcd()
{
#ifndef UART_SCANNER_MAIN
    LOG_FUNCTION();
    if (!lcd_ || lcd_->isSimu()) {
        if (lcd_) { delete lcd_; lcd_ = NULL; }
        if (RobotConfig->lcdSimu) {
            lcd_   = new LcdSimu();
        } else {
            if (getUartById(UART_LCD_04) != NULL) {;
            lcd_   = new Lcd_04();
            } else if (getUartById(UART_LCD_03) != NULL) {
                lcd_   = new Lcd_03();
            } else {
                lcd_   = new LcdSimu();
            }
        }
    } 
#endif
}



// -------------------------------------------------------------------------
// UartManager::allocOdometer
// -------------------------------------------------------------------------
// start the odometer board
// -------------------------------------------------------------------------
void UartManager::allocOdometer()
{
#ifndef UART_SCANNER_MAIN
    LOG_FUNCTION();
    if (!odometer_ || odometer_->isSimu()) {
        if (odometer_) {delete odometer_; odometer_ = NULL; }
        if (RobotConfig->odometerSimu) {
            odometer_   = new OdometerSimu();
        } else {
            if (getUartById(UART_ODOMETER_04) != NULL) {
                odometer_   = new Odometer_04();
            } else { 
                odometer_   = new OdometerSimu();
            }
        }
    }
#endif
}


// -------------------------------------------------------------------------
// UartManager::allocDevices
// -------------------------------------------------------------------------
// Alloue l'instance du controleur de carte des cartes qui ont ete trouvees
// -------------------------------------------------------------------------
void UartManager::allocDevices() 
{
    if (RobotConfig->ioManagerAlloc) {
        
        LOG_FUNCTION();
        allocLcd();
        allocOdometer();
    }
}

// -------------------------------------------------------------------------
// UartManager::close
// -------------------------------------------------------------------------
void UartManager::close()
{
    UartPortMask closeMask = RobotConfig->uartPortMask;
    for(int i=0; i<UART_PORT_NBR; ++i) {
        if (closeMask & (1<<i)) {
	    if (uartListByPort_[i] && uartListByPort_[i]->isOpened()) {
                uartListByPort_[i]->close();
	    }
	    uartListByPort_[i] = NULL;
        }
    }
}

// -------------------------------------------------------------------------
// UartManager::getUartById
// -------------------------------------------------------------------------
Uart* UartManager::getUartById(UartId id)
{
    return uartListById_[(int)id]->isOpened() ? uartListById_[(int)id] : NULL;
}

// -------------------------------------------------------------------------
// UartManager::getUartByPort
// -------------------------------------------------------------------------
Uart* UartManager::getUartByPort(UartPort port)
{
    return uartListByPort_[(int)port];
}


#ifdef UART_SCANNER_MAIN
// Programme de test qui afiche la liste des cartes connectees aux uarts
int main(int argc, char* argv[]) 
{
    RobotConfig->uartPortMask=0xFF;
    ClassConfig::find(CLASS_MTHREAD)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_UART)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_ROBOT_TIMER)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_UART_MANAGER)->setVerboseLevel(VERBOSE_DEBUG);
    UartManager mgr(true);
    UartInfo infos[UART_PORT_NBR];
    
    mgr.scan(infos);
    return 0;
}

#endif // UART_SCANNER_MAIN
