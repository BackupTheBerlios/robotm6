/**
 * @file lcd.h
 * 
 * @author Laurent Saint-Marcel
 *
 * Gestion de l'afficheur lcd qui communique sur port UART. 
 * L'interface est commune aux cartes 2003 et 2004
 */

#ifndef __LCD_05_H__
#define __LCD_05_H__

#include "lcd.h"

class IoDevice;


class Lcd_05 : public LcdCL
{
 public:
  ~Lcd_05();
  Lcd_05();

  bool exists() const { return true; }

  /**
   * @brief Reset the lcd uart
   */
  bool reset() { clear(); return true;}

  /**
   * @brief This function prints a message on the lcd (2x16). Use it like 
   * printf.
   */
  void print(const char* fmt, ...);  
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
 private:
  IoDevice* device_;
};

#endif // __LCD_H__
