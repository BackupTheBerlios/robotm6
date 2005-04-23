#define LOG_CLASSID CLASS_DEFAULT
//#include "log.h"
#include "io/serialPort.h"
#include "io/ioManager.h"

void printLoop(IoDevice* device) {
    device->open();
    device->write(0xAA);
    unsigned char c;
    while (true) {
	device->read(&c);
	printf("0x%2.2x ", c);
    }
    device->close();
}

int main(int argc, char* argv[])
{
    if (argc == 1) {
	IoManager->submitIoHost(new SerialPort(0));
	IoManager->submitIoHost(new SerialPort(2));
	IoManager->submitIoHost(new SerialPort(3));
	return 0;
    } else if (argc == 2) {
	IoManager->submitIoHost(new SerialPort(atoi(argv[1])));
	return 0;
    } else if (argc == 3) {
	IoHost* host = new SerialPort(atoi(argv[1]), SERIAL_SPEED_38400);
	IoDevice* device = host->listPorts()[0];
	printLoop(device);
	return 0;
    }
}
