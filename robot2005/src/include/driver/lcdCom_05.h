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

#define LCD_CLEAR          0x00
#define LCD_ERROR          0xFF
#define LCD_SET_CURSOR     0x02
#define LCD_BACKLIGHT_ON   0x03
#define LCD_BACKLIGHT_OFF  0x04
#define LCD_PRINTF         0x01
#define LCD_PRINTF_EOF     0x00

#define LCD_PRINT_MESSAGE  0x80



#endif /* __LCD_COM_05_H__ */
