#ifndef __SERVO_COM_05_H__
#define __SERVO_COM_05_H__

// carte servo moteurs 2005

static const unsigned char SERVO_REQ_PING = 0xAA;
static const unsigned char SERVO_PING_ID  = 0xCB;

// definir la position d'un des 4 servos : SERVO_REQ_SET_POS + (servoId)
// puis envoyer la position entre 0xB0 et 0xF1
// la carte ne repond rien
static const unsigned char SERVO_REQ_SET_POS  = 0x90;
static const unsigned char SERVO_POS_MIN  = 0x09;
static const unsigned char SERVO_POS_MAX  = 0x48;

static const unsigned char SERVO_ID_MIN  = 0;
static const unsigned char SERVO_ID_MAX  = 3;

// desasservir un servo : SERVO_REQ_DISABLE + servoId
// la carte ne repond rien
static const unsigned char SERVO_REQ_DISABLE  = 0xC0;
// desaservir tous les servo
// la carte ne repond rien
static const unsigned char SERVO_REQ_DISABLE_ALL  = 0xB0;

#endif 
