#ifndef __SERVO_COM_05_H__
#define __SERVO_COM_05_H__

// carte servo moteurs 2005

static const unsigned char SERVO_REQ_PING = 0xAA;
static const unsigned char SERVO_PING_ID  = 0xCB;

// definir la position d'un des 4 servos : SERVO_REQ_SET_POS + (servoId)<<5 + servoPos
// servoId: 2 bits
// servoPos: 5bits
static const unsigned char SERVO_REQ_SET_POS  = 0x00;
// definir la position d'un des 4 servos : SERVO_REQ_ENABLE + (servoEnableMask)
// servoEnableMask: 4 bits, chaque bit represente un servo. si le bit est a 1 on asservi le moteur, si le bit est a 0 on desasservit le servo
static const unsigned char SERVO_REQ_ENABLE  = 0x80;



#endif // __ALIM_COM_05_H__
