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

static const unsigned int DEFAULT_READ_RETRIES = 3;

/** 
 * @class SerialPort
 *
 * Serial Port under Linux
 */
class SerialPort : public IoHost, public RobotBase
{
public:
    SerialPort(int ttyNbr, bool isBlocking);
    // retries implies non-blocking.
    SerialPort(int ttyNbr, unsigned int readRetries = DEFAULT_READ_RETRIES);
    virtual ~SerialPort();

public: // overwritten methods from IoHost
    const IoDeviceVector& listPorts();
    const IoDeviceScanInfoPairVector& scan();

private: // fields
    IoDeviceVector device_; // there's just one device
    IoDeviceScanInfoPairVector scannedDevice_; // one device only...
};
