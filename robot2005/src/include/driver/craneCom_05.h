#ifndef __CRANE_COM_05_H__
#define __CRANE_COM_05_H__

// carte grue : controle des moteurs de l'electroaimant

static const unsigned char CRANE_REQ_PING = 0xAA;
static const unsigned char CRANE_PING_ID  = 0xC4;

// most left position = 10. right = 1F
static const unsigned char CRANE_REQ_SET_HORIZONTAL_POSITION = 0x10;
// up position = 20. down = 2F
static const unsigned char CRANE_REQ_SET_VERTICAL_POSITION = 0x20;

// Homologation. up-down mode.
static const unsigned char CRANE_REQ_UP_DOWN_MODE = 0x30;

// just stops the motors (if we were in a movement)
static const unsigned char CRANE_REQ_DISABLE = 0x40;

// returns the position in one byte (horizontal << 4 | vertical)
static const unsigned char CRANE_REQ_GET_POS = 0x50;

#endif 
