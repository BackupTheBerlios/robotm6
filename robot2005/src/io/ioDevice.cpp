#include "io/ioDevice.h"
#include "classConfig.h"

IoDevice::IoDevice()
    : RobotBase("IoDevice", CLASS_IO_DEVICE),
      callback_(NULL),
      userData_(NULL)
{
}

IoDevice::IoDevice(const char* name, ClassId id)
    : RobotBase(name, id),
      callback_(NULL),
      userData_(NULL)
{
}

IoDevice::~IoDevice()
{
}

void IoDevice::registerCallback(NonBlockingIoCallback cb,
				void* userData)
{
    callback_ = cb;
    userData_ = userData;
}

void IoDevice::unregisterCallback()
{
    callback_ = NULL;
    userData_ = NULL;
}

void IoDevice::signalNewData(const IoByte* data, int length)
{
    if (callback_ == NULL)
	return;
    callback_(userData_, data, length);
}
