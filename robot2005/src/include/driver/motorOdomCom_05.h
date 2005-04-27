#ifndef __MOTOR_ODOM_COM_05_H__
#define __MOTOR_ODOM_COM_05_H__

// carte motor/odometre 2005

static const unsigned char MOTOR_ODOM_REQ_PING = 0xAA;
static const unsigned char MOTOR_ODOM_PING_ID  = 0xC0;

// mettre a jour l'acceleration des moteurs des roues :
//  MOTOR_ODOM_REQ_SET_ACCELERATION + 1 octet. Valeur par defaut: 0x10
static const unsigned char MOTOR_ODOM_REQ_SET_ACCELERATION  = 0x06;

// mettre a jour la vitesse des roues : MOTOR_ODOM_REQ_SET_SPEED + 2 octets
static const unsigned char MOTOR_ODOM_REQ_SET_SPEED  = 0x07;

// mettre a jour la vitesse des roues et reccuperer la position de codeurs : 
// envoyer MOTOR_ODOM_REQ_SET_SPEED_AND_GET_POS + 2 octets
// on recoit 12 octets:
// 2 codeur odometre Gauche
// 2 codeur odometre Droit
// 3 codeur hctl gauche
// 3 coderu hctl droit
// 1 pwm hctl gauche
// 1 pwm hctl droit
static const unsigned char MOTOR_ODOM_REQ_SET_SPEED_AND_GET_POS  = 0x0A;

// reccuper la position des 2 pwm des moteurs des roues:
// envoyer MOTOR_ODOM_GET_PWM
// on reccupere 2 octets:
// 1 pwm hctl gauche
// 1 pwm hctl droit
static const unsigned char MOTOR_ODOM_GET_PWM  = 0x0B;

// reccuper la position des 2 codeurs odometres:
// envoyer MOTOR_ODOM_REQ_GET_HCTL_CODER
// on reccupere 6 octets:
// 3 codeur hctl gauche
// 3 coderu hctl droit
static const unsigned char MOTOR_ODOM_REQ_GET_HCTL_CODER  = 0x0C;

// reccuper la position des 2 codeurs odometres:
// envoyer MOTOR_ODOM_REQ_GET_ODOM_CODER
// on reccupere 4 octets:
// 2 codeur odometre Gauche
// 2 codeur odometre Droit
static const unsigned char MOTOR_ODOM_REQ_GET_ODOM_CODER  = 0x0D;

// reseter les 2 hctl
static const unsigned char MOTOR_ODOM_REQ_RESET  = 0x0E;

// desaservir les 2 hctl
static const unsigned char MOTOR_ODOM_REQ_IDLE  = 0x12;
// desasservir le moteur gauche
static const unsigned char MOTOR_ODOM_REQ_IDLE_LEFT  = 0x14;
// desasservir le moteur droit
static const unsigned char MOTOR_ODOM_REQ_IDLE_RIGHT  = 0x15;
// asservir les 2 hctl
static const unsigned char MOTOR_ODOM_REQ_UNIDLE  = 0x13;

// l'envoie de donnees sur plusieurs octets commence toujours par l'octet de poids fort
#endif 
