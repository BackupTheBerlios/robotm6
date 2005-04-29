/**
 * @file ubart.h
 *
 * @author Florian Loitsch
 *
 * Ubart: Due to a lack of "native" serial ports, an external Atmel multiplexes
 * between several attached devices. An Ubart is such a multiplexed device.
 * In order to talk to an Ubart, one must first route using the
 * UbartMultiplexer, and only then the line's open. An Ubart reserves the
 * complete line for itself, and must hence lock it.
 * These restrictions make Ubarts blocking too. It is just not possible to
 * reserve the whole line forever...
 */

#pragma once

#include "io/ioDevice.h"
#include "io/ioHost.h"
#include "robotBase.h"
//#include "mthread.h"

static const unsigned int UBART_DEFAULT_ID = 0;
static const unsigned int UBART_MAX_DEVICES = 9;

// forward-declaration
class Ubart; // exists only in cpp-file.
class UbartLock; // exists only in cpp-file.

class UbartMultiplexer :
    public IoHost, public RobotBase
{
public: // constructors/destructors
    UbartMultiplexer(IoDevice* device);
    virtual ~UbartMultiplexer();

public: // overwritten IoHost-methods
    const IoDeviceVector& listPorts();
    const IoDeviceScanInfoPairVector& scan();
    
private: // UbartMultiplexer-methods
    friend class Ubart;
    friend class UbartLock;
    void lock();
    void unlock();
    void switchToUbart(Ubart* ubart);
    bool openUbart();
    bool closeUbart();
    bool isBlocking() const;
    bool write(IoByte* buf, unsigned int& length);
    bool read(IoByte* buf, unsigned int& length);

private: // fields
    IoDevice* device_;
    IoDeviceVector ubarts_;
    IoDeviceScanInfoPairVector foundDevices_;
    unsigned int openedUbarts_;
    unsigned int currentId_;
    unsigned int targetId_;
    pthread_mutex_t lock_;
    unsigned int lockCounter_;
};
