/**
 * @file lcd.h
 * 
 * @author Laurent Saint-Marcel
 *
 * Gestion de l'afficheur lcd qui communique sur port UART. 
 * L'interface est commune aux cartes 2003 et 2004
 */

#ifndef __LCD_H__
#define __LCD_H__

#include "robotDevice.h"

#define Lcd LcdCL::instance()

typedef unsigned int LcdMessage;
static const unsigned int LCD_CHAR_PER_LINE = 16;

typedef enum LcdLed {
    LCD_LED_RIGHT=1,
    LCD_LED_LEFT=0
} LcdLed;

static const LcdLed LCD_LED_RED   = LCD_LED_RIGHT;
static const LcdLed LCD_LED_GREEN = LCD_LED_LEFT;

typedef enum LcdLedMode {
    LCD_LED_ON=0,
    LCD_LED_BLINK=1,
    LCD_LED_OFF=2
} LcdLedMode;

// ===========================================================================
// class Lcd
// ===========================================================================
/**
 * @class Lcd
 * Interface avec l'afficheur LCD
 */

class LcdCL : public RobotDeviceCL
{
public:
    LcdCL();
    static LcdCL* instance();
    virtual ~LcdCL();

    bool exists() const { return false; }
    
  /**
   * @brief This function prints a message on the lcd (2x16). Use it like 
   * printf.
   */
  virtual void print(const char* fmt, ...){}
  /**
   * @brief This function erase the lcd screen
   */
  virtual void clear(){}
  /**
   * @brief This function prints a message on the lcd (2x16). Use it 
   * like printf.
   */
  virtual void printMessage(LcdMessage message){}
  /**
   * @brief This function turns on the backlight (retro eclairage)
   */
  virtual void backlightOn(){}
  /**
   * @brief This function turns off the backlight (retro eclairage)
   */
  virtual void backlightOff(){}
  /**
   * @brief This function turns on the automatic backlight (retro eclairage)
   */
  virtual void automaticBacklightOn(){}
  /**
   * @brief This function turns off the automatic backlight (retro eclairage)
   */
  virtual void automaticBacklightOff(){}
  /**
   * @brief This function turn on/off/blink the led ledId
   */
  virtual void setLed(LcdLed ledId, LcdLedMode mode){}
  /**
   * @brief This function set the led blinking alternatively
   */
  virtual void ledPolice(){}

 private:
  LcdCL(const LcdCL& lcd); // disabled

protected:
  static LcdCL* lcd_; // singleton
};

// ===========================================================================
// inline functions
// ===========================================================================

// ---------------------------------------------------------------------------
// LcdCL::instance
// ---------------------------------------------------------------------------
inline LcdCL* LcdCL::instance()
{
  assert(lcd_); // lcd must have been initialized by IoManager
  return lcd_;
}

#endif // __LCD_H__
