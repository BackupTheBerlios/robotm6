#ifndef __ENV_DETECTOR_COM_05_H__
#define __ENV_DETECTOR_COM_05_H__

// carte detecteur d'environnement 2005

static const unsigned char ENV_DETECTOR_REQ_PING = 0xAA;
static const unsigned char ENV_DETECTOR_PING_ID  = 0xCF;

static const unsigned char ENV_DETECTOR_GET_DATA  = 0xCF;

// ping:
// Le pc envoie ENV_DETECTOR_REQ_PING la carte repond ENV_DETECTOR_PING_ID

// recupperer les donnes des 4 capteurs:
// le pc envoie ENV_DETECTOR_GET_DATA et la carte renvoie les donnees des sharps 
// sur un octet: 2 bits par capteurs

#endif // __ENV_DETECTOR_COM_05_H__
