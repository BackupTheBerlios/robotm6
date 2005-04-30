#define BUMPER_MAPPING_INFO

#include "devices/implementation/bumper05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/bumperCom_05.h"
#include "bumperMapping.h"
#include "events.h"
#include "mthread.h"

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
    reset();
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
        skipCaptor_[i] = !(BumpersMapping[i].enableAtReset);
    }
    
    firstRead_ = true;
    return BumperCL::reset();
}

void Bumper05::disableCaptor(unsigned int captorId) 
{
    if (!skipCaptor_[captorId]) {
        skipCaptor_[captorId] = true;
        Events->unraise(BumpersMapping[captorId].evt);
    }
}

void Bumper05::enableCaptor(unsigned int captorId) 
{
    if (skipCaptor_[captorId]) {
        skipCaptor_[captorId] = false;
    }
}

bool Bumper05::getBridgeCaptors(BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR])
{
    unsigned int id=0;
    id = BRIDG_SHARP_LEFT;
    captors[BRIDGE_SHARP_LEFT] = getValue(BumpersMapping[id].byte,
                                          BumpersMapping[id].bit,
                                          BumpersMapping[id].reversed)?
        BRIDGE_DETECTED:BRIDGE_NO;
    id = BRIDG_SHARP_CENTER;
    captors[BRIDGE_SHARP_CENTER] =  getValue(BumpersMapping[id].byte,
                                             BumpersMapping[id].bit,
                                             BumpersMapping[id].reversed)?
        BRIDGE_DETECTED:BRIDGE_NO;
    id = BRIDG_SHARP_RIGHT;
    captors[BRIDGE_SHARP_RIGHT] =  getValue(BumpersMapping[id].byte,
                                            BumpersMapping[id].bit,
                                            BumpersMapping[id].reversed)?
        BRIDGE_DETECTED:BRIDGE_NO;
    id = BRIDG_BUMP_LEFT;
    captors[BRIDGE_BUMPER_LEFT] =  getValue(BumpersMapping[id].byte,
                                            BumpersMapping[id].bit,
                                            !BumpersMapping[id].reversed)?
        BRIDGE_DETECTED:BRIDGE_NO;
    id = BRIDG_BUMP_RIGHT;
    captors[BRIDGE_BUMPER_RIGHT] =  getValue(BumpersMapping[id].byte,
                                             BumpersMapping[id].bit,
                                             !BumpersMapping[id].reversed)?
        BRIDGE_DETECTED:BRIDGE_NO;
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
                    if (((newData[byte]&(bit)) != 0) ^ BumpersMapping[i].reversed) {
                        Events->raise(BumpersMapping[i].evt);
			if (i == EMERGENCY_STOP) Log->emergencyStopPressed(true);
			else if (i == START_JACK) Log->jackIn(true);
                    } else {
                        Events->unraise(BumpersMapping[i].evt);
			if (i == EMERGENCY_STOP) Log->emergencyStopPressed(false);
			else if (i == START_JACK) Log->jackIn(false);
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
  unsigned int l=4;
  unsigned char comData[4];
  bool status = device_->writeRead(BUMPER_REQ_GET_ALL, comData, l);
  if (!status && firstError) {
    // affiche le mesage d'erreur une seule fois
    LOG_ERROR("Bumper05::getAllCaptors read error\n");
  }
  if (status
      && ((comData[0] & 0xF0) != BUMPER_START_BITS)) {
    if (firstError) LOG_ERROR("Bumper05::getAllCaptors bad starting bit\n");
    LOG_DEBUG("getAllCaptors: bad starting bits 0x%2.2x, expected 0xA?\n", 
	      comData[0]);
    LOG_DEBUG("Bumper05 com: 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x \n", 
	      comData[0], comData[1], comData[2], comData[3]);
    // try to flush the bumper buffer
    l=4; device_->read(comData, l);
    status = false;
  }
  if (status) {
    // compute checksum
    data[0] = (comData[0]&0x0F) + (comData[3] & 0xF0);
    data[1] = comData[1];
    data[2] = comData[2];
    unsigned char checksum = (data[2]^data[1]^data[0]);
    unsigned char checksum4bits = (checksum&0x0F) ^ ((checksum&0xF0)>>4);
    if ((comData[3]&0x0F) != checksum4bits) {
      if (firstError) LOG_ERROR("Bumper05::getAllCaptors bad checksum\n");
      LOG_DEBUG("getAllCaptors: checksum error 0x%2.2x, expected 0x%2.2x\n", 
		(comData[3]&0x0F), checksum4bits);
      LOG_DEBUG("Bumper05 com: 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x \n", 
		comData[0], comData[1], comData[2], comData[3]);
      // try to flush the bumper buffer
      l=4; device_->read(comData, l);
      status = false;
    }
  }
  
  // verifie que les donnees sont valides:
  firstError = status;
  if (status) {
    // for debug only
    assert(2<BUMPER_DATA_NBR);
    LOG_DEBUG("Bumper05: 0x%2.2x 0x%2.2x 0x%2.2x \n", 
	      data[0], data[1], data[2]);
  }
  return status;
}

////////////////////////////////////////////////////////////////////////////////////

#ifdef TEST_MAIN

void* BumperThreadBody(void* bumper)
{
    while(1)
    {
	// TODO: remove magic number and replace with constant [flo]
	usleep(100000);
	static_cast<Bumper05*>(bumper)->periodicTask();
    }
    return NULL;
}

#include "io/serialPort.h"
int main(int argc, char* argv[]) 
{
    IoManager->submitIoHost(new SerialPort(0, SERIAL_SPEED_38400));
#ifndef GUMSTIX
    IoManager->submitIoHost(new SerialPort(1, SERIAL_SPEED_38400));
#endif
    IoManager->submitIoHost(new SerialPort(2, SERIAL_SPEED_38400));
    IoManager->submitIoHost(new SerialPort(3, SERIAL_SPEED_38400));
    EventsManagerCL* evtMgr = new EVENTS_MANAGER_DEFAULT();

    Bumper05 bumper;
    MThreadId thread = 0;
    MTHREAD_CREATE("Bumper Thread",
		   &thread,
		   NULL,
		   BumperThreadBody,
		   &bumper);
    int choice, i;
    bool loop=true, myBool=false;
    BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR];
    while(loop) {
      printf("Menu: 0=bridge status, 1=jack-AU-reboot-match, 2=exit\n>");
      scanf("%d", &choice);
      switch(choice) {
      case 0:
	bumper.getBridgeCaptors(captors);
	for(i=0;i<BRIDGE_CAPTORS_NBR;i++) {
	  printf("bridge[%d]=%s\n", i, captors[i]==BRIDGE_DETECTED?"Pont":"Trou");
	}
	break;
      case 1:
	myBool = false;
	bumper.getJackin(myBool);
	printf("Jackin=%s\n", b2s(myBool));
	bumper.getEmergencyStop(myBool);
	printf("emergencyStop=%s\n", b2s(myBool));
	bumper.getRebootSwitch(myBool);
	printf("reboot switch =%s\n", b2s(myBool));
	bumper.getMatchSwitch(myBool);
	printf("match switch =%s\n", b2s(myBool));
	break;
      case 2:
	loop=false;
	break;
      default:
	break;
      }
    }
    delete evtMgr;
    printf("Bye\n");
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
  IoManager->submitIoHost(new SerialPort(0, SERIAL_SPEED_38400));
#ifndef GUMSTIX
  IoManager->submitIoHost(new SerialPort(1, SERIAL_SPEED_38400));
#endif
  IoManager->submitIoHost(new SerialPort(2, SERIAL_SPEED_38400));
  IoManager->submitIoHost(new SerialPort(3, SERIAL_SPEED_38400));
  LogCL log;

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
