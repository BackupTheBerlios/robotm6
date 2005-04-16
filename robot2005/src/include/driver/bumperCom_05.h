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
// demande de renvoyer les 3 octects de donnees + 4 bits de start 
// + 4 bits de checksum
// 0xA0+(data[0]&0x0F) : start + 4bits de poids faible de data[0]
// data[1]
// data[2]
// (data[0]&0xF0) + 4 bits de checksum : 
//          checksum = data[0]^data[1]^data[2]  (^=xor)
//          4 bits de checksum= (checksum&0x0F)^((checksum&0xF0)>>4)
static const unsigned char BUMPER_REQ_GET_ALL    = 0x07;

// nombre d'octet maximal
static const unsigned int  BUMPER_DATA_NBR  = 3;
static const unsigned char  BUMPER_START_BITS  = 0xA0;

#endif // __BUMPER_COM_05_H__
