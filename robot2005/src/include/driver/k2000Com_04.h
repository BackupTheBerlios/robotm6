#ifndef __K2000_COM_04_H__
#define __K2000_COM_04_H__

static const unsigned char K2000_REQ_PING = 0xAA;
static const unsigned char K2000_PING_ID  = 0xBC;

static const unsigned char K2000_REQ_MODE_OFF      = 0x00;
static const unsigned char K2000_REQ_MODE_FREE     = 0x01;
static const unsigned char K2000_REQ_MODE_K2000    = 0x02;
static const unsigned char K2000_REQ_MODE_CENTER   = 0x03;
static const unsigned char K2000_REQ_MODE_EXTERN   = 0x04;
static const unsigned char K2000_REQ_MODE_LEFT     = 0x05;
static const unsigned char K2000_REQ_MODE_RIGHT    = 0x06;
static const unsigned char K2000_REQ_MODE_POLICE   = 0x07;

#endif // __K2000_COM_04_H__
