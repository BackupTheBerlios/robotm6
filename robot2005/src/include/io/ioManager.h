/**
 * @file ioManager.h
 *
 * @author Florian Loitsch
 *
 * manages all ioDevices (really!;). Every given IoHost is scanned, and the found
 * devices are stored in the manager. If anything needs a special ioDevice, it should
 * just ask here...
 */

#pragma once

#include "ioHost.h"
#include "ioDevice.h"
#include "ioDeviceId.h"

#include <map>
#include <set>

#define IoManager IoManagerCL::instance()
#define IoDeviceManager IoManagerCL::instance()

class IoManagerCL : public RobotBase {
public: // singleton
    /**
     * @brief returns the unique instance.
     * uses lazy initialization.
     */
    static IoManagerCL* instance();

private: // Constructors/Destructors
    IoManagerCL();
    ~IoManagerCL();

public: // methods
    /**
     * @brief submits an IoHost.
     * the manager will scan the host, and manage the found devices.
     */
    void submitIoHost(IoHost* host);

    /**
     * @brief lists all devices found
     */
    // TODO: declare method [flo]
    // const XXX listIoDevices() const;

    // TODO: way of getting unknown devices. (with the introduction of UBART
    //       this became more difficult...).

    /**
     * @brief returns the requested IoDevice if available. If the device is
     * not found, null is returned.
     * All devices obtained using this function must be released through
     * releaseIoDevice.
     */
    IoDevice* getIoDevice(IoId id);

    /**
     * @brief given a scanInfo finds the associated IoId. If none matches
     * IO_ID_NONE is returned.
     */
    static IoId scanInfo2ioId(IoByte scanInfo);
    
    /**
     * @brief given a ioId returns the associated info.
     */
    static const IoInfo& ioId2ioInfo(IoId id);

private: // fields
    static IoManagerCL* instance_;

    typedef std::map<IoId, IoDevice*> IoId2IoDeviceMap;
    typedef std::set<IoDevice*> IoDeviceSet;
    typedef std::vector<IoHost*> IoHostVector;
    IoId2IoDeviceMap id2deviceMap_;
    IoDeviceSet allocatedIoDevices_; // not yet used.
    IoHostVector ioHosts_; // not sure, if we need this field [flo].
};

inline IoManagerCL* IoManagerCL::instance() {
    if (!instance_)
	instance_ = new IoManagerCL();
    return instance_;
}
