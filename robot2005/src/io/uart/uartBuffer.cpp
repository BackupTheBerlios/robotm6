#include "uart.h"
#include "log.h"
#include <unistd.h>
#include "ioManager.h"
#include "mthread.h"

#include <fcntl.h>
#include <sys/ioctl.h>
//#include "uartIoCtlCmd.h"


// ============================================================================
// ============================  class UartBuffer   ===========================
// ============================================================================


// -------------------------------------------------------------------------
// UartBufferFileDescriptorTask
// -------------------------------------------------------------------------
void UartBufferFileDescriptorTask(void* userData, int fd)
{
    static_cast<UartBuffer*>(userData)->fileDescriptorTask(fd);
}


// -------------------------------------------------------------------------
// UartBuffer::UartBuffer
// -------------------------------------------------------------------------
UartBuffer::UartBuffer() : 
    Uart(), filterFn_(NULL), timeout_(UART_DEFAULT_TIMEOUT), mutex_()
{
    strcpy(robotBaseName_, "UartBuffer"); // change the name used in the logs
    buffer_ = new UartBuf;
}

// -------------------------------------------------------------------------
// UartBuffer::~UartBuffer
// -------------------------------------------------------------------------
UartBuffer::~UartBuffer()
{
    delete buffer_;
}

// -------------------------------------------------------------------------
// UartBuffer::open
// -------------------------------------------------------------------------
bool UartBuffer::open(UartPort port)
{
    // open the port but don't use UART_DEFAULT_TIMEOUT during periodic read request
    LOG_DEBUG("open\n");
    bool result = Uart::open(port);
    result &= Uart::setReadTimeout(10);
    if (result) {
        IOMGR->registerFileDescriptor(fd_,
                                      UartBufferFileDescriptorTask, 
                                      "UartBufferFileDescriptorTask", 
                                      this);
    }
    LOG_DEBUG("open done\n");
    return result;
}

// -------------------------------------------------------------------------
// UartBuffer::close
// -------------------------------------------------------------------------
bool UartBuffer::close() 
{
    LOG_DEBUG("Unregister fd\n");
    if (fd_ > 0) {
        IOMGR->unregisterFileDescriptor(fd_);
    }
    LOG_DEBUG("Unregister fd done\n");
    return Uart::close();
}

// -------------------------------------------------------------------------
// UartBuffer::reset
// -------------------------------------------------------------------------
bool UartBuffer::reset()
{
    LOG_FUNCTION();
    LOG_DEBUG("clearBuffer\n");
    clearFifo();
    LOG_DEBUG("clearBuffer Done\n");
    return Uart::reset();
}    

// -------------------------------------------------------------------------
// UartBuffer::read
// -------------------------------------------------------------------------
bool UartBuffer::read(UartByte* buf,  unsigned int &length)
{
    LOG_INFO("uartbufer read\n");
    unsigned int length2=0;
    Millisecond mtimer=0;
    //printf("/");
    int maxCounter=0;
    do {
        while (maxCounter++<100
	       && length2 < length 
                && buffer_
                && !buffer_->empty()) {
            Lock localLock(&mutex_);
            buf[length2] = buffer_->front();
            buffer_->pop_front();
            localLock.earlyUnlock();
            ++length2;
	    // printf(".");
        }
        if (maxCounter >= 100) {
	    //printf("#\n");
	    return false;
	}
        if (length2 == length) {
	    //printf("°\n");
	    return true;
	}
        usleep(10000);
        mtimer += 10; 
        //printf("*");
    } while(mtimer < timeout_);
    //printf("@\n");
    length = length2;
    return false;
}
// -------------------------------------------------------------------------
// UartBuffer::read
// -------------------------------------------------------------------------
bool UartBuffer::read(UartByte* buf)
{
    unsigned int length=1;
    return read(buf, length);
}

// -------------------------------------------------------------------------
// UartBuffer::registerFilterFunction
// -------------------------------------------------------------------------
void UartBuffer::registerFilterFunction(UartFilterFun filter)
{
    filterFn_ = filter;
}

// -------------------------------------------------------------------------
// UartBuffer::registerFilterFunction
// -------------------------------------------------------------------------
bool UartBuffer::setReadTimeout(Millisecond timeout)
{
    timeout_=timeout;
    return true;
}

// -------------------------------------------------------------------------
// UartBuffer::fileDescriptorTask
// -------------------------------------------------------------------------
void UartBuffer::fileDescriptorTask(int fd)
{
    if (fd != fd_ || !isOpened()) return;
    unsigned int lread=0;
    static const unsigned int UARTBUFFER_SIZE=255;
    static UartByte buf[UARTBUFFER_SIZE];
    do {
      //printf("uartBufRead\n");
	lread = ::read(fd_, buf, UARTBUFFER_SIZE); /// >>>>>>>>>>>>
      if (lread <= 0) {
	LOG_ERROR("%s read error\n", name());
	break;
      }
      for (unsigned int i=0; i<lread; i++) {
	if (!filterFn_ || !filterFn_(buf[i])) {
            Lock localLock(&mutex_);
            buffer_->push_back(buf[i]);
	}
      }
      if (lread != UARTBUFFER_SIZE) break;
    } while(1);
}

// -------------------------------------------------------------------------
// UartBuffer::clearFifo
// -------------------------------------------------------------------------
void UartBuffer::clearFifo()
{
    LOG_WARNING("ClearFifo %s\n", name());
    Lock localLock(&mutex_);      
    if (buffer_) buffer_->clear();
}
