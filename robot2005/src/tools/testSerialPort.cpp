
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <memory.h>
#include <stdio.h>

struct termios oldtio_;
int fd=-1;
typedef unsigned char Byte;
#define DEV "/dev/ttyS1"
// http://ftp.man.olsztyn.pl/pub/linux/distributions/slackware/slackware-3.5/docs/Serial-Programming-HOWTO
#define BAUDRATE B9600
// --------------------------------------------------------------------
// SerialPort::open
// --------------------------------------------------------------------
bool myopen()
{

  fd = open(DEV, O_RDWR | O_NOCTTY /*| O_NONBLOCK*/);
    if (fd <0) { 
        printf("Cannot open serial port %s\n", DEV);
        return false;
    }

    if( tcgetattr( fd, &oldtio_ ) < 0 ) 
        goto fail;/* save current port settings */
    struct termios newtio;
    memset(&newtio, 0, sizeof(newtio));
    newtio.c_iflag = INPCK; 
    newtio.c_lflag = 0;//ICANON;
    newtio.c_oflag = 0;
    newtio.c_cflag = BAUDRATE | CREAD | CS8 | CLOCAL;
    newtio.c_cc[ VMIN ] = 0;  // min char to read in non canonical mode
    newtio.c_cc[ VTIME ] = 1; // timout to read in non canoncal    
    //if( cfsetispeed( &newtio, BAUDRATE ) < 0 ) goto fail; 
    //if( cfsetospeed( &newtio, BAUDRATE ) < 0 ) goto fail; 
    // start the communication
    tcflush(fd, TCIFLUSH);
    if( tcsetattr( fd, TCSANOW, &newtio ) < 0 ) goto fail;
    
#ifdef linux
    // not used because we use poll instead
    // fcntl( fd, F_SETOWN, getpid() );
    //  fcntl( fd, F_SETFL, FASYNC );
#endif
    printf("Serial port opened %d \n", fd);
    return true;
 fail:
    return false;
}

// --------------------------------------------------------------------
// close
// --------------------------------------------------------------------
bool myclose()
{
    if (fd >= 0) {
        tcsetattr(fd, TCSANOW, &oldtio_);
        close(fd);
        fd = -1;
        printf("SerialPort %s closed\n", DEV);
    }
    return true;
}

// --------------------------------------------------------------------
// myread
// --------------------------------------------------------------------
bool myread(Byte* buf, int length, int retry=10)
{
    printf("read %d\n", length);
    int k=0, res=0, N=length;
    while(k++<retry && N > 0) {
       res = read(fd, buf, N);
       printf("%d res=%d 0x%02x\n", fd, res, buf[0]);
       if (res == 0) {
           continue;
       } else if (res<0) {
           printf("read error %d\n", res);
           return false;
       } else {
           N-=res;
       }
    }
    if (N>0) {
        printf("Read error Not enought data on serial port (read %d/%d)\n", 
               length-N, length);
    }
    return (N == 0);
}

// --------------------------------------------------------------------
// SerialPort::write
// --------------------------------------------------------------------
bool mywrite(Byte* buf, unsigned int& length)
{
    int length2 = 0;
    do {
      printf("write %d %d\n", fd, length);
        length2 = write(fd, buf, length);
	if (length2 <= 0) return false;
	length -= length2;
    } while (length > 0);
    return true;
}

// --------------------------------------------------------------------
// main
// --------------------------------------------------------------------
int main(int argc, char**argv){
    
    if (!myopen()) {
        printf("Cannot open port %s\n", DEV);
        return -1;
    }
    Byte buf[10];
    unsigned int l=0;

    // write
    l=1;
    buf[0]=0xAA;
    if (!mywrite(buf, l)) {
        printf("Cannot write on port\n");
    }

    // read
    l=1;
    if (!myread(buf, l, 10)) {
        printf("Cannot read on port\n");
    } else {
        printf("read:0x%02x\n", buf[0]);
    }

    myclose();
    return 0;
}
