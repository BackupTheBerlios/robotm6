/**
 * @file lcd.h
 * 
 * @author Laurent Saint-Marcel
 *
 * Gestion de l'afficheur lcd qui communique sur port UART. 
 * L'interface est commune aux cartes 2003 et 2004
 */

#ifndef __LCD_04_H__
#define __LCD_04_H__

#include "lcd.h"

class UartBuffer;
class Uart;


/** 
 * @class Lcd_04
 * Implementation des fonctions de class Lcd dans le cas d'un afficheur lcd 
 * sur uart. Protocole de communication adapte a la carte 2003
 */
class Lcd_04 : public LcdCL
{
 public:
  ~Lcd_04();
  Lcd_04();

  bool exists() const { return true; }

  /**
   * @brief Reset the lcd uart
   */
  bool reset();

  /**
   * @brief This function prints a message on the lcd (2x16). Use it like 
   * printf.
   */
  void print(const char* fmt, ...);  
  /**
   * @brief This function prints a message on the lcd (2x16). Use it 
   * like printf.
   */
  void printMessage(LcdMessage message){}
  /**
   * @brief This function erase the lcd screen
   */
  void clear();
  /**
   * @brief This function turns on the backlight (retro eclairage)
   * Not implemented on the board we use
   */
  void backlightOn();
  /**
   * @brief This function turns off the backlight (retro eclairage)
   * Not implemented on the board we use
   */
  void backlightOff();
  /**
   * @brief This function turns on the automatic backlight (retro eclairage)
   */
  void automaticBacklightOn();
  /**
   * @brief This function turns off the automatic backlight (retro eclairage)
   */
  void automaticBacklightOff();
  /**
   * @brief This function turn on the led ledId
   */
  void setLed(LcdLed ledId, LcdLedMode mode);
  /**
   * @brief This function set led blinking alternatively
   */
  void ledPolice();
 
protected:
  UartBuffer* uart_;
  friend class UartManagerCL;
  friend int main(int argc, char* argv[]);

  bool filter(Byte data);
  friend bool lcd04Filter(Byte data);
};

#endif // __LCD_H__
