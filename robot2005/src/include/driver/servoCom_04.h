#ifndef __SERVO_COM_04_H__
#define __SERVO_COM_04_H__


static const unsigned char SERVO_REQ_PING          = 0xAA;
static const unsigned char SERVO_UART_ID           = 0xB4;

static const unsigned char SERVO_MIN_POSITION      = 0;
static const unsigned char SERVO_MAX_POSITION      = 42;

static const unsigned char SERVO_REQ_GET_POSITION  = 0x80;
static const unsigned char SERVO_REQ_SET_CONSIGN   = 0x90;
static const unsigned char SERVO_REQ_GET_CONSIGN   = 0xA0;
static const unsigned char SERVO_REQ_DISABLE_ALL   = 0xB0;
static const unsigned char SERVO_REQ_DISABLE       = 0xC0;

/*
== Protocole de communication avec la carte servo 2004 ==
[] = 1 octet

Au reset tous les servo de la carte sont desasservis

Ping:
-----
Le pc envoie : [SERVO_REQ_PING]
La carte renvoie son identifiant [SERVO_UART_ID]

Connaitre la position des servos:
--------------------------------
Le pc envoie : [SERVO_REQ_GET_POSITION + numero du servo]
La carte renvoie un octect correspondant a la position (tension) du servo: [position]

Connaitre la consigne des servos:
--------------------------------
Le pc envoie : [SERVO_REQ_GET_CONSIGN + numero du servo]
La carte renvoie un octect correspondant a la consign du servo: [position]


Mettre a jour la position d'un servo:
------------------------------------
Le pc envoie : [SERVO_REQ_SET_CONSIGN + numero du servo][position demandee]
La carte renvoie un octect correspondant a la position demandee: [position demandee]
le servo passe en mode asservi et il est envoye a la position demandee

Desactiver un servo:
------------------------------------
Le pc envoie : [SERVO_REQ_DISABLE + numero du servo]
La carte renvoie un octect correspondant a la requete : [SERVO_REQ_DISABLE + numero du servo]
le servo est desasservi

Desactiver tous les servos:
------------------------------------
Le pc envoie : [SERVO_REQ_DISABLE_ALL]
La carte renvoie un octect correspondant a la requete : [SERVO_REQ_DISABLE_ALL]
Tous les servos sont desasservis.

*/

#endif // SERVO_COM_04_H__
