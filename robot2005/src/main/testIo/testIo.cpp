#define LOG_CLASSID CLASS_DEFAULT
#include "log.h"
#include "io/serialPort.h"
#include "io/ioManager.h"

int main(int argc, char* argv[])
{
  IoManager->submitIoHost(new SerialPort(1, true));
  IoDevice* device = IoManager->getIoDevice(IO_ID_ODOMETER_04);
  if (device != NULL)
    {
      LOG_OK("non-null device! :)\n");
    }
  else
    {
      LOG_ERROR("didn't find Odometer :(\n");
    }
  return 0;
}
