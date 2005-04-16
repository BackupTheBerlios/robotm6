/**
 * @file serialPort.cpp
 *
 * @author florian loitsch
 * @author Laurent Saint-Marcel
 *
 * way of communicating with Serial Ports.
 * All low-level code (ie most of SerialDevice) by LSM.
 *
 * @date 2005/03/22
 *
 * Serial-device-code: date: 2003/05/11
 */

#include <termios.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include "io/serialPort.h"
#include "io/fileDescriptorPoller.h"
#include "log.h"

static const speed_t SERIAL_SPEEDS[SERIAL_SPEED_MAX] =
{
    B50, B75, B110, B134, B150, B200, B300,
    B600, B1200, B1800, B2400, B4800, B9600,
    B19200, B38400, B57600, B115200, B230400
};
// slightly redundant...
static const char* SERIAL_SPEEDS_STRINGS[SERIAL_SPEED_MAX] =
{ "50", "75", "110", "134", "150", "200", "300",
  "600", "1200", "1800", "2400", "4800", "9600",
  "19200", "38400", "57600", "115200", "230400"
};

class SerialDevice : public IoDevice {
public: // constructor/destructor
    SerialDevice(int ttyNbr, bool isBlocking, unsigned int maxRetries, SerialSpeed speed);
    ~SerialDevice();

public: // overwritten IoDevice methods
    bool open();
    bool isOpen() const;
    bool close();
    bool reset();

    bool read(IoByte* buf, unsigned int& length);
    bool write(IoByte* buf, unsigned int& length);
    bool isBlocking() const;

    bool canListen() const;
    void startListening();
    void stopListening();

    void pollerTask(int fd);

    int getTtyNbr() const { return ttyNbr_; }
    int getFileDescriptor() const { return fd_; }
    SerialSpeed getSpeed() const { return speed_; }
    bool setSpeed(SerialSpeed newSpeed);
    
private:
    int ttyNbr_;
    bool isBlocking_;
    int fd_;
    unsigned int maxRetries_;
    SerialSpeed speed_;
    struct termios oldtio_;
};

SerialDevice::SerialDevice(int ttyNbr, bool isBlocking,
			   unsigned int maxRetries, SerialSpeed speed)
    : IoDevice("SerialDevice", CLASS_SERIAL_DEVICE),
      ttyNbr_(ttyNbr), isBlocking_(isBlocking),
      fd_(-1), maxRetries_(maxRetries), speed_(speed)
{
}

SerialDevice::~SerialDevice() {
    close();
}

bool SerialDevice::open() {
    char dev[20];
    close();

    sprintf(dev, "/dev/ttyS%d", ttyNbr_);
    int flag = O_RDWR | O_NOCTTY;
    if (!isBlocking_) {
      // flag |= O_NONBLOCK;
    }
    fd_ = ::open(dev, flag);
    if (fd_ <0) { 
        LOG_ERROR("Cannot open serial port %s\n", dev);
        return false;
    }

    if (tcgetattr(fd_, &oldtio_) < 0) {
	LOG_ERROR("tcgetattr failed on serial %s\n", dev);
	close();
	return false;
    }
    struct termios newtio;
    memset(&newtio, 0, sizeof(newtio));
    newtio.c_iflag = INPCK; // | IGNBRK | IGNPAR ; // input parity check
    newtio.c_lflag = 0;
    newtio.c_oflag = 0;
    newtio.c_cflag = CREAD | CS8 | CLOCAL;// | B9600
    if (isBlocking_) {
      newtio.c_cc[ VMIN ] = 1;
      newtio.c_cc[ VTIME ] = 0;
    } else {
      newtio.c_cc[ VMIN ] = 0;
      newtio.c_cc[ VTIME ] = 1;
    }
    if ((cfsetospeed(&newtio, SERIAL_SPEEDS[speed_]) < 0) ||
	(cfsetispeed(&newtio, SERIAL_SPEEDS[speed_]) < 0)) {
	LOG_ERROR("Can't set speed to %s on device %s\n", SERIAL_SPEEDS_STRINGS[speed_], dev);
	close();
	return false;
    }

    // start the communication
    tcflush(fd_, TCIFLUSH);
    if(tcsetattr(fd_, TCSANOW, &newtio) < 0) {
	LOG_ERROR("tcsetattr failed on device %s\n", dev);
	close();
	return false;
    }
    
#ifdef linux
    // not used because we use poll instead
    //fcntl( fd_, F_SETOWN, getpid() );
    //fcntl( fd_, F_SETFL, FASYNC );
#endif
    LOG_OK("Serial port %s opened, %d\n", dev, fd_);
    return true;
}

bool SerialDevice::isOpen() const {
    return  fd_ >= 0;
}


bool SerialDevice::close() {
    if (fd_ >= 0) {
        tcsetattr(fd_, TCSANOW, &oldtio_);
        ::close(fd_);
        fd_ = -1;
        LOG_OK("SerialPort /dev/ttyS%d closed\n", ttyNbr_);
    }
    return true;
}

bool SerialDevice::reset() {
    if (isOpen())
    {
	close();
	open();
    }
    return true;
}

bool SerialDevice::read(IoByte* buf, unsigned int& length) { //, unsigned int retry)
    unsigned int retry = maxRetries_;
    unsigned int k=0, res=0;
    unsigned int N=length;
    length = 0; // nothing received yet.
    //printf("fd=%d\n", fd_);
    //printf("length=%d\n", N);
    do {
      //printf("fd=%d");
       res = ::read(fd_, buf + length, N-length);
       //printf("res=%d N=%d length=%d\n", res, N, length);
       if (res == 0) {
           continue;
       } else if (res<0) {
           //LOG_ERROR("Serial port %d. Read error(%d)\n", ttyNbr_, res);
           return false;
       } else {
	   length += res;
       }
    } while(k++ < retry && length < N);
    if (length != N) {
        LOG_WARNING("Serial port %d. Not enough data on serial port "
		    "(read %d/%d)\n", ttyNbr_, length, N);
    }
    return (length == N);
}

bool SerialDevice::write(IoByte* buf, unsigned int& length) {
    //printf("length=%d\n", length);
    int length2 = 0;
    do {
        length2 = ::write(fd_, buf, length);
	//printf("written %d bytes\n", length2);
	if (length2 <= 0) return false;
	length -= length2;
	//printf("length: %d\n", length);
    } while (length > 0);
    return true;
}

bool SerialDevice::isBlocking() const {
    return isBlocking_;
}

bool SerialDevice::canListen() const {
    return !isBlocking_;
}

void SerialDeviceCallBack(void* userData, int fd) {
    static_cast<SerialDevice*>(userData)->pollerTask(fd);
}

void SerialDevice::startListening() {
    if (isBlocking_)
    {
	LOG_ERROR("Can't listen on blocking Serial port %d\n", ttyNbr_);
	return;
    }
    FileDescriptorPoller->registerFileDescriptor(fd_,
						 SerialDeviceCallBack,
						 "Serial port Callback",
						 this);
}

void SerialDevice::pollerTask(int fd) {
    if (fd != fd_ || !isOpen()) return;
    unsigned int lread=0;
    static const unsigned int BUFFER_SIZE=255;
    static IoByte buf[BUFFER_SIZE];
    while (true) {
	lread = ::read(fd_, buf, BUFFER_SIZE);
	if (lread <= 0) {
	    LOG_ERROR("read error on tty %d\n", ttyNbr_);
	    break;
	}
	signalNewData(buf, lread);
	if (lread != BUFFER_SIZE) break;
    } 
}

void SerialDevice::stopListening() {
    FileDescriptorPoller->unregisterFileDescriptor(fd_);
}

bool SerialDevice::setSpeed(SerialSpeed newSpeed) {
    if (isOpen()) {
	LOG_ERROR("Can't set speed of open device (tty %d)." , ttyNbr_);
	return false;
    }
    LOG_INFO("Setting speed of tty %d to %s\n", ttyNbr_, SERIAL_SPEEDS_STRINGS[newSpeed]);
    speed_ = newSpeed;
    return true;
}
    


//=============================================================================
//=============================================================================

SerialPort::SerialPort(int ttyNbr, bool isBlocking, SerialSpeed speed)
    : RobotBase("SerialPort", CLASS_SERIAL_PORT), speed_(speed)
{
    unsigned int retries;
    if (isBlocking)
	retries = 0;
    else
	retries = DEFAULT_READ_RETRIES;
    addInitialDevice(ttyNbr, isBlocking, speed, retries);
}

SerialPort::SerialPort(int ttyNbr, SerialSpeed speed, unsigned int retries)
    : RobotBase("SerialPort", CLASS_SERIAL_PORT), speed_(speed)
{
    addInitialDevice(ttyNbr, false, speed, retries);
}

void SerialPort::addInitialDevice(int ttyNbr,
				  bool isBlocking,
				  SerialSpeed speed,
				  unsigned int retries) {
    SerialSpeed initialSpeed;
    if (speed == SERIAL_SPEED_SCAN) {
	initialSpeed = DEFAULT_SERIAL_SPEED;
    } else {
	initialSpeed = speed;
    }

    device_.push_back(new SerialDevice(ttyNbr, isBlocking, retries, initialSpeed));
}

SerialPort::~SerialPort() {
    delete(device_[0]);
}

const IoDeviceVector& SerialPort::listPorts() {
    return device_;
}

const IoDeviceScanInfoPairVector& SerialPort::scan() {
    // TODO_future: we could implement a scan for blocking-devices using select. [flo]
    LOG_INFO("Scanning serial port\n");
    IoByte scanAnswer;
    scannedDevice_.clear(); // new scan...
    const unsigned int NB_MAX_NB_SPEEDS = SERIAL_SPEED_MAX;
    SerialSpeed scanSpeeds[NB_MAX_NB_SPEEDS];
    unsigned int nbSpeeds=0;
    //printf("%d\n", (unsigned int)SERIAL_SPEED_MAX);
    if (speed_ == SERIAL_SPEED_SCAN) {
/*
      for(unsigned int i=0;i<NB_MAX_NB_SPEEDS; i++) {
        
	scanSpeeds[i]=(SerialSpeed)i;
      }
      nbSpeeds = NB_MAX_NB_SPEEDS;
*/
	scanSpeeds[0] = SERIAL_SPEED_38400;
	scanSpeeds[1] = SERIAL_SPEED_9600;
	nbSpeeds = 2;
    } else {
	scanSpeeds[0] = speed_;
	nbSpeeds = 1;
    }

    SerialDevice* device = static_cast<SerialDevice*>(device_[0]);
    SerialSpeed originalSpeed = device->getSpeed();
    bool answered = false;
    for (unsigned int i = 0; i < nbSpeeds; ++i) {
	device->setSpeed(scanSpeeds[i]);
	LOG_INFO("Scanning device tty %d at speed %s\n",
		 device->getTtyNbr(),
		 SERIAL_SPEEDS_STRINGS[scanSpeeds[i]]);
	
	if (IoHost::doIoDeviceScan(device, &scanAnswer)) {
	    IoDeviceScanInfoPair info;
	    info.device = device;
	    info.scanInfo = scanAnswer;
	    scannedDevice_.push_back(info);
	    answered = true;
	    break; // don't continue scanning at other speeds.
	}
    }
    if (!answered) device->setSpeed(originalSpeed);
    return scannedDevice_;
}
