#ifndef __GONIO_COM_04_H__
#define __GONIO_COM_04_H__

static const unsigned char GONIO_REQ_PING = 0xAA;
static const unsigned char GONIO_PING_ID  = 0xB5;

// bit de poids fort == 0 => requete esclave 
static const unsigned char GONIO_REQ_CALIBRATE       = 0x2F;
static const unsigned char GONIO_REQ_SET_SPEED       = 0xC0;
static const unsigned char GONIO_REQ_MODE_OFF        = 0x81;
static const unsigned char GONIO_REQ_MODE_BALAYAGE   = 0x82;
static const unsigned char GONIO_REQ_MODE_ASSER      = 0x90;
static const unsigned char GONIO_REQ_GET_MODE        = 0x84;
static const unsigned char GONIO_REQ_DIRECTION       = 0x40;

static const unsigned char GONIO_BALISE_ID_GOAL_LEFT = 1;
static const unsigned char GONIO_BALISE_ID_GOAL_RIGHT= 0;
static const unsigned char GONIO_BALISE_ID_RP        = 3;
static const unsigned char GONIO_BALISE_ID_RS        = 2;
static const unsigned char GONIO_BALISE_ID_GOAL      = 4;

static const unsigned char GONIO_SPEED_MAX           = 0x05;


/*
Ping:
-----
Le PC envoie GONIO_REQ_PING.
La carte passe en mode OFF, elle arrete d'envoyer des donnees automatiquement puis repond GONIO_PING_ID.

Set mode OFF:
-------------
Le PC envoie GONIO_REQ_MODE_OFF.
La carte arrete le moteur du gonio et arrete d'envoyer des donnees automatiquement puis elle renvoie GONIO_REQ_MODE_OFF

Set mode Balayage:
------------------
Le PC envoie GONIO_REQ_MODE_BALAYAGE.
La carte passe en mode balayage et renvoie GONIO_REQ_MODE_BALAYAGE. En mode balayage la moteur tourne en continu et la carte renvoie la position des balises des quelle les voit (sur front montant: passage de je ne vois rien a je vois la balise i).

Set mode asservissement:
------------------------
Le PC envoie un octet: GONIO_REQ_MODE_ASSER + numero de balise.
La carte renvoie un octect GONIO_REQ_MODE_ASSER + numero de balise.
Puis elle passe en mode asservissement: le moteur tourne en continu jusq'a trouver la balise. Quand la balise est trouve la carte asservi le moteur sur sa position.
En mode asservissement, la carte envoie la position de la balise toutes les 100 ms. Quand la balise est perdue, elle n'envoie plus de donnees.

Calibrage:
----------
Le PC envoie GONIO_REQ_CALIBRATE.
La carte stoppe l'envoie de donnees automatiques, fait tourner le moteur jusq'a la position 0 puis renvoie GONIO_REQ_CALIBRATE.

Get mode:
----------
Le PC envoie GONIO_GET_MODE.
La carte renvoie GONIO_REQ_MODE_OFF ou GONIO_REQ_MODE_OFF ou GONIO_REQ_MODE_ASSER + numero de balise



Octects envoyes pour donner la position d'une balise:
-----------------------------------------------------
- angle = la direction de la balise par rapport au zero du gonio est codee sur 9 bits. L'angle doit etre different de 0x1FF. unsigned short angle. angle =[0..0x1FE]
- id: numero de la balise: id=[0..3]. Pour les buts (quand le gonio oscille entre les 2 balises, id = 4.

octet 1:
|0.0.id2.id1.id0.angle8.angle7.angle0|
octect 2:
|0.1.angle5.angle4.angle3.angle2.angle1.angle0|

= 
putchar((id << 3) + (angle >> 6))
putchar(0x40 + (angle & 0x3F))

*/
#endif // __GONIO_COM_04_H__
