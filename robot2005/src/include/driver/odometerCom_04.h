#ifndef __ODOMETER_COM_04_H__
#define __ODOMETER_COM_04_H__


static const unsigned char ODOMETER_REQ_PING =     0xAA;
static const unsigned char ODOMETER_ID =           0xB7;
/*
static const unsigned char ODOMETER_GET_POSITION       = 0X03;
static const unsigned char ODOMETER_SET_MODE_AUTOMATIC = 0x02;
static const unsigned char ODOMETER_SET_MODE_MANUAL    = 0x01;
static const unsigned char ODOMETER_GET_MODE           = 0X04;
static const unsigned char ODOMETER_MODE_AUTOMATIC     = 0x02;
static const unsigned char ODOMETER_MODE_MANUAL        = 0x01;
*/

static const unsigned char ODOMETER_GET_POSITION       = 0X83;
static const unsigned char ODOMETER_SET_MODE_AUTOMATIC = 0x82;
static const unsigned char ODOMETER_SET_MODE_MANUAL    = 0x81;
static const unsigned char ODOMETER_GET_MODE           = 0X84;
static const unsigned char ODOMETER_MODE_AUTOMATIC     = 0x82;
static const unsigned char ODOMETER_MODE_MANUAL        = 0x81;

static const unsigned char ODOMETER_BEGIN_AUTO_SEND    = 0x40; // + 5 bits
static const unsigned char ODOMETER_AUTO_SEND_LEFT     = 0x00; // + 6 bits
static const unsigned char ODOMETER_AUTO_SEND_RIGHT    = 0xC0; // + 6 bits
static const unsigned char ODOMETER_END_AUTO_SEND      = 0x60; // + 5 bits


/*
Les octects correspondant a la reponse a une question directe du PC commencent par 2bits: 10, ce qui permet de ne pas les confondre avec les donnees envoyees automatiquement par la carte (01, 00, 11)

Ping:
-----
Le pc envoie [ODOMETER_REQ_PING]
La carte passe en mode manuel puis renvoie son identifiant: [ODOMETER_REQ_PING]
Repasser en mode manuel pendant un ping permet de detecter la carte meme si elle est deja en mode automatique et que le pc ne sait pas combien d'octets il a deja recu. 

Passer en mode manuel:
----------------------
Le pc envoie [ODOMETER_SET_MODE_MANUAL]
La carte finit d'envoyer les donnees automatiques qu'elle envoyait puis elle renvoie [ODOMETER_MODE_MANUAL]

Passer en mode automatique:
---------------------------
Le pc envoie [ODOMETER_SET_MODE_AUTOMATIC]
La carte renvoie [ODOMETER_MODE_AUTOMATIC] puis passe en mode automatique
et envoie periodiquement la position des codeurs

Lire le mode dans lequel se trouve la carte:
--------------------------------------------
Le pc envoie [ODOMETER_GET_MODE]
La carte renvoie [ODOMETER_MODE_AUTOMATIC] ou [ODOMETER_MODE_MANUEL]

Lire la position eds codeurs (en mode manuel seulement):
--------------------------------------------
Le pc envoie [ODOMETER_GET_POSITION]
La carte renvoie 2 trames de 3 octects espaces de 20ms correspondant a la position 
des codeurs.

En mode automatique
-------------------
la carte envoie periodiquement 2 trames de 3 octets espacees de 20ms donnant la position des codeurs
// 20 ms
[ODOMETER_BEGIN_AUTO_SEND+(dataLeft1>>3)]                       //  010 + 5bits de dataLeft1
[ODOMETER_AUTO_SEND_LEFT+((dataLeft1&0x07)<<3)+(dataLeft2>>5)]  //  00  + 3bits de dataLeft1+ 3bits de dataLeft2
[ODOMETER_END_AUTO_SEND+((dataLeft2&0x1F))]                     //  011 + 5bits de dataLeft2

// 20 ms
[ODOMETER_BEGIN_AUTO_SEND+(dataRight1>>3)]                       //  010 + 5bits de dataRight1
[ODOMETER_AUTO_SEND_RIGHT+((dataRight1&0x07)<<3)+(dataRight2>>5)]//  11  + 3bits de dataRight1+ 3bits de dataRight2
[ODOMETER_END_AUTO_SEND+((dataRight2&0x1F))]                     //  011 + 5bits de dataRight2

*/

#endif // __ODOMETER_COM_04_H__
