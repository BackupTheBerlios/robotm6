#ifndef __BUMPER_COM_05_H__
#define __BUMPER_COM_05_H__

// carte input/bumper 2005

static const unsigned char BUMPER_REQ_PING = 0xAA;
static const unsigned char BUMPER_PING_ID  = 0xC1;

// demande de renvoyer le 1er octet
static const unsigned char BUMPER_REQ_GET_DATA1  = 0x01;
// demande de renvoyer le 2eme octet
static const unsigned char BUMPER_REQ_GET_DATA2  = 0x02;
// demande de renvoyer le 3eme octet
static const unsigned char BUMPER_REQ_GET_DATA3  = 0x04;
// demande de renvoyer les 3 octects de donnees
static const unsigned char BUMPER_REQ_GET_ALL    = 0x07;

// nombre d'octet maximal
static const unsigned int  BUMPER_DATA_NBR  = 3;

#endif // __BUMPER_COM_05_H__
