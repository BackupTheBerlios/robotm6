#ifndef __SERVO_COM_03_H__
#define __SERVO_COM_03_H__


static const unsigned char SERVO_REQ_PING          = 0xAA;
static const unsigned char SERVO_UART_ID           = 0xA4;

static const unsigned char SERVO_MIN_POSITION      = 0;
static const unsigned char SERVO_MAX_POSITION      = 28;
static const unsigned char SERVO_REQ_GET_POSITION  = 29;
static const unsigned char SERVO_REQ_DISABLE       = 31;
static const unsigned char SERVO_REQ_GET_STATE     = 30;

#endif // SERVO_COM_03_H__
