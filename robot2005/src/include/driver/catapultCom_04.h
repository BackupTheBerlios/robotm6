#ifndef __CATAPULT_COM_04_H__
#define __CATAPULT_COM_04_H__

static const unsigned char CATAPULT_REQ_PING = 0xAA;
static const unsigned char CATAPULT_PING_ID  = 0xB1;

static const unsigned char CATAPULT_REQ_PREPARE_FOR_DROP       = 0x01;
static const unsigned char CATAPULT_REQ_PREPARE_FOR_TOUCHDOWN  = 0x02;
static const unsigned char CATAPULT_REQ_DROP                   = 0x03;
static const unsigned char CATAPULT_REQ_TOUCHDOWN_ALL          = 0x04;
static const unsigned char CATAPULT_REQ_BALL_NBR               = 0x05;
static const unsigned char CATAPULT_REQ_STATUS                 = 0x06;
static const unsigned char CATAPULT_REQ_AUTOCHECK              = 0x07;
static const unsigned char CATAPULT_REQ_ARM_CATA               = 0xD4;
static const unsigned char CATAPULT_REQ_MODE_2_BALLS           = 0xD2;
static const unsigned char CATAPULT_REQ_MODE_3_BALLS           = 0xD3;
static const unsigned char CATAPULT_REQ_RESET_AFTER_DROP       = 0xD5;
static const unsigned char CATAPULT_REQ_RESET_AFTER_DROP_KEEP_ARMED = 0xD6;
static const unsigned char CATAPULT_REQ_EMERGENCY_STOP         = 0xD0;
static const unsigned char CATAPULT_REQ_RESTART                = 0xD1;
static const unsigned char CATAPULT_REQ_CATA_REVERSE           = 0xD7;

static const unsigned char CATAPULT_BALL_RELEASE_ACK  = 0x08;
static const unsigned char CATAPULT_BALL_AT_TOP_ACK   = 0x09;
static const unsigned char CATAPULT_BALL_EATEN_ACK    = 0x0A;
static const unsigned char CATAPULT_AUTOCHECK_ACK     = 0x0B;

static const unsigned char CATAPULT_MASK_BALL_EATEN   = 0x10;
static const unsigned char CATAPULT_MASK_BALL_AT_TOP  = 0x20;
static const unsigned char CATAPULT_MASK_RELEASE      = 0x30;
static const unsigned char CATAPULT_MASK_BALL_NBR     = 0xC0;

static const unsigned char CATAPULT_STATUS_WAITING    = 0x80;
static const unsigned char CATAPULT_STATUS_EATING     = 0x81;
static const unsigned char CATAPULT_STATUS_REVERSING  = 0x82;
static const unsigned char CATAPULT_STATUS_DROPING    = 0x83;
static const unsigned char CATAPULT_STATUS_TOUCHDOWN  = 0x84;
/* ---- Rajouter d'autres etats si necessaire ------ */

static const unsigned char CATAPULT_AUTOCHECK_OK1         = 0xE0;
static const unsigned char CATAPULT_AUTOCHECK_LASER_TOP   = 0x01;
static const unsigned char CATAPULT_AUTOCHECK_LASER_BOTTOM= 0x02;
static const unsigned char CATAPULT_AUTOCHECK_BUMP_ENTRY  = 0x04;
static const unsigned char CATAPULT_AUTOCHECK_BUMP_BOTTOM = 0x08;
static const unsigned char CATAPULT_AUTOCHECK_OK2       = 0xF0;
static const unsigned char CATAPULT_AUTOCHECK_BUMP_TOP  = 0x01;
static const unsigned char CATAPULT_AUTOCHECK_BUMP_CATA = 0x02;

static const unsigned char CATAPULT_ENTRY_OPENED      = 0x40;
static const unsigned char CATAPULT_ENTRY_CLOSED      = 0x41;

static const unsigned char CATAPULT_FOUR_BALLS_ERROR  = 0x42;
static const unsigned char CATAPULT_LASER_CUT         = 0x43;

/*
Ping:
-----
Le PC envoie CATAPULT_REQ_PING
La carte repond CATAPULT_PING_ID

PREPARE_FOR_DROP:
-----------------
Le PC envoie CATAPULT_REQ_PREPARE_FOR_DROP
La carte met une balle dans la catapulte et attend l'ordre de tir. Si la carte recoit ensuite PREPARE_FOR_TOUCHDOWN ou TOUCHDOWN, elle catapulte la balle et passe dans le mode demande (ce cas ne devrait pas arriver).

PREPARE_FOR_TOUCHDOWN:
----------------------
Le PC envoie CATAPULT_REQ_PREPARE_FOR_TOUCHDOWN
La carte fait monter une balle jusqu'au capteur du haut

DROP:
-----
Le PC envoie CATAPULT_REQ_DROP
La carte met une balle dans la catapulte si ce n'est pas deja fait et la tire.

TOUCHDOWN:
----------
Le PC envoie CATAPULT_REQ_TOUCHDOWN_ALL
La carte fait sortir toutes les balles par l'arriere du robot.

REQ_BALL_NBR:
-------------
Le PC envoie CATAPULT_REQ_BALL_NBR
La carte renvoie le nombre de balles contenues dans le robot: 
   un octet= CATAPULT_MASK_BALL_NBR | nombre de balles

REQ_STATUS:
-----------
Le PC envoie CATAPULT_REQ_STATUS
La carte renvoie un octect correpondant a l'etat dans lequel elle est. Voir CATAPULT_STATUS_*

REQ_AUTOCHECK:
--------------
Avant le debut du match, le PC envoie la requete CATAPULT_REQ_AUTOCHECK
L'ascenseur est vide (nombre de balles =0), la carte verifie que tout le systeme mecanique fonctionne, que tous les capteurs fonctionnent. On pourra avaler une balle pour l'autoverification.
La carte renvoie l'etat de la porte: CATAPULT_ENTRY_OPENED / CATAPULT_ENTRY_CLOSED
La carte renvoie CATAPULT_AUTOCHECK_ERROR si elle detecte un probleme.
La carte renvoie CATAPULT_AUTOCHECK_OK en fin de verification quand tout va bien.
Le PC repond CATAPULT_AUTOCHECK_ACK. Si le PC ne repond pas dans les 100 ms la carte renvoie CATAPULT_AUTOCHECK_ERROR ou CATAPULT_AUTOCHECK_OK et attend le ACK du PC.

Evenements:
===========

Porte:
------
Quand l'entree du robot change d'etat, un octet est envoye: CATAPULT_ENTRY_OPENED / CATAPULT_ENTRY_CLOSED

Balle avalee:
-------------
Quand une balle est avalee, la carte envoie un octet pour signaler le nombre de balles que le robot contient: CATAPULT_MASK_BALL_EATEN | nombre de balles
Le PC repond CATAPULT_BALL_EATEN_ACK. Si le PC ne repond pas dans les 100 ms la carte renvoie sa donnee et attend le ACK du PC.

Balle au sommet du robot:
-------------------------
Quand une balle arrive au sommet du robot, la carte envoie un octet pour signaler le nombre de balles que le robot contient: CATAPULT_MASK_BALL_AT_TOP | nombre de balles
Le PC repond CATAPULT_BALL_AT_TOP_ACK. Si le PC ne repond pas dans les 100 ms la carte renvoie sa donnee et attend le ACK du PC.

Balle relachee:
---------------
Quand une balle est catapultee ou lachee a l'arriere du robot, la carte envoie un octet pour signaler le nombre de balles que le robot contient: CATAPULT_MASK_RELEASE | nombre de balles
Le PC repond CATAPULT_BALL_RELEASE_ACK. Si le PC ne repond pas dans les 100 ms la carte renvoie sa donnee et attend le ACK du PC.

4 balles:
---------
Quand la carte detecte 4 balles dans le robot, elle envoie un octet: CATAPULT_FOUR_BALLS_ERROR et libere une balle.

*/

#endif // __CATAPULT_COM_04_H__
