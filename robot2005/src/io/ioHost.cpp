#include "io/ioHost.h"

bool IoHost::doIoDeviceScan(IoDevice* device,
			    IoByte* scanAnswer)
{
    IoDeviceOpenerCloser openerCloser(device);

    // TODO: get this const from somewhere else [flo]
    const IoByte SCAN_REQ = 0xAA;
    if (!device->write(SCAN_REQ))  // TODO: not uartPingReq_, but ScanReq...
        return false;
    if (device->isOpen()) {
	usleep(100000);
    }
    // Pour les cartes qui peuvent etre maitre, la requete ping stoppe l'envoi
    // des donnees, puis la carte envoie son identifiant. Mais on peut avoir 
    // des donnees dans le buffer donc si la carte repond au ping, son 
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
    } while (length > 0);
    if (doesAnswer)
	return true;
    else
	return false;
}
