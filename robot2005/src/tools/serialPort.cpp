/**
 * @file serialPort.cpp
 *
 * Driver for the communicating on /dev/tty
 *
 * @author Laurent Saint-Marcel
 * @date   2003/05/11
 */
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include "serialPort.h"
#include "log.h"
 
// --------------------------------------------------------------------
// SerialPort::SerialPort
// --------------------------------------------------------------------
SerialPort::SerialPort() : 
    RobotBase("SerialPort", CLASS_SERIAL_PORT),
    fd_(-1), ttyNbr_(0)
{
}

// --------------------------------------------------------------------
// SerialPort::~SerialPort
// --------------------------------------------------------------------
SerialPort::~SerialPort()
{
   close();
}

// --------------------------------------------------------------------
// SerialPort::open
// --------------------------------------------------------------------
bool SerialPort::open(int portNbr, 
                      bool blocking)
{
    char dev[20];
    close();

    ttyNbr_=portNbr;
    if (ttyNbr_ <  0) ttyNbr_ = 0;
    if (ttyNbr_ >= 2) ttyNbr_ = 1;

    sprintf(dev, "/dev/ttyS%d", ttyNbr_);
    int flag = O_RDWR | O_NOCTTY;
    if (!blocking) {
      // flag |= O_NONBLOCK;
    }
    fd_ = ::open(dev, flag);
    if (fd_ <0) { 
        LOG_ERROR("Cannot open serial port %s\n", dev);
        return false;
    }

    if( tcgetattr( fd_, &oldtio_ ) < 0 ) 
        goto fail;/* save current port settings */
    struct termios newtio;
    memset(&newtio, 0, sizeof(newtio));
    newtio.c_iflag = INPCK; // | IGNBRK | IGNPAR ; // input parity check
    newtio.c_lflag = 0;
    newtio.c_oflag = 0;
    newtio.c_cflag = B9600 | CREAD | CS8 | CLOCAL;
    if (blocking) {
      newtio.c_cc[ VMIN ] = 1;
      newtio.c_cc[ VTIME ] = 0;
    } else {
      newtio.c_cc[ VMIN ] = 0;
      newtio.c_cc[ VTIME ] = 1;
    }
    /*
    if( cfsetospeed( &newtio, B9600 ) < 0 ) goto fail;
    if( cfsetispeed( &newtio, B9600 ) < 0 ) goto fail;
    */
    // start the communication
    tcflush(fd_, TCIFLUSH);
    if( tcsetattr( fd_, TCSANOW, &newtio ) < 0 ) goto fail;
    
#ifdef linux
    // not used because we use poll instead
    //fcntl( fd_, F_SETOWN, getpid() );
    //fcntl( fd_, F_SETFL, FASYNC );
#endif
    LOG_OK("Serial port %s opened, %d\n", dev, fd_);
    return true;
 fail:
    LOG_ERROR("Serial port %s configuration error, cannot open device\n", 
              dev);
    close();
    return 1;
}

// --------------------------------------------------------------------
// SerialPort::close
// --------------------------------------------------------------------
bool SerialPort::close()
{
    if (fd_ >= 0) {
        tcsetattr(fd_, TCSANOW, &oldtio_);
        ::close(fd_);
        fd_ = -1;
        LOG_OK("SerialPort /dev/ttyS%d closed\n", ttyNbr_);
    }
    return true;
}

// --------------------------------------------------------------------
// SerialPort::read
// --------------------------------------------------------------------
// read data on serial port, return false if there is no data or if it 
// doesn't received enought data
// --------------------------------------------------------------------
bool SerialPort::read(Byte* buf, unsigned int& length, unsigned int retry)
{
    unsigned int k=0, res=0;
    unsigned int N=length;
    //printf("fd=%d\n", fd_);
    do {
      //printf("fd=%d
       res = ::read(fd_, buf+(N-length), length);
       //printf("res=%d\n", res);
       if (res == 0) {
           continue;
       } else if (res<0) {
           LOG_ERROR("Serial port %d. Read error(%d)\n", ttyNbr_, res);
           return false;
       } else { 
	 length-=res;
       }
    } while(k++<retry && length>0);
    if (length>0) {
        LOG_ERROR("Serial port %d. Not enough data on serial port "
		  "(read %d/%d)\n", ttyNbr_, N-length, N);
    }
    return (length == 0);
}

// --------------------------------------------------------------------
// SerialPort::write
// --------------------------------------------------------------------
bool SerialPort::write(Byte* buf, unsigned int& length)
{
    unsigned int length2 = 0;
    do {
        length2 = ::write(fd_, buf, length);
	if (length2 <= 0) return false;
	length -= length2;
    } while (length > 0);
    return true;
}

#ifdef SERIAL_MAIN
// --------------------------------------------------------------------
// main
// --------------------------------------------------------------------
int main(int argc, char**argv)
{
    int port=0;
    bool blocking=true;
    
    SerialPort tty;
    if (!tty.open(port, blocking)) {
        printf("Cannot open port\n");
        return -1;
    }
    tty.write(0xAA);
    unsigned char buf[10];
    if (!tty.read(buf)) {
        printf("Cannot read on port\n");
    } else {
        printf("read:0x%02x\n", buf[0]);
    }
    tty.close();
    return 0;
}
#endif
