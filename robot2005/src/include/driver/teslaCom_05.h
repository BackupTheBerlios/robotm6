#ifndef __TESLA_COM_05_H__
#define __TESLA_COM_05_H__

// carte controle electroaimant

static const unsigned char TESLA_REQ_PING = 0xAA;
static const unsigned char TESLA_PING_ID  = 0xC7;

static const unsigned char TESLA_OK  = 0x01;
static const unsigned char TESLA_NOK  = 0x02;

static const unsigned char TESLA_REQ_AUTOTEST          = 0x71;
static const unsigned char TESLA_REQ_DETECT_VALUE      = 0x72;
static const unsigned char TESLA_REQ_ENABLE_DETECTOR   = 0x73;
static const unsigned char TESLA_REQ_DISABLE_DETECTOR  = 0x74;
static const unsigned char TESLA_REQ_STOP_ELECTRO      = 0x75;
static const unsigned char TESLA_REQ_START_ELECTRO     = 0x76; // 0x76 .. 0x80


#endif 
