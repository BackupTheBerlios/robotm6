#ifndef __ENV_DETECTOR_COM_04_H__
#define __ENV_DETECTOR_COM_04_H__

static const unsigned char ENV_DETECTOR_REQ_PING  = 0xAA;
static const unsigned char ENV_DETECTOR_BASE_ID   = 0xB8;

static const unsigned char ENV_REC_MODE_AUTOMATIC = 0x81;
static const unsigned char ENV_REC_MODE_MANUAL    = 0x82;
static const unsigned char ENV_REC_GET_ALL_DIST   = 0x84;
static const unsigned char ENV_REC_GET_VALUE      = 0xC0;
static const unsigned char ENV_REQ_GET_MODE       = 0x83;
static const unsigned char ENV_REQ_CALIBRATE      = 0x85;
static const unsigned char ENV_MESSAGE_MASK       = 0x80;


/* Protocole de communication
Ping :
------
Le pc envoie ENV_DETECTOR_REQ_PING, 
la carte passe en mode manuel,
Quand la carte n'envoie plus de donnees, elle repond ENV_DETECTOR_BASE_ID ou ENV_DETECTOR_BASE_ID+1

Passage en mode automatique:
----------------------------
Le pc envoie ENV_REC_MODE_AUTOMATIC
La carte renvoie la distance mesuree par tous les capteurs puis ENV_REC_MODE_AUTOMATIC

La distance d'un capteur est un octect:
---------------------------------------
bit de poids fort=0 +1 1 bit pour definir le type de capteur: long(0) ou court(1) + 4 bits pour definir le numero du capteur + 2 bits pour la distance(0=rien, 3=tres pres)
Par exemple pour le capteur court numero 2 qui mesure une distance egale a 1, la carte envoie: 0 1 0010 01 = 0x49

En mode automatique, quand la distance mesuree par un capteur change, la carte envoie la nouvelle valeur (cf octet ci dessus)

Passage en mode manuel
----------------------
Le pc envoie ENV_REC_MODE_MANUAL
La carte stoppe l'envoie de nouvelles donnees puis elle renvoie ENV_REC_MODE_MANUAL

Lire toutes les distances
-------------------------
Le pc envoie ENV_REC_GET_ALL_DIST
La carte renvoie la distance mesuree par tous les capteurs puis ENV_REC_GET_ALL_DIST

Lire la valeur physique d'un capteur  (utile??)
-----------------------------------------------
Le pc envoie ENV_REC_GET_VALUE | numero du capeur
La carte renvoie la valeur reelle du capteurs sur 7 bits | 0x80
Par exemple si la valeur vaut 0x1A, la carte retourne 0x80 | 0x1A = 0x9A

Lire le mode de la carte
------------------------
Le pc envoie ENV_REQ_GET_MODE
La carte renvoie ENV_REC_MODE_AUTOMATIC ou ENV_REC_MODE_MANUAL

Calibration des capteurs
------------------------
Le pc envoie ENV_REQ_CALIBRATE
La carte calibre tous ses capteurs puis renvoie ENV_REQ_CALIBRATE

*/
#endif // __ENV_DETECTOR_COM_04_H__
