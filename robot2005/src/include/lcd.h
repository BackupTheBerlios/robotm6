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

#include "robotBase.h"

#define Lcd LcdCL::instance()

typedef unsigned int LcdMessage;
static const unsigned int LCD_CHAR_PER_LINE = 16;

typedef enum LcdLed {
    LCD_LED_LEFT=1,
    LCD_LED_RIGHT=0
} LcdLed;

static const LcdLed LCD_LED_RED   = LCD_LED_RIGHT;
static const LcdLed LCD_LED_GREEN = LCD_LED_LEFT;

typedef enum LcdLedMode {
    LCD_LED_ON=0,
    LCD_LED_BLINK=1,
    LCD_LED_OFF=2
} LcdLedMode;

class UartBuffer;
class Uart;

// ===========================================================================
// class Lcd
// ===========================================================================
/**
 * @class Lcd
 * Interface avec l'afficheur LCD
 */

class LcdCL : public RobotIODevice
{
 public:
  static LcdCL* instance();
  virtual ~LcdCL();

  /**
   * @brief Test the device
   */
  bool validate() { print("Hello World\n"); return true; }
  /**
   * @brief This function prints a message on the lcd (2x16). Use it like 
   * printf.
   */
  virtual void print(const char* fmt, ...)=0;
  /**
   * @brief This function erase the lcd screen
   */
  virtual void clear()=0;
  /**
   * @brief This function prints a message on the lcd (2x16). Use it 
   * like printf.
   */
  virtual void printMessage(LcdMessage message)=0;
  /**
   * @brief This function turns on the backlight (retro eclairage)
   */
  virtual void backlightOn()=0;
  /**
   * @brief This function turns off the backlight (retro eclairage)
   */
  virtual void backlightOff()=0;
  /**
   * @brief This function turns on the automatic backlight (retro eclairage)
   */
    virtual void automaticBacklightOn()=0;
  /**
   * @brief This function turns off the automatic backlight (retro eclairage)
   */
    virtual void automaticBacklightOff()=0;
  /**
   * @brief This function turn on/off/blink the led ledId
   */
    virtual void setLed(LcdLed ledId, LcdLedMode mode)=0;
  /**
   * @brief This function set the led blinking alternatively
   */
    virtual void ledPolice()=0;

 private:
  LcdCL(const LcdCL& lcd); // disabled
 protected:
  LcdCL();
  static LcdCL* lcd_; // singleton
};

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
  
  bool isSimu() const { return true; }
  bool reset()        { init_=false; return false; }
  bool ping()         { return true; }

 /**
   * @brief This function prints a message on the lcd (2x16). Use it like 
   * printf.
   */
  void print(const char* fmt, ...);
  /**
   * @brief This function erase the lcd screen
   */
  void clear(){}
  /**
   * @brief This function prints a message on the lcd (2x16). Use it 
   * like printf.
   */
  void printMessage(LcdMessage message);
  /**
   * @brief This function turns on the backlight (retro eclairage)
   */
  void backlightOn(){}
  /**
   * @brief This function turns off the backlight (retro eclairage)
   */
  void backlightOff(){}
   /**
   * @brief This function turns on the automatic backlight (retro eclairage)
   */
  void automaticBacklightOn(){}
  /**
   * @brief This function turns off the automatic backlight (retro eclairage)
   */
  void automaticBacklightOff(){}

  /**
   * @brief This function turn on the led ledId
   */
  void setLed(LcdLed ledId, LcdLedMode mode){}
  void ledPolice(){}
 protected:
  LcdSimu();
  friend class UartManagerCL;
  friend int main(int argc, char* argv[]);
};
  

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

  bool isSimu() const { return false; }

  /**
   * @brief Return true if the lcd board is responding
   */
  bool ping();

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
  /**
   * @brief This function turns on the backlight (retro eclairage)
   */
   void backlightOn(){}
  /**
   * @brief This function turns off the backlight (retro eclairage)
   */
   void backlightOff(){}
   /**
   * @brief This function turns on the automatic backlight (retro eclairage)
   */
   void automaticBacklightOn(){}
  /**
   * @brief This function turns off the automatic backlight (retro eclairage)
   */
   void automaticBacklightOff(){}

  /**
   * @brief This function turn on the led ledId
   */
   void setLed(LcdLed ledId, LcdLedMode mode){}
    void ledPolice(){}
 protected:
  Lcd_03();
  Uart* uart_;
  friend class UartManagerCL;
  friend int main(int argc, char* argv[]);
};

/** 
 * @class Lcd_04
 * Implementation des fonctions de class Lcd dans le cas d'un afficheur lcd 
 * sur uart. Protocole de communication adapte a la carte 2003
 */
class Lcd_04 : public LcdCL
{
 public:
  ~Lcd_04();

  bool isSimu() const { return false; }

  /**
   * @brief Return true if the lcd board is responding
   */
  bool ping();

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
  Lcd_04();
  UartBuffer* uart_;
  friend class UartManagerCL;
  friend int main(int argc, char* argv[]);

  bool filter(Byte data);
  friend bool lcd04Filter(Byte data);
};

// ===========================================================================
// inline functions
// ===========================================================================

// ---------------------------------------------------------------------------
// LcdCL::instance
// ---------------------------------------------------------------------------
inline LcdCL* LcdCL::instance()
{
  assert(lcd_!=NULL); // lcd must have been initialized by IoManager
  return lcd_;
}

#endif // __LCD_H__
