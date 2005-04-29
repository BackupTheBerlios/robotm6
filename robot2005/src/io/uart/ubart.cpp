#include "io/ubart.h"
//#define LOG_DEBUG_ON
#include "log.h"

/**
 * @brief Ubarts need to lock the multiplexer, then make the multiplexer
 * switch, and can then only send/receive data.
 */
class Ubart : public IoDevice {
public: // constructors/destructors
    Ubart(UbartMultiplexer* multiplexer, unsigned int ubartId);
    ~Ubart();
public: // overwritten
    bool open();
    bool isOpen() const;
    bool close();
    bool reset();
    bool isBlocking() const;
    bool read(IoByte* buf, unsigned int& length);
    bool write(IoByte* buf, unsigned int& length);
    bool writeRead(IoByte* sendBuffer, unsigned int& sendLength,
		   IoByte* receiveBuffer, unsigned int& receiveLength);

public: // Ubart-specific methods
    UbartMultiplexer* getUbartMultiplexer();
    unsigned int getUbartId() const;

private: // fields
    UbartMultiplexer* multiplexer_;
    bool isOpen_;
    const unsigned int ubartId_;
};

/**
 * @brief locks the UbartMultiplexer for the given Ubart.
 */
class UbartLock {
public:
    UbartLock(Ubart* ubart)
	: multiplexer_(ubart->getUbartMultiplexer())
	{
	    multiplexer_->lock();
	    multiplexer_->switchToUbart(ubart);
	    locked_ = true;
	}
    ~UbartLock()
	{
	    unlock();
	}

    void earlyUnlock()
	{
	    unlock();
	}

private:
    void unlock()
	{
	    if (locked_)
		multiplexer_->unlock();
	    locked_ = false;
	}

private:
    bool locked_;
    UbartMultiplexer* multiplexer_;
};

Ubart::Ubart(UbartMultiplexer* multiplexer, unsigned int ubartId)
    : IoDevice("Ubart", CLASS_UBART),
      multiplexer_(multiplexer),
      isOpen_(false),
      ubartId_(ubartId)
{
}

Ubart::~Ubart()
{
    close();
}

bool Ubart::open()
{
    if (isOpen_) return true;
    return multiplexer_->openUbart();
}

bool Ubart::isOpen() const
{
    return isOpen_;
}

bool Ubart::close()
{
    if (!isOpen_) return true;
    return multiplexer_->closeUbart();
}

// we *don't* reset the multiplexer.
bool Ubart::reset()
{
    return true;
}

bool Ubart::isBlocking() const
{
    return multiplexer_->isBlocking();
}

bool Ubart::read(IoByte* buf, unsigned int& length)
{
    UbartLock(this);
    return multiplexer_->read(buf, length);
}

bool Ubart::write(IoByte* buf, unsigned int& length)
{
    UbartLock(this);
    return multiplexer_->write(buf, length);
}

bool Ubart::writeRead(IoByte* sendBuffer, unsigned int& sendLength,
		      IoByte* receiveBuffer, unsigned int& receiveLength)
{
    LOG_DEBUG("writeRead of Ubart %d\n", ubartId_);
    // DEBUG:
    IoByte buffer[16];
    unsigned int maxLength = 16;
    read(buffer, maxLength);
    if (maxLength != 16) {
	LOG_ERROR("writeRead has input before send request (received %d bytes).\n", 16 - maxLength);
	for (unsigned int i = 0; i < 16 - maxLength; ++i) {
	    printf("0x%2.2x ", buffer[i]);
	}
    }
    UbartLock(this);
    return write(sendBuffer, sendLength)
	&& read(receiveBuffer, receiveLength);
}

UbartMultiplexer* Ubart::getUbartMultiplexer()
{
    return multiplexer_;
}

unsigned int Ubart::getUbartId() const
{
    return ubartId_;
}

/************************************************************************
 * UbartMultiplexer
 ************************************************************************/

UbartMultiplexer::UbartMultiplexer(IoDevice* device)
    : RobotBase("UbartMultiplexer", CLASS_UBART_MULTIPLEXER),
      device_(device),
      openedUbarts_(0),
      currentId_(UBART_DEFAULT_ID),
      targetId_(UBART_DEFAULT_ID),
      lockCounter_(0)
{
    pthread_mutex_init(&lock_, NULL);
    for (unsigned int i = 0; i < UBART_MAX_DEVICES; ++i)
	ubarts_.push_back(new Ubart(this, i));
}

UbartMultiplexer::~UbartMultiplexer() {
    for (unsigned int i = 0; i < foundDevices_.size(); ++i)
	delete(ubarts_[i]);
    device_->close(); // should not be necessary after deleting all foundDevices
}

const IoDeviceVector& UbartMultiplexer::listPorts() {
    return ubarts_;
}

const IoDeviceScanInfoPairVector& UbartMultiplexer::scan() {
    foundDevices_.clear();
    for (unsigned int i = 0; i < ubarts_.size(); ++i) {
        LOG_INFO("ubart scan %d\n", i);
        switchToUbart(static_cast<Ubart*>(ubarts_[i]));
	IoByte scanAnswer;
	if (doIoDeviceScan(ubarts_[i], &scanAnswer))
	    foundDevices_.push_back(IoDeviceScanInfoPair(ubarts_[i], scanAnswer));
    }
    return foundDevices_;
}

void UbartMultiplexer::lock() {
    pthread_mutex_lock(&lock_);
    lockCounter_++;
}

void UbartMultiplexer::unlock() {
    if (--lockCounter_ == 0) {
	pthread_mutex_unlock(&lock_);
    }
}

void UbartMultiplexer::switchToUbart(Ubart* ubart) {
    LOG_DEBUG("switching to ubart %d\n", ubart->getUbartId());
    targetId_ = ubart->getUbartId();
}

bool UbartMultiplexer::openUbart() {
    if (openedUbarts_ > 0 || device_->isOpen()) {
	openedUbarts_++;
	return true;
    } else {
	if (device_->open()) {
	    openedUbarts_++;
	    return true;
	} else
	    return false;
    }
}

bool UbartMultiplexer::closeUbart() {
    if (--openedUbarts_ == 0) {
	return device_->close();
    } else
	return true;
}

bool UbartMultiplexer::isBlocking() const {
    return device_->isBlocking();
}

bool UbartMultiplexer::write(IoByte* buf, unsigned int& length) {
    currentId_ = targetId_;
    LOG_DEBUG("write %d bytes on ubart %d\n", length, currentId_);
    // Hardcoded protocol:
    //   first 4 bits are device-id.
    //   resting 4 bits are length of request.
    bool result = true;
    unsigned int sent = 0;
    unsigned int stillToSendThisTurn=0;
    while (result && (sent < length)) {
	unsigned int toSendThisTurn = (length - sent);
	// we can only send 4 bits of length to the Ubart
	// 4 bits -> 15 chars
	if (toSendThisTurn > 15) toSendThisTurn = 15;
	stillToSendThisTurn = toSendThisTurn;
	IoByte address = ((static_cast<unsigned char>(targetId_) << 4)
			   | ((static_cast<unsigned char>(stillToSendThisTurn)) & 0x0F)) ;
	result = 
	  device_->write(address)
	  && device_->write(buf + sent, stillToSendThisTurn);
#ifdef LOG_DEBUG_ON
	  printf("address=0x%2.2x\n",address);
	for(unsigned int i=0;i<toSendThisTurn; i++) {
	  char* buf2=(char*)buf+sent;
	  printf("Ox%2.2x(%c)", buf2[i], buf2[i]);
	}
	printf("\n");
	printf("write result=%s, toSend=%d, stillToSend=%d\n", 
	       b2s(result), toSendThisTurn, stillToSendThisTurn);
#endif
	// update var, even if it didn't work.
	sent += toSendThisTurn - stillToSendThisTurn;
    }
    length = sent;
    return result;
}

bool UbartMultiplexer::read(IoByte* buf, unsigned int& length) {
    if (currentId_ != targetId_) {
	// Hardcoded protocol:
	//   first 4 bits are device-id.
	//   resting 4 bits are length of request.
	// In this case we just switch the ubart (so length == 0).
	device_->write(static_cast<unsigned char>(targetId_) << 4);
	currentId_ = targetId_;
    }
    LOG_DEBUG("Read on ubart %d\n", currentId_);
    if(device_->read(buf, length)) {
      if (length>0) { LOG_DEBUG("Read OK: 0x%2.2x\n", buf[0]) };
      return true;
    } else {
      LOG_DEBUG("Read error\n");
      return false;
    }
}
