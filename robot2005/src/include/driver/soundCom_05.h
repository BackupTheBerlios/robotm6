#ifndef __SOUND_COM_05_H__
#define __SOUND_COM_05_H__

// carte son 2005
/*
-ping: le pc envoie 0xAA, la carte repond 0xC3  

-playSound: le pc envoie 0xXX  %11XXXXXX   XXXXX numero du titre, la carte ne repond rien
  => la carte met le son dans sa pile pour le jouer quand les sons precedents seront termines (prevoir une pile d'au moins 5 sons)

-stop: le pc envoie 0x00, la carte ne repond rien
  => le son/musique actuelle est arrete et la pile de son est effacee

-playRandomMusic: le pc envoie 0x80, la carte ne repond rien
  => la carte joue des musiques au hasard jusqu'à ce que la commande stop ou playSound soit appelee

Quand la carte ne joue pas de son ou de musique depuis plus de 5 secondes, elle joue un son au hasard (attention ne pas jouer les sons d'alertes...)
*/

static const unsigned char SOUND_REQ_PING = 0xAA;
static const unsigned char SOUND_PING_ID  = 0xC3;

//
static const unsigned char SOUND_REQ_PLAY  = 0xC0; // 11xx xxxx
static const unsigned char SOUND_REQ_CLEAR_STACK  = 0x00;
static const unsigned char SOUND_REQ_MUSIC  = 0x80;

#endif // __SOUND_COM_05_H__
