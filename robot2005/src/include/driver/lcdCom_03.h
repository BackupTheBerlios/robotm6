/**
 * @file : lcdCom_03.h
 *
 * Description :
 * Protocole de communication avec la carte LCD 2003 et liste de messages
 *
 * Pour l'atmel :
 *   #define LCD_COM_INFO
 *   #include "lcdCom_03.h"
 *
 * @date    10/08/02
 * @author  Laurent Saint-Marcel
 */

#ifndef __LCD_COM_03_H__ 
#define __LCD_COM_03_H__

#define LCD_PING           0xAA
#define LCD_UART_ID        0xA2

#define LCD_CLEAR          0x00
#define LCD_ERROR          0xFF
#define LCD_SET_CURSOR     0x02
#define LCD_BACKLIGHT_ON   0x03
#define LCD_BACKLIGHT_OFF  0x04
#define LCD_PRINTF         0x01
#define LCD_PRINTF_EOF     0x00

#define LCD_PRINT_MESSAGE  0x80


typedef enum LcdMessage_03
{
  LCD_BOOTING=0,
  LCD_INIT,
  LCD_CONFIGURE_ALL,
  LCD_AUTO_CHECK,
  LCD_WAIT_JACK_IN,
  LCD_WAIT_JACK_OUT,
  LCD_READY_WAIT_START,
  LCD_UNLOCK_EMERGENCY_STOP,
  LCD_RELEASE_ALL_PUCKS,
  LCD_GAME_OVER,
  LCD_PUCK_DETECTED,
  /* BEGIN could be erase if there is not enough memory in at90s2313 */
  LCD_PUCK_IS_RED,
  LCD_PUCK_IS_GREEN,
  LCD_PUCK_IS_RIGHT,
  LCD_PUCK_IS_LEFT,
  LCD_ARRET_D_URGENCE_VITE,
  LCD_JE_SUIS_UN_GENTIL_ROBOT,
  LCD_JE_SUIS_UN_MECHANT_ROBOT,
  LCD_SALUT,
  /* END could be erase if there is not enough memory in at90s2313 */
  LCD_AU_FEU,

  LCD_REV_ERROR,
  LCD_REV_VERSION_ERROR,
  LCD_SERVO_ERROR,
  LCD_COLOR_ERROR,
  LCD_LASER_ERROR,
  
  LCD_REV_CALIBRATE,
  LCD_REV_CANT_GO_UP,
  LCD_CANT_FREE_ENTRY,
  LCD_SWITCH_WHITE_CONFIRM,
  LCD_SWITCH_BLACK_CONFIRM,

  LCD_MESSAGE_NUMBER
} LcdMessage_03;

typedef LcdMessage_03 LcdMessageEnum;

#ifdef LCD_COM_INFO

static const char* lcdMessages[(unsigned char)LCD_MESSAGE_NUMBER]={
  "Booting...",
  "Initialisation",
  "Calibrating...",
  "Verifying\n captors",
  "Wait Jack In",
  "Wait Jack Out",
  "Ready to start\n Wait start Jack",
  "Unlock\nemergency stop",
  "Releasing\n all pucks",
  "Game over\nWinner     Loser",
  "Puck detected",

  "Puck is red",
  "Puck is green",
  "Puck is\n on right",
  "Puck is\n on left",
  "Arret d'urgence!\n Vite !!",
  "Je suis un\n gentil robot",
  "Je suis un\n mechant robot",
  "Salut",

  "Au feu, de l'eau\n Je brule!!", 

  "Ping err retourn\nRetry    Skip",
  "Rev version err\nSkip    Skip",
  "Ping err servo\nRetry    Skip",
  "Ping err color\nRetry    Skip",
  "Ping err laser\nRetry    Skip",

  "Press any key to\ncalibrate rev",
  "Rev cant go up\nRetry    Skip",
  "Cant free entry\nRetry    Skip",
  "Color switcher\nWhite confirm",
  "Color switcher\nBlack confirm",
};

/* quand on envoie
   LCD_PRINT_MESSAGE | (unsigned char)LcdMessageId
   on affiche le message lcdMessages[(unsigned char)LcdMessageId].
   Attention il faut avoir teste le cas 0xAA avant
   Pour l'atmel si vous avez la fonction printf(const char*), y'a qu'a faire
   printf(lcdMessages[request&0x7F])
*/

#endif /* LCD_COM_INFO */

#endif /* __LCD_COM_03_H__ */
