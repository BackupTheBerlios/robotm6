#include "devices/implementation/bumper05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/bumperCom_05.h"
#include "bumperMapping.h"
#include "events.h"

#ifdef TEST_MAIN
#define LOG_DEBUG_ON
#endif

#include "log.h"

/** @brief Constructeur */
Bumper05::Bumper05() : device_(NULL), firstRead_(true)
{
    device_ = IoManager->getIoDevice(IO_ID_BUMPER_05);
    if (device_ != NULL) {
	if (device_->open()) {
	    LOG_OK("Initialization Done\n");
	} else {
	    device_=NULL;
	    LOG_ERROR("Device-open for Bumper05 failed.\n");
	}
    } else {
        LOG_ERROR("Bumper05 device not found!\n");
    } 
}

Bumper05::~Bumper05() 
{
    if (device_) device_->close();
}

bool Bumper05::reset()
{
    // check bumper ids
    for(unsigned int i=0; i < BUMPERS_NBR; i++) {
        assert((BumperId)i == BumpersMapping[i].id);
        skipCaptor_[i] = BumpersMapping[i].enableAtReset;
    }
    
    firstRead_ = true;
    return BumperCL::reset();
}

void Bumper05::disableCaptor(unsigned int captorId) 
{
    skipCaptor_[captorId] = true;
    Events->unraise(BumpersMapping[captorId].evt);
}

bool Bumper05::getBridgeCaptors(BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR])
{
    
    return true;
}

inline bool Bumper05::getValue(int byte, int bit, bool reversed) 
{
  return (((data_[byte]&(1<<bit))?true:false) ^ reversed);
}

bool Bumper05::getEmergencyStop(bool& emergencyStop)
{
  unsigned int id = EMERGENCY_STOP;
  emergencyStop = getValue(BumpersMapping[id].byte,
			   BumpersMapping[id].bit,
			   BumpersMapping[id].reversed);
  return true;
}

bool Bumper05::getJackin(bool& jackin)
{
  unsigned int id = START_JACK;
  jackin = getValue(BumpersMapping[id].byte,
		    BumpersMapping[id].bit,
		    BumpersMapping[id].reversed);
  return true; 
}

bool Bumper05::getMatchSwitch(bool& match)
{
  unsigned int id = SWITCH_MATCH;
  match = getValue(BumpersMapping[id].byte,
		   BumpersMapping[id].bit,
		   BumpersMapping[id].reversed);
  return true;
}

bool Bumper05::getRebootSwitch(bool& reboot)
{
  unsigned int id = SWTICH_REBOOT;
  reboot = getValue(BumpersMapping[id].byte,
		    BumpersMapping[id].bit,
		    BumpersMapping[id].reversed);
  return true;
}


/** @brief function that read all captors and run the corresponding events */
void Bumper05::periodicTask()
{
    unsigned char newData[BUMPER_DATA_NBR];
    if (getAllCaptors(newData)) {
        for(unsigned int i=0; i < BUMPERS_NBR; i++) {
            if (skipCaptor_[i]) continue;
            unsigned char bit = (1<<BumpersMapping[i].bit);
            unsigned char byte = BumpersMapping[i].byte;
            if (BumpersMapping[i].evt != EVENTS_GROUP_NONE) {
                if (firstRead_ || 
                    ((data_[byte]&(bit)) != (newData[byte]&(bit)))) {
                    if ((newData[byte]&(bit)) ^ BumpersMapping[i].reversed) {
                        Events->raise(BumpersMapping[i].evt);
                    } else {
                        Events->unraise(BumpersMapping[i].evt);
                    }
                }
            }
        }
        memcpy(data_, newData, BUMPER_DATA_NBR);
        firstRead_ = false;
    }
}

/** @brief read all captors status: do this before other get functions */
bool Bumper05::getAllCaptors()
{
  return getAllCaptors(data_);
}

/** @brief read all captors status: do this before other get functions */
bool Bumper05::getAllCaptors(unsigned char data[BUMPER_DATA_NBR])
{
  if (!device_) return false;
  static bool firstError=true;
  unsigned int l=BUMPER_DATA_NBR;
  bool status = device_->writeRead(BUMPER_REQ_GET_ALL, data, l);
  if (!status && firstError) {
    // affiche le mesage d'erreur une seule fois
    LOG_ERROR("Bumper05::getAllCaptors read error\n");
  }
  firstError = status;
  if (status) {
    // for debug only
    assert(2<BUMPER_DATA_NBR);
    LOG_DEBUG("Bumper05: 0x%2.2x 0x%2.2x 0x%2.2x \n", data[0], data[1], data[2]);
  }
  return status;
}

////////////////////////////////////////////////////////////////////////////////////

#ifdef TEST_MAIN

#include "io/serialPort.h"
int main(int argc, char* argv[]) 
{
    IoManager->submitIoHost(new SerialPort(0, false));
    IoManager->submitIoHost(new SerialPort(1, false));
    EventsManagerCL* evtMgr = new EVENTS_MANAGER_DEFAULT();

    Bumper05 bumper;
    while(true) {
        bumper.periodicTask();
        usleep(100000);
    }

    delete evtMgr;
    return 0;
}

#endif

#ifdef GET_BUMPER_MAIN

#include "io/serialPort.h"
int main(int argc, char* argv[]) 
{
  if (argc!=3) {
    printf("Usage: %s byte, bit\n  "
	   "Return the value of a bit in the 3 bytes returned "
	   "by the bumper device\n", argv[0]);
    return -1;
  }
  ClassConfig::find(CLASS_SERIAL_DEVICE)->setVerboseLevel(VERBOSE_NO_MESSAGE);
  ClassConfig::find(CLASS_SERIAL_PORT)->setVerboseLevel(VERBOSE_NO_MESSAGE);
  IoManager->submitIoHost(new SerialPort(0, false));
  IoManager->submitIoHost(new SerialPort(1, false));
  if (IoManager->getIoDevice(IO_ID_BUMPER_05)) {
    Bumper05 bumper;
    unsigned char data[BUMPER_DATA_NBR];
    bumper.getAllCaptors(data);
    return (data[atoi(argv[1])] & (1<<atoi(argv[2])))?1:0;
  } else {
    printf("Bumper device not found\n");
  }
  return -1;
}

#endif
