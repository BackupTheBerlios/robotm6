/**
 * @file daq_moduleFifo.h
 *
 * This Module manages the dridaq board (inputs/outputs)
 *
 * @author Alexis Nikichine
 * @author Laurent Saint-Marcel
 * @date   Sat Oct 25 03:31:45 2003
 */

#ifndef __DAQ_MODULE_FIFO_H__
#define __DAQ_MODULE_FIFO_H__

#include "daq_module.h"

/** Stack size of the module rt task */
#define DAQ_TASK_STACK_SIZE           4096

/** 
 * Fifo containing the inputs data. 
 * Inputs data will be accessible by reading /dev/rtfX (where X is
 * DAQ_READ_FIFO => /dev/rtf0) 
 * In the linux application, use :
 *   int fd=::open("/dev/rtf0", O_RDONLY | O_NONBLOCK);
 *   read(fd, buf, 3*inputsChipNbr);
 *   close(fd)
 * As soon as a pin value changes, the fifo is filled with all inputs 
 * ports values.
 * If 2 chips are inputs, the fifo is filled with 2 x 3 = 6 bytes
 */
#define DAQ_READ_FIFO                    0 

/** Size of the read/write fifos */
#define DAQ_FIFO_SIZE                   12

/** Minimum delay between 2 updates of the read fifo */
#define DAQ_DELAY                 50000000 


/** 
 * Fifo containing the outputs data
 * Outputs pins can be set by writing in /dev/rtfX (where X is
 * DAQ_WRITE_FIFO => /dev/rtf1) 
 *
 * In the linux application, use :
 *   int fd=::open("/dev/rtf1", O_WRONLY | O_NONBLOCK);
 *   unsigned char buf[DAQ_WRITE_PACKET_SIZE];
 *   daq_convertDataToBuf(0,1,3,1); // chip:0, port:1, bit:3, 5V 
 *   int res=write(fd, buf, DAQ_WRITE_PACKET_SIZE);
 *   close(fd);
 *
 * On success the write command returns 0. It may also return 
 * DAQ_INVALID_PACKET_FORMAT or DAQ_INVALID_CHIP_DIR
 */
#define DAQ_WRITE_FIFO                   1 

/** Number of byte to send to the module to set an output value */
#define DAQ_WRITE_PACKET_SIZE            2

/** Error when chip/port/bit is not valid during the write command */
#define DAQ_INVALID_PACKET_FORMAT -50

/** Error when trying to write on an input chip during the write command */
#define DAQ_INVALID_CHIP_DIR      -51

/** 
 * Macro to convert a pin address and value into a buffer that can be written
 * on /dev/rtfX.
 * chip  [0..3]
 * port  [0..2]
 * bit   [0..7]
 * value [0..1]
 * If bit = 8, then value [0..255] is the value for the 8 pins of the port
 */
#define daq_convertDataToBuf(chip, port, bit, value, buf) \
  do { \
    buf[0] = (chip << 6) + ((port << 4) & 0x30) + (bit & 0x0f); \
    buf[1] = value; \
  } while(0);


#endif // __DAQ_MODULE_FIFO_H__
