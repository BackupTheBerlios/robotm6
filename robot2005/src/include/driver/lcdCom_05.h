/*
 * @file : lcdCom_05.h
 *
 * Description :
 * Protocole de communication avec la carte LCD 2005 et liste de messages
 *
 *
 * @date    10/08/02
 * @author  Laurent Saint-Marcel
 */

#ifndef __LCD_COM_05_H__ 
#define __LCD_COM_05_H__

#define LCD_PING           0xAA
#define LCD_UART_ID        0xC2

// la carte ne repond pas aux requetes suivantes
#define LCD_REQ_PRINTF           0x01
#define LCD_REQ_PRINTF_EOF       0x00
#define LCD_REQ_CLEAR            0x02
#define LCD_REQ_BACKLIGHT_ON     0x03
#define LCD_REQ_BACKLIGHT_OFF    0x04
#define LCD_REQ_AUTOMATIC_BL_ON  0x05
#define LCD_REQ_AUTOMATIC_BL_OFF 0x06
#define LCD_REQ_POLICE           0x07
/// envoie LCD_REQ_LED_ + le numero de la led
#define LCD_REQ_LED_ON           0x20
#define LCD_REQ_LED_OFF          0x30
#define LCD_REQ_LED_BLINK        0x40

#endif /* __LCD_COM_05_H__ */
