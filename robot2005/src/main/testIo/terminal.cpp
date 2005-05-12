#include "io/serialPort.h"
#include <stdio.h>

class OpenCloser {
public:
    OpenCloser(IoDevice* device) {
	device->open();
	device_ = device;
    }
    ~OpenCloser() {
	device_->close();
    }

    IoDevice* device_;
};

void manualIoOut(IoDevice* device) {
    OpenCloser opener(device);
    const unsigned int MAX_BUFFER = 256;
    IoByte buffer[MAX_BUFFER];
    while(true) {
	unsigned int n;
	if (scanf("%x", &n) == EOF) return;
	printf("0x%2.2x", (unsigned char) n);
	IoByte b = (IoByte)n;
	device->write(b);
	unsigned int length = MAX_BUFFER;
	do {
	    if (device->read(buffer, length)) {
		if (length > 0) {
		    for (unsigned int i = 0; i < length; ++i)
			printf("0x%2.2x ", buffer[i]);
		}
	    } else
		break;
	} while (length == MAX_BUFFER);
    }
}

int main(int argc, char* argv[])
{
    IoHost* host = new SerialPort(atoi(argv[1]));
    manualIoOut(host->listPorts()[0]);
}
