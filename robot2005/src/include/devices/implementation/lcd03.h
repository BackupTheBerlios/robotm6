/**
 * @file lcd.h
 * 
 * @author Laurent Saint-Marcel
 *
 * Gestion de l'afficheur lcd qui communique sur port UART. 
 * L'interface est commune aux cartes 2003 et 2004
 */

#ifndef __LCD_03_H__
#define __LCD_03_H__

#include "lcd.h"

class UartBuffer;
class Uart;


// ===========================================================================
// class Lcd_03
// ===========================================================================

/** 
 * @class Lcd_03
 * Implementation des fonctions de class Lcd dans le cas d'un afficheur lcd 
 * sur uart. Protocole de communication adapte a la carte 2003
 */
class Lcd_03 : public LcdCL
{
 public:
  ~Lcd_03();
  Lcd_03();

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
   * @brief This function erase the lcd screen
   */
  void clear();
  /**
   * @brief This function prints a message on the lcd (2x16). Use it 
   * like printf.
   */
  void printMessage(LcdMessage message);
 
 protected:
  Uart* uart_;
};


#endif // __LCD_03_H__
