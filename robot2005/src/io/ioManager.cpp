#include "io/ioManager.h"
#include "log.h"

IoManagerCL* IoManagerCL::instance_ = NULL;

IoManagerCL::IoManagerCL()
    : RobotBase("IoManager", CLASS_IO_MANAGER)
{
}

IoManagerCL::~IoManagerCL()
{
}

void IoManagerCL::submitIoHost(IoHost* host)
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
	    id2deviceMap_[id] = foundDevices[0].device;
	}
	else
	{
	    LOG_ERROR("Dropping unknown device with scanInfo (0x%x)\n",
		      foundDevices[0].scanInfo);
	}
    }
    ioHosts_.push_back(host);
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
