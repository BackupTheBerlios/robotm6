/**
 * @file ioHost.h
 *
 * @author Florian Loitsch
 *
 * the IoHost interface allows to scan for connected devices.
 */

#pragma once

#include "robotBase.h"
#include "ioDevice.h"
#include <vector>

// TODO: should be replaced with robotTypes.h once these types are moved.
#include "types.h"

struct IoDeviceScanInfoPair {
    IoDeviceScanInfoPair() {}
    IoDeviceScanInfoPair(IoDevice* t_device, IoByte t_scanInfo)
	: device(t_device), scanInfo(t_scanInfo)
	{
	}
    
    IoDevice* device;
    // ATM the scan uses a ping to detect devices, and the pong contains the
    // id of the device. If this changes one must obviously change the type
    // of the info here.
    IoByte scanInfo;
};

typedef std::vector<IoDeviceScanInfoPair> IoDeviceScanInfoPairVector;

class IoHost
{
public: // constructors/destructors
    virtual ~IoHost() {}

public:
    /**
     * @brief returns a list of ports.
     * Ports are non-scanned ioDevices: it is not yet established, if
     * anything is connected to a particular port.
     * Sending something to these ports may end up at some device, or
     * just disappear...
     */
    virtual const IoDeviceVector& listPorts() = 0;

    /**
     * @brief scans for existing devices
     * the returned vector contains all found devices during this scan.
     * the contained devices are going to be destroyed by the IoHost.
     * TODO: can a scan be done at any moment? what if a device is open...? [flo]
     */
    virtual const IoDeviceScanInfoPairVector& scan() = 0;

protected:
    /**
     * @brief ioDevice-based ioHosts might want to use this method for the scan.
     * returns, true, if a reponse has been received. In this case scanAnswer
     * contains the received IoByte.
     */
    static bool doIoDeviceScan(IoDevice* device, IoByte* scanAnswer);
};
