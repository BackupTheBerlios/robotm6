/**
 * @file lcd.h
 * 
 * @author Laurent Saint-Marcel
 *
 * Gestion de l'afficheur lcd qui communique sur port UART. 
 * L'interface est commune aux cartes 2003 et 2004
 */

#ifndef __LCD_SIMU_H__
#define __LCD_SIMU_H__

#include "devices/lcd.h"

// ===========================================================================
// class LcdSimu
// ===========================================================================

/** 
 * @class LcdSimu
 * Implementation des fonctions de class Lcd dans le cas d'un afficheur lcd 
 * sur uart
 */
class LcdSimu : public LcdCL
{
 public:
  virtual ~LcdSimu();
  LcdSimu();

  bool exists() const {return true; }

 /**
   * @brief This function prints a message on the lcd (2x16). Use it like 
   * printf.
   */
  void print(const char* fmt, ...);
};
  

#endif // __LCD_SIMU_H__
