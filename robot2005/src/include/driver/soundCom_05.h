#ifndef __SOUND_COM_05_H__
#define __SOUND_COM_05_H__

// carte son 2005

static const unsigned char SOUND_REQ_PING = 0xAA;
static const unsigned char SOUND_PING_ID  = 0xC3;

// demande la tension des 4 batteries : renvoie 4 octects (1 octect par batterie)
static const unsigned char SOUND_REQ_PLAY  = 0x00;
static const unsigned char SOUND_REQ_CLEAR_STACK  = 0x0FF;

#endif // __SOUND_COM_05_H__
