/**
 * @file daq_module.h
 *
 * This Module manages the dridaq board (inputs/outputs) 
 * doc: pc104 data aquisition board: daq1272
 *      one chip: Harris Semiconductor, 85C55A CMOS Programmable Peropheral Interface
 *
 * @author Alexis Nikichine
 * @author Laurent Saint-Marcel
 * @date   Sat Oct 25 03:31:45 2003
 */

#ifndef __DAQ_MODULE_H__
#define __DAQ_MODULE_H__

#define DAQ_CHIP_NBR      4 // number of chip on the board
#define DAQ_PORT_PER_CHIP 3
/*****************************************************************************/
/* USER SETTINGS                                                             */
/*****************************************************************************/

/** Set DAQ_DEBUG to 1 to have additionnal kernel traces */
#define DAQ_DEBUG 1 

/** Address of the dridaq board */
#define DAQ_BASE_ADDRESS 0x280 

#define DAQ_MODE_BYTE_OUTPUT 0x80   // chip writing mode (write it on CTRL byte)
#define DAQ_MODE_BYTE_INPUT  0x9B   // chip reading mode (write it on CTRL byte)

#define DAQ_CHIP_1_PORTA DAQ_BASE_ADDRESS       // Address of the first data of chip1
#define DAQ_CHIP_1_CTRL  DAQ_BASE_ADDRESS+3     // Address of the control byte of chip1
#define DAQ_BYTE_PER_CHIP 4 // Number of bytes used by a chip
                            // DAQ_CHIP_2_PORTA = DAQ_CHIP_1_PORTA + BYTE_PER_CHIP
                            // DAQ_CHIP_2_CTRL  = DAQ_CHIP_1_CTRL  + BYTE_PER_CHIP

/** 
 * Chips direction cannot be changed dynamically because it would change the 
 * number of bytes sent to the fifo when one input state changes
 * 0=output, 1=input 
 */
static const unsigned char DAQ_CHIP_DIR[DAQ_CHIP_NBR] = {1, 1, 0, 0}; 

/** 
 * Filter used to avoid seeing modifications of input pins which have 
 * no pull up and may have a random value
 */
static const unsigned char DAQ_INPUT_FILTER[DAQ_CHIP_NBR][DAQ_PORT_PER_CHIP] = {
    {0xff, 0xff, 0xff},  // all pins are filter because there is no pullup chip 1
    {0xff, 0xff, 0xff}, // chip 2
    {0xff, 0xff, 0xff}, // chip 3
    {0xff, 0xff, 0xff}, // chip 4
}; 

#endif // __DAQ_MODULE_H__
