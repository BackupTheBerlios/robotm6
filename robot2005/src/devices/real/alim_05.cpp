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
    unsigned int l=ALIM_TENSION_NBR;
    unsigned char comData[ALIM_TENSION_NBR];
    bool status = device_->writeRead(ALIM_REQ_GET_TENSION, comData, l);
    if (!status) {
        // affiche le mesage d'erreur une seule fois
        LOG_ERROR("Alim05::getAllTension read error\n");
        return false;
    }
    for(int i=0; i<4 && i<ALIM_TENSION_NBR; i++) {
        if (comData[i]==0) {
            LOG_ERROR("Tension battery %d: 0 V\n", i);
        } else if (comData[i]==1) {
            LOG_ERROR("Battery %d is not connected\n", i);
        } else {
            tension[i] = 7200 + 50*comData[i]; // TODO use fab conversion method
            LOG_WARNING("Battery %d: %2.2fV\n", i, tension[i]/1000.);
        }
    }
    return true; 
}
/** @brief passe en mode 12V */
bool Alim05::mode12V()
{
    return false;
}
/** @brief passe en mode 28V */
bool Alim05::mode28V()
{
    return false;
}
/** @brief passe en mode 42V */
bool Alim05::mode42V()
{
    return false;
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
