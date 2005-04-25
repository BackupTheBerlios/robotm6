#include "io/ioManager.h"
#include "io/ubart.h"
#include "log.h"
#include <algorithm>

IoManagerCL* IoManagerCL::instance_ = NULL;

IoManagerCL::IoManagerCL()
    : RobotBase("IoManager", CLASS_IO_MANAGER)
{
}

IoManagerCL::~IoManagerCL() {
    IoHostVector::iterator it;
    for (it = autoSubmitted_.begin();
	 it != autoSubmitted_.end();
	 ++it)
    {
	delete (*it);
    }
    autoSubmitted_.clear();
}

void IoManagerCL::submitIoHost(IoHost* host)
{
    rescan(host);
}

void IoManagerCL::rescan() {
    // don't use iterator, as we might modify the ioHosts_-vector.
    for (unsigned int i = 0; i < ioHosts_.size(); ++i)
	rescan(ioHosts_[i]);
}

void IoManagerCL::rescan(IoHost* host)
{
    const IoDeviceScanInfoPairVector& foundDevices = host->scan();
    for (unsigned int i = 0; i < foundDevices.size(); ++i)
    {
	IoByte scanInfo = foundDevices[i].scanInfo;
	IoId id = scanInfo2ioId(scanInfo);
	if (id != IO_ID_NONE)
	{
	    IoInfo info = ioId2ioInfo(id);
	    LOG_INFO("%s is connected (scan-info: 0x%x)\n", info.name, scanInfo);
	    if (id2deviceMap_.find(id) == id2deviceMap_.end()) {
		id2deviceMap_[id] = foundDevices[i].device;
		if (isIoHost(id)) autoSubmit(id);
	    }
	}
	else
	{
	    LOG_ERROR("Dropping unknown device with scanInfo (0x%x)\n",
		      foundDevices[i].scanInfo);
	}
    }
    if (find(ioHosts_.begin(), ioHosts_.end(), host) == ioHosts_.end())
	ioHosts_.push_back(host);
}

bool IoManagerCL::isIoHost(IoId id) const {
    return (id == IO_ID_UBART_05);
}

void IoManagerCL::autoSubmit(IoId id) {
    LOG_INFO("Autosubmitting IoHost\n");
    // *small* hack:
    // At the moment the id must be IO_ID_UBART_05
    // -> shortcut...
    IoDevice* device = getIoDevice(id);
    IoHost* ubartMulti = new UbartMultiplexer(device);
    autoSubmitted_.push_back(ubartMulti);
    submitIoHost(ubartMulti);
}

IoDevice* IoManagerCL::getIoDevice(IoId id)
{
    IoId2IoDeviceMap::const_iterator it = id2deviceMap_.find(id);
    if (it != id2deviceMap_.end())
	return it->second;
    else
	return NULL;
}

IoId IoManagerCL::scanInfo2ioId(IoByte scanInfo)
{
    // dump linear search. we don't have many elements, and
    // this way we can do this staticly (which is not a requirement).
    for (int i = 0; i < IO_ID_NBR; ++i)
    {
	if (ioInfos_[i].pingId == scanInfo)
	    return ioInfos_[i].id;
    }
    return IO_ID_NONE;
}

const IoInfo& IoManagerCL::ioId2ioInfo(IoId id)
{
    return ioInfos_[id];
}
