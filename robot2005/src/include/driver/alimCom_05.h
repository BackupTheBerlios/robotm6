#ifndef __ALIM_COM_05_H__
#define __ALIM_COM_05_H__

// carte alimentation 2005

static const unsigned char ALIM_REQ_PING = 0xAA;
static const unsigned char ALIM_PING_ID  = 0xCD;

// demande la tension des 4 batteries : renvoie 4 octects (1 octect par batterie)
static const unsigned char ALIM_REQ_GET_TENSION  = 0xA0;
static const unsigned char ALIM_TENSION_NBR  = 4;

// Le PC envoie 1 octet, la carte passe en mode tension 12V (defaut) et renvoie 01=OK, 02=ERROR
static const unsigned char ALIM_REQ_MODE_12V  = 0xA1;
// Le PC envoie 1 octet, la carte passe en mode tension 28V et renvoie 01=OK, 02=ERROR
static const unsigned char ALIM_REQ_MODE_28V  = 0xA2;
// Le PC envoie 1 octet, la cartepasse en mode tension 42V et renvoie 01=OK, 02=ERROR
static const unsigned char ALIM_REQ_MODE_42V  = 0xA3;

// Le PC envoie 1 octet, la cartedisable 12V (1) et renvoie 01=OK, 02=ERROR
static const unsigned char ALIM_REQ_DISABLE_12V_1  = 0xA4;
// Le PC envoie 1 octet, la carteenable 12V (1) envoie 01=OK, 02=ERROR
static const unsigned char ALIM_REQ_ENABLE_12V_1  = 0xA5;
// Le PC envoie 1 octet, la cartedisable 12V (2) envoie 01=OK, 02=ERROR
static const unsigned char ALIM_REQ_DISABLE_12V_2  = 0xA6;
// Le PC envoie 1 octet, la carteenable 12V (2) et renvoie 01=OK, 02=ERROR
static const unsigned char ALIM_REQ_ENABLE_12V_2  = 0xA7;

static const unsigned char ALIM_ACK = 0x01;
static const unsigned char ALIM_NACK = 0x02;

#endif // __ALIM_COM_05_H__
