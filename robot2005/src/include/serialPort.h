/**
 * @file serialPort.h
 *
 * Driver for the communicating on /dev/tty
 *
 * @author Laurent Saint-Marcel
 * @date   2003/05/11
 */

#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

#include <termios.h>
#include "robotBase.h"

static const bool SERIAL_NON_BLOCKING = false;
static const bool SERIAL_BLOCKING = true;

/** 
 * @class SerialPort
 *
 * Gestion d'un port serie du pc sous linux
 */
class SerialPort : public RobotBase
{
public:
    SerialPort();
    virtual ~SerialPort();
    
    bool open(int portNbr, bool blocking);
    bool close();
    int  getFileDescriptor() const;
    bool read(Byte* buf, unsigned int &bufLength, unsigned int retry=10);
    bool read(Byte* buf);
    bool write(Byte* buf, unsigned int &bufLength);
    bool write(Byte buf);
    bool isOpened() const;

private:
    struct termios oldtio_;
    int fd_;
    int ttyNbr_;
};

// --------------------------------------------------------------------
// SerialPort::getFileDescriptor
// --------------------------------------------------------------------
inline int SerialPort::getFileDescriptor() const
{
    return  fd_;
}

// --------------------------------------------------------------------
// SerialPort::isOpened
// --------------------------------------------------------------------
inline bool SerialPort::isOpened() const
{
    return  fd_ >= 0;
}
 
// --------------------------------------------------------------------
// SerialPort::read
// --------------------------------------------------------------------
inline bool SerialPort::read(Byte* data)
{
    unsigned int l=1;
    return read(data, l, 10);
}

// --------------------------------------------------------------------
// SerialPort::write
// --------------------------------------------------------------------
inline bool SerialPort::write(Byte data)
{
    unsigned int l=1;
    return write(&data, l);
}

#endif // __SERIAL_PORT_H__

