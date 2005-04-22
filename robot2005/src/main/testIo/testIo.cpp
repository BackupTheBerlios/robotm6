#define LOG_CLASSID CLASS_DEFAULT
#include "log.h"
#include "io/serialPort.h"
#include "io/ioManager.h"

int main(int argc, char* argv[])
{
    IoManager->submitIoHost(new SerialPort(0));
    IoManager->submitIoHost(new SerialPort(2));
    IoManager->submitIoHost(new SerialPort(3));
    return 0;
}
