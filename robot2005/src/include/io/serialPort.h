/**
 * @file serialPort.h
 *
 * @author florian loitsch
 *
 * way of communicating with Serial Ports.
 * Uses heavily code written by Laurent Saint Marcel.
 */

#pragma once

#include "robotBase.h"
#include "ioHost.h"

static const bool SERIAL_NON_BLOCKING = false;
static const bool SERIAL_BLOCKING = true;

/** 
 * @class SerialPort
 *
 * Serial Port under Linux
 */
class SerialPort : public IoHost, public RobotBase
{
public:
    SerialPort(int ttyNbr, bool isBlocking);
    virtual ~SerialPort();

public: // overwritten methods from IoHost
    const IoDeviceVector& listPorts();
    const IoDeviceScanInfoPairVector& scan();

private: // fields
    IoDeviceVector device_; // there's just one device
    IoDeviceScanInfoPairVector scannedDevice_; // one device only...
};
