#include "io/ioHost.h"
//#include "log.h"

// TODO: we really want to know what's happening in here... [flo]

#define LOG_INFO(...) do{}while(0)
#define LOG_ERROR(...) do{}while(0)

static const IoByte SCAN_REQ = 0xAA;

bool IoHost::doIoDeviceScan(IoDevice* device,
			    IoByte* scanAnswer)
{
    LOG_INFO("Scanning serial port\n");

    if (device->isBlocking()) {
	LOG_ERROR("Can't scan blocking device.\n");
	return false;
    }
    
    IoDeviceOpenerCloser openerCloser(device);

    if (!device->write(SCAN_REQ)) {
	LOG_ERROR("Couldn't write to device.\n");
        return false;
    }
    
    LOG_INFO("scan written\n");
    if (device->isOpen()) {
	LOG_INFO("sleeping\n");
	usleep(100000);
    }

    // Pour les cartes qui peuvent etre maitre, la requete scan stoppe l'envoi
    // des donnees, puis la carte envoie son identifiant. Mais on peut avoir 
    // des donnees dans le buffer donc si la carte repond au scan, son 
    // identifiant est le dernier octet envoyé.
    bool doesAnswer = false;
    static const unsigned int SCAN_BUF_SIZE=16;
    static IoByte buf[SCAN_BUF_SIZE];
    unsigned int length=SCAN_BUF_SIZE;

    do {
	device->read(buf, length);
	
	if (length > 0) {
	    (*scanAnswer) = buf[length-1];
	    doesAnswer = true;
	}
    } while (length == SCAN_BUF_SIZE);

    if (doesAnswer) {
	LOG_INFO("Answer received 0x%2.2x\n", scanAnswer);
    } else {
	LOG_INFO("No answer.\n");
    }
    return doesAnswer;
}
