#include "uart.h"
#include "log.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
//#include "uartIoCtlCmd.h"

static const char* uartDevs_[UART_PORT_NBR] = {
    "/dev/stty0" , 
    "/dev/stty1"
};

// ============================================================================
// =============================  class UartBasic   ===========================
// ============================================================================

// -------------------------------------------------------------------------
// Uart::Uart
// -------------------------------------------------------------------------
Uart::Uart() : 
    RobotBase("Uart", CLASS_UART), port_(UART_PORT_NBR), fd_(-1), 
    serialPort_(), isSerial_(false), needClearFifo_(false)
{
    info_ = uartInfos_[UART_NONE];
}

// -------------------------------------------------------------------------
// Uart::~Uart
// -------------------------------------------------------------------------
Uart::~Uart()
{
    close();
}
    
// -------------------------------------------------------------------------
// Uart::open
// -------------------------------------------------------------------------
bool Uart::open(UartPort port)
{
    port_ = port;
    if (fd_ < 0) {
        if (port == UART_STTY0) {
#ifndef TTYS0_NOT_USED
	  isSerial_=true;
	  serialPort_.open(0, SERIAL_NON_BLOCKING);
	  fd_ = serialPort_.getFileDescriptor();
#endif
        } else if (port == UART_STTY1) {
#ifndef TTYS1_NOT_USED
	  isSerial_=true;
	  serialPort_.open(1, SERIAL_NON_BLOCKING);
	  fd_ = serialPort_.getFileDescriptor();
#endif
        } else {
            // try to open it
            isSerial_=false;
            fd_ = ::open(uartDevs_[(int)port], O_RDWR);
            if (fd_ >= 0) {
                LOG_OK("port %d succesfully open\n", port);
                Uart::setReadTimeout(UART_DEFAULT_TIMEOUT);
            } else {
                LOG_ERROR("Can't open port %d\n", port);
            }
        }
    } else {
        // alread opened
        LOG_WARNING("port %d is already opened, close it before\n", port);
    }
    return (fd_ >= 0);
}

// -------------------------------------------------------------------------
// Uart::open
// -------------------------------------------------------------------------
bool Uart::open(UartPort port, UartInfo info)
{
    info_=info;
    return open(port);
}

// -------------------------------------------------------------------------
// Uart::close
// -------------------------------------------------------------------------
bool Uart::close()
{
    if (isSerial_) {
        serialPort_.close();
    } else {
        if (fd_ >= 0) {
            ::close(fd_);
            LOG_OK("%s closed\n", name());            
            info_ = uartInfos_[UART_NONE];
        } 
    }
    fd_ = -1;
    isSerial_=false;
    return true;
}

// -------------------------------------------------------------------------
// Uart::reset
// -------------------------------------------------------------------------
bool Uart::reset()
{
    LOG_FUNCTION();
    if (fd_>=0) {
      /* UartInfo info = info_;
         close();
	  return open(port_, info);*/
    }
    return true;
}

// -------------------------------------------------------------------------
// Uart::ping
// -------------------------------------------------------------------------
bool Uart::ping()
{
   UartPingId pingId=UART_NONE;
   if (requestPingId(pingId)) {
       if (info_.id == UART_NONE) {
	   info_ = getUartInfo(pingId);
	   if (info_.id == UART_NONE) {
	       LOG_ERROR("Unknown device pinging (0x%x)\n", pingId);
	   } else {
	     LOG_OK("%s pinging\n", name());
	     // TODO: IMO following line should be inserted. [flo]
	     return true;
	   }
       } else {
	   if (pingId == info_.pingId) {
	       LOG_OK("%s pinging\n", name());
	       return true;
	   } else {
	       LOG_ERROR("%s pinging but bad id: 0x%x (instead of 0x%x)\n",
			 name(), pingId, info_.pingId);
	       return false;
	   }
       }
   } else {
       if (info_.id != UART_NONE) {
	   LOG_ERROR("%s not pinging\n", name());
       } else {
	   LOG_ERROR("Device not pinging\n");
       }
   }
   return false;
}

// -------------------------------------------------------------------------
// Uart::requestPingId
// -------------------------------------------------------------------------
bool Uart::requestPingId(UartPingId& pingId)
{
    if (!write(uartPingReq_)) 
        return false;
    if (serialPort_.isOpened()) {
      usleep(100000);
    }
    // Pour les cartes qui peuvent etre maitre, la requete ping stoppe l'envoi
    // des donnees, puis la carte envoie son identifiant. Mais on peut avoir 
    // des donnees dans le buffer donc si la carte repond au ping, son 
    // identifiant est le dernier octect envoyé.
    bool doesNotAnswer = true;
    static const unsigned int UART_PING_BUF_SIZE=255;
    static UartByte buf[UART_PING_BUF_SIZE];
    unsigned int length=UART_PING_BUF_SIZE;
    do {
      length = ::read(fd_, buf, UART_PING_BUF_SIZE);

      if (length > 0) {
	pingId = buf[length-1];
	doesNotAnswer = false;
      }
    } while (length > 0);
    return !doesNotAnswer;
}

// -------------------------------------------------------------------------
// Uart::getUartInfo
// -------------------------------------------------------------------------
UartInfo Uart::getUartInfo(UartPingId pingId) 
{
     UartInfo info = uartInfos_[UART_NONE];
     for(int i=1; i<UART_NBR; i++) {
         if (uartInfos_[i].pingId == pingId) {
	   info = uartInfos_[i];
	   break;
         }
     }
     return info;
}

// -------------------------------------------------------------------------
// Uart::read
// -------------------------------------------------------------------------
bool Uart::read(UartByte* buf,  unsigned int &length)
{
    if (isSerial_) {
      return serialPort_.read(buf, length, 3);
    }
    if (fd_ >= 0) {
        unsigned int lread=0;
        unsigned int length2=0;
        do {
            lread = ::read(fd_, buf+length2, length-length2);
            if (lread == 0)  break;
            if (lread <= 0) {
                LOG_ERROR("%s read error\n", name());
            }
	    length2+=lread;
        } while(length2<length);
	bool result= (length2==length);
	length=length2;
        return result;
    } else { 
        return false;
    }
}

// -------------------------------------------------------------------------
// Uart::read
// -------------------------------------------------------------------------
bool Uart::read(UartByte* buf)
{
  unsigned int length=1;
  bool v=read(buf, length);
/*  int state=0;
  state=ioctl(fd_, GET_STATE, 0);
  printf ("Uart State=%d\n", state);*/
  return v;     
}

// -------------------------------------------------------------------------
// Uart::write
// -------------------------------------------------------------------------
bool Uart::write(UartByte buf)
{
  unsigned int length=1;
  return write(&buf, length);
}

// -------------------------------------------------------------------------
// Uart::write
// -------------------------------------------------------------------------
bool Uart::write(UartByte* buf, unsigned int& length)
{
    if (needClearFifo_) clearFifo();
    if (isSerial_) {
      return serialPort_.write(buf, length);
    }
    if (fd_>=0) {
        for(unsigned int i=0;i<length;++i) {
            if (::write((int)fd_, (const void*)(buf+i), (size_t)1) != 1) {
                LOG_ERROR("%s write error\n", name());
		length=i;
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

// -------------------------------------------------------------------------
// Uart::setReadTimeout
// -------------------------------------------------------------------------
bool Uart::setReadTimeout(int millisecond)
{
    if (serialPort_.isOpened()) return true;
    else return false;
}

// -------------------------------------------------------------------------
// Uart::checkMSerieIsAlive
// -------------------------------------------------------------------------
bool Uart::checkMSerieIsAlive()
{
    if (serialPort_.isOpened()) return false;
    else return false;
}

// -------------------------------------------------------------------------
// Uart::name
// -------------------------------------------------------------------------
const char* Uart::name()
{
    return info_.name;
}

// -------------------------------------------------------------------------
// Uart::clearFifo
// -------------------------------------------------------------------------
void Uart::clearFifo()
{
    LOG_WARNING("ClearFifo %s\n", name());
    // on n'a jamais plus de 20 octets de retard!
    static UartByte buf[20];
    unsigned int l =0;
    read(buf, l);
    needClearFifo_ = false;
}
