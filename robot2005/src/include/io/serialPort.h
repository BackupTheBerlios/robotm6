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

enum SerialSpeed {
    SERIAL_SPEED_SCAN = -1,
    SERIAL_SPEED_50 = 0,
    SERIAL_SPEED_75,
    SERIAL_SPEED_110,
    SERIAL_SPEED_134,
    SERIAL_SPEED_150,
    SERIAL_SPEED_200,
    SERIAL_SPEED_300,
    SERIAL_SPEED_600,
    SERIAL_SPEED_1200,
    SERIAL_SPEED_1800,
    SERIAL_SPEED_2400,
    SERIAL_SPEED_4800,
    SERIAL_SPEED_9600,
    SERIAL_SPEED_19200,
    SERIAL_SPEED_38400,
    SERIAL_SPEED_57600,
    SERIAL_SPEED_115200,
    SERIAL_SPEED_230400,
    SERIAL_SPEED_MAX
};

/* if a device's serial-speed is set to SERIAL_SPEED_SCAN and no scan has been
 * done yet, DEFAULT_SERIAL_SPEED is used as default. If the scan fails,
 * the speed is set back to DEFAULT_SERIAL_SPEED too.*/
static const SerialSpeed DEFAULT_SERIAL_SPEED = SERIAL_SPEED_9600;

/** 
 * @class SerialPort
 *
 * Serial Port under Linux
 */
class SerialPort : public IoHost, public RobotBase
{
public:
    SerialPort(int ttyNbr, bool isBlocking, SerialSpeed speed = SERIAL_SPEED_SCAN);
    // retries implies non-blocking.
    SerialPort(int ttyNbr,
	       unsigned int readRetries = DEFAULT_READ_RETRIES,
	       SerialSpeed = SERIAL_SPEED_SCAN);
    virtual ~SerialPort();

public: // overwritten methods from IoHost
    const IoDeviceVector& listPorts();
    const IoDeviceScanInfoPairVector& scan();

private: // methods
    void addInitialDevice(int ttyNbr,
			  bool isBlocking,
			  unsigned int retries,
			  SerialSpeed speed);
    
private: // fields
    IoDeviceVector device_; // there's just one device
    IoDeviceScanInfoPairVector scannedDevice_; // one device only...
    SerialSpeed speed_;
};
