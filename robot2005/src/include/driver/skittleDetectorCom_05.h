#ifndef __SKITTLE_DETECTOR_COM_05_H__
#define __SKITTLE_DETECTOR_COM_05_H__

// carte rateau, detection de quille: carte qui detecte le contact electrique enntre 2 branches du rateau

static const unsigned char SKITTLE_DETECTOR_REQ_PING = 0xAA;
static const unsigned char SKITTLE_DETECTOR_PING_ID  = 0xC6;

// renvoie un octet pour dire ou est le premier contact sur le rateau
static const unsigned char SKITTLE_DETECTOR_REQ_GET_BASIC_POSITION = 0x00;

// renvoie 2 octets pour dir quelles sont les lames qui sont en contact
static const unsigned char SKITTLE_DETECTOR_REQ_GET_ALL_POSITIONS = 0x00;

#endif 
