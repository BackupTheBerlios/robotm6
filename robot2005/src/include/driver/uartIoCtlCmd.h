/**
 * 
 * UART IOCTL COMMANDS
 */

#ifndef __UART_IOCTL_COMMANDS_H__
#define __UART_IOCTL_COMMANDS_H__

/**
 * 
 */
#define SET_READ_TIMEOUT    3
// arg is the time in ms
// always return 0



// not implemented
// definir la vitesse de communication
#define SET_SPEED      5
// arg are :
#define SPEED_50         0
#define SPEED_75         1
#define SPEED_110        2
#define SPEED_134_5      4
#define SPEED_200        6
#define SPEED_150        7
#define SPEED_300        8
#define SPEED_600       10
#define SPEED_1200      12
#define SPEED_1050      14
#define SPEED_2000      15
#define SPEED_2400      16
#define SPEED_4800      18
#define SPEED_7200      20
#define SPEED_9600      22
#define SPEED_38400     24
#define SPEED_19200     25
// return 
#define OK		0
// or
#define INCOMPATIBLE_SPEED 1


//odd even none
#define SET_PARITY     6
// arg are
#define PARITY_ODD     0
#define PARITY_EVEN    1
#define PARITY_NONE    2
// always return 0

// multiple de 9/16
#define SET_STOP_LENGTH 7
// not yet implemented


//on off
#define SET_Rx_STATE    8
// arg are :
#define UART_Rx_OFF  (1 << 1)
#define UART_Rx_ON   1
// always return 0

//on off
#define SET_Tx_STATE    9
// arg are :
#define UART_Tx_OFF  (1 << 3)
#define UART_Tx_ON  (1 << 2)
// always return 0


//none
#define SET_INT_ON_Rx   10

//none
#define SET_INT_ON_TIMEOUT 11

// Rx Tx
#define SET_LED_STATE      12


// return UART_Status Register
#define GET_STATUS_REGISTER 13
// return the status register
// it should be decoded using cmd.h
// 0 means nothing special


// Set Rx ON and Tx on
#define SET_CHANNEL_STATUS     14
// arg are : 
#define UART_CHANNEL_ON     UART_Rx_ON | UART_Tx_ON
#define UART_CHANNEL_OFF    UART_Rx_OFF | UART_Tx_OFF
// always return 0

// to dump uart state to dmesg
#define DUMP_STATE           15
// always return 0


// return the state of the UART
#define GET_STATE             16
// returned values :
#define NO_ERROR            0
#define TIMEOUT             1
#define COMMUNICATION_ERROR 2

// reset error counters
#define RESET_ERROR_COUNTERS 17
// return the number of overrun errors since last reset
#define GET_OVERRUN_ERROR    18
// return the number of receveid break errors since last reset
#define GET_BREAK_ERROR      19
// return the number of framing errors since last reset
#define GET_FRAMING_ERROR    20
// return the number of parity errors since last reset
#define GET_PARITY_ERROR     21
// clear the interrupt flag
#define CLEAR_BUFFER_AND_INTERRUPT  22
// return if an interrupt did not correspond to a fifo data
#define GET_INTERRUPT_ERROR_PENDING 23
// return the number of times the interrupt did not correspond to fifo data
#define GET_INTERRUPT_ERRORS_COUNT  24
// display all isr and sr registers
#define DUMP_ALL_STATUS_REGISTERS   25


/********** les commandes bas-niveau **************/

// ecrire une valeur a une adresse sur l'UART
// lire a une adresse sur l'UART

// HardWareWrite
#define HWWR		0
// arg is : dataaddress
// first eight bit are data and last eight bit are adress
// always returns 0


// HardWareRead
#define HWRD		1
// arg is address
// returns the value at this address

#endif // __UART_IOCTL_COMMANDS_H__
