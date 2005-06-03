#ifndef __TESLA_COM_05_H__
#define __TESLA_COM_05_H__

// carte controle electroaimant

static const unsigned char TESLA_REQ_PING = 0xAA;
static const unsigned char TESLA_PING_ID  = 0xCE;

/*
static const unsigned char TESLA_OK  = 0x01;
static const unsigned char TESLA_NOK  = 0x02;
*/

// TODO: not yet implemented.
static const unsigned char TESLA_REQ_DETECT_VALUE      = 0x00;
static const unsigned char TESLA_REQ_ENABLE_DETECTOR   = 0x00;
static const unsigned char TESLA_REQ_DISABLE_DETECTOR  = 0x00;

static const unsigned char TESLA_REQ_STOP_ELECTRO = 0x01;
static const unsigned char TESLA_REQ_TENSION = 0x04;

#endif 
