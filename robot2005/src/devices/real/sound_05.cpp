#include "implementation/sound05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/soundCom_05.h"
#include "log.h"

Sound05CL::Sound05CL() : device_(NULL)
{
    device_ = IoManager->getIoDevice(IO_ID_SOUND_05);
    if (device_ != NULL) {
	if (device_->open()) {
	    LOG_OK("Initialization Done\n");
	} else {
	    device_=NULL;
	    LOG_ERROR("device-open for alim 05 failed.\n");
	}
    } else {
        LOG_ERROR("alim 05 device not found!\n");
    }   
}

Sound05CL::~Sound05CL() 
{
    if (device_) device_->close();
}

void Sound05CL::play(SoundId sound, 
                     SoundPriority priority)
{

}

void Sound05CL::clearStack()
{

}
