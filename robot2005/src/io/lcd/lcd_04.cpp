#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <signal.h>

#include "lcdCom_04.h"

#include "implementation/lcd04.h"
#include "uart.h"
#include "classConfig.h"
#include "log.h"
#include "events.h"
#include "mthread.h"

// =========================================================================
// class Lcd_04
// =========================================================================
bool lcd04Filter(UartByte data)
{
    return ((Lcd_04*)Lcd)->filter(data);
}

// ---------------------------------------------------------------------------
// Lcd_04::Lcd_04
// ---------------------------------------------------------------------------
Lcd_04::Lcd_04() : uart_(NULL)
{
    UartManagerCL* uartMgr = UartManagerCL::instanceNoCheck();
    if (!uartMgr) {
        uartMgr = new UartManagerCL();
    }
    uart_ = (UartBuffer*)uartMgr->getUartById(UART_LCD_04);
    if (uart_ != NULL) {
        uart_->registerFilterFunction(lcd04Filter);
        LOG_OK("Initialization Done\n");
    } else {
        LOG_ERROR("Lcd device not found!\n");
    }
}

// ---------------------------------------------------------------------------
// Lcd_04::~Lcd_04
// ---------------------------------------------------------------------------
Lcd_04::~Lcd_04()
{
}

// ---------------------------------------------------------------------------
// Lcd_04::reset
// ---------------------------------------------------------------------------
// Reset the lcd uart
// ---------------------------------------------------------------------------
bool Lcd_04::reset()
{
    LOG_FUNCTION();
    if (!uart_) {
        uart_ = (UartBuffer*)UartMgr->getUartById(UART_LCD_04);
        if (uart_ != NULL) {
            uart_->registerFilterFunction(lcd04Filter);
            LOG_OK("Initialization Done\n");
            return true;
        } else {
            LOG_ERROR("Lcd device not found!");
        }
        return false;
    } else {
        return uart_->reset();
    }
}

// ---------------------------------------------------------------------------
// Lcd_04::print
// ---------------------------------------------------------------------------
// This function prints a message on the lcd (2x16). Use it like 
// printf.
// ---------------------------------------------------------------------------
void Lcd_04::print(const char* fmt, ...)
{
    if (!uart_) return;
    char txt[256];
    va_list argp;
    va_start(argp, fmt);
    vsprintf(txt, fmt, argp);
    va_end(argp);
    // to be sure not exceeding the lcd display
    txt[33]=0; // not 32 because of the first \n
    
    uart_->write(LCD_REQ_PRINTF);
    unsigned int i=0;
    unsigned int countFromBeginOfLine=0;
    for(i=0; txt[i]!=0; ++i) {
        countFromBeginOfLine++;
        if ((countFromBeginOfLine <= LCD_CHAR_PER_LINE) 
            || (txt[i] == '\n')) {
            uart_->write(txt[i]);
        }
        if (txt[i] == '\n') {
            countFromBeginOfLine = 0;
        }
    }
    uart_->write(LCD_REQ_PRINTF_EOF); 
    
    txt[i]='\n'; txt[i+1]=0;
    Log->lcdMessage(txt);
}

// ---------------------------------------------------------------------------
// Lcd_04::clear
// ---------------------------------------------------------------------------
// This function erase the lcd screen
// ---------------------------------------------------------------------------

void Lcd_04::clear()
{
    if (!uart_) return;
    uart_->write(LCD_REQ_CLEAR);
    Log->lcdMessage("");
}

// ---------------------------------------------------------------------------
// Lcd_04::backlightOn
// ---------------------------------------------------------------------------
// This function turns on the backlight (retro eclairage)
// Not implemented on the board we use
// ---------------------------------------------------------------------------

void Lcd_04::backlightOn()
{
    if (!uart_) return;
    uart_->write(LCD_REQ_BACKLIGHT_ON);
}

// ---------------------------------------------------------------------------
// Lcd_04::ledPolice
// ---------------------------------------------------------------------------
void Lcd_04::ledPolice()
{
    if (!uart_) return;
    uart_->write(LCD_REQ_POLICE);
}

// ---------------------------------------------------------------------------
// Lcd_04::backlightOff
// ---------------------------------------------------------------------------
// This function turns off the backlight (retro eclairage)
// Not implemented on the board we use
// ---------------------------------------------------------------------------
void Lcd_04::backlightOff()
{
    if (!uart_) return;
    uart_->write(LCD_REQ_BACKLIGHT_OFF);
}

// ---------------------------------------------------------------------------
// Lcd_04::setLed
// ---------------------------------------------------------------------------
// This function turns on/off a led
// Not implemented on the board we use
// ---------------------------------------------------------------------------
void Lcd_04::setLed(LcdLed     ledId,
                    LcdLedMode ledMode)
{
    if (!uart_) return;
    switch(ledMode) {
    case LCD_LED_ON:
        uart_->write(LCD_REQ_LED_ON+(unsigned char)ledId);
        break;
    case LCD_LED_BLINK:
        uart_->write(LCD_REQ_LED_BLINK+(unsigned char)ledId);
        break;
    case LCD_LED_OFF:
        uart_->write(LCD_REQ_LED_OFF+(unsigned char)ledId);
        break;  
    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// Lcd_04::automaticBacklightOn
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void Lcd_04::automaticBacklightOn()
{
    if (!uart_) return;
    uart_->write(LCD_REQ_AUTOMATIC_BL_ON);
}

// ---------------------------------------------------------------------------
// Lcd_04::automaticBacklightOff
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void Lcd_04::automaticBacklightOff()
{
    if (!uart_) return;
    uart_->write(LCD_REQ_AUTOMATIC_BL_OFF);
}

// ---------------------------------------------------------------------------
// Lcd_04::filter
// ---------------------------------------------------------------------------
bool Lcd_04::filter(UartByte data)
{
  // printf("lcd: 0x%x\n", data);
    if (data == LCD_EVENTS_BTN1_PUSHED) {
        Events->raise(EVENTS_BUTTON_YES);
        return true;
    } else if (data == LCD_EVENTS_BTN1_RELEASED) {
        Events->unraise(EVENTS_BUTTON_YES);
        return true;
    } else if (data == LCD_EVENTS_BTN2_PUSHED) {
        Events->raise(EVENTS_BUTTON_NO);
        return true;
    } else if (data == LCD_EVENTS_BTN2_RELEASED) {
        Events->unraise(EVENTS_BUTTON_NO);
        return true;
    } else {
        return false; // data is not filtered
    }
}

#ifdef MAKE_MAIN_LCD_PRINT
// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char* argv[]) 
{
    ClassConfig::find(CLASS_MTHREAD)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_ROBOT_TIMER)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_UART)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_UART_MANAGER)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_IO_MANAGER)->setVerboseLevel(VERBOSE_NO_MESSAGE);

    EVENTS_MANAGER_DEFAULT event;
    Lcd_04 lcd;
    char txt[256];
    if (argc>1) {
        strcpy(txt, argv[1]);
    }
    if (argc>2) {
        sprintf(txt, "%s\n%s", txt,argv[2]);
    }
    for(unsigned int i=0;i<strlen(txt); i++) {
        if (txt[i]=='_') txt[i]=' ';
    }
    lcd.print(txt);
    return 0;
}
#endif // MAKE_MAIN_LCD_PRINT

// ===========================================================================
// MAIN de test
// ===========================================================================

#ifdef MAKE_MAIN

// ---------------------------------------------------------------------------
// lcd04SIGINT
// ---------------------------------------------------------------------------
extern "C" void lcd04SIGINT(int sig)
{
    MTHREAD_KILL_ALL();
    exit(1);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char*argv[]) 
{
    printf("+-----------------------------------------------------+\n");
    printf("| LCD_04 -- testeur --                                |\n");
    printf("+-----------------------------------------------------+\n");
    ClassConfig::find(CLASS_MTHREAD)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_ROBOT_TIMER)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_UART)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_UART_MANAGER)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_IO_MANAGER)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_LCD)->setVerboseLevel(VERBOSE_DEBUG);
    
    EVENTS_MANAGER_DEFAULT event;
    Lcd_04 lcd;
    // enregistre la detection du Ctrl+C et du callback correspondant qui 
    // stoppe le programme immediatement
    (void) signal(SIGINT, lcd04SIGINT);
    while(1) {
      int order;
      printf("Menu: 0=police, 1=clear, 2=message, 3=backlightOn, 4=backlightOff "
	     "\n5=automaticBacklightOn, 6=automaticBacklightOff, 7=ledOn, "
	     "8=ledBlink, 9=ledOff, \n 10=ping 11 exit\n>");
      if (scanf("%d", &order)!=1) {
          printf("Vous avez entre une valeur incorrecte.\n");
          goto LcdEnd;
      }
      switch(order) {
      case 1:
          lcd.clear();
          break;
      case 2:
          lcd.print("ca s'affiche\n1234567890ABCDEF");
          break;
      case 3:
          lcd.backlightOn();
          break;
      case 4:
          lcd.backlightOff();
          break;
      case 5:
          lcd.automaticBacklightOn();
          break;
      case 6:
          lcd.automaticBacklightOff();
          break;
      case 7:
          {
              int ledId=0;
              printf("Entrer le numero de la led a allumer [0;1]\n>");
              scanf("%d", &ledId);
              if (ledId<0) ledId=0;
              else if (ledId>1) ledId=1;
              lcd.setLed((LcdLed)ledId, LCD_LED_ON);
          }
          break;	
      case 8:
          {
              int ledId=0;
              printf("Entrer le numero de la led a blinker [0;1]\n>");
              scanf("%d", &ledId);
              if (ledId<0) ledId=0;
              else if (ledId>1) ledId=1;
              lcd.setLed((LcdLed)ledId, LCD_LED_BLINK);
          }
          break;
	case 9:
          {
              int ledId=0;
              printf("Entrer le numero de la led a eteindre [0;1]\n>");
              scanf("%d", &ledId);
              if (ledId<0) ledId=0;
              else if (ledId>1) ledId=1;
              lcd.setLed((LcdLed)ledId, LCD_LED_OFF);
          }
          break;
	case 0:
	  lcd.ledPolice();
	  break;
      case 11:
          goto LcdEnd;
      default:
          printf("Requete inconnue %d\n", order);
          break;
      } 
    }
 LcdEnd:
    printf("Bye\n");
    MTHREAD_KILL_ALL();
    return 0;
}

#endif
