
#define  LCD_COM_INFO
#include "lcdCom_03.h"

#include "lcd.h"
#include "uart.h"
#include "classConfig.h"
#include "log.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

// =========================================================================
// class Lcd_03
// =========================================================================

// ---------------------------------------------------------------------------
// Lcd_03::Lcd_03
// ---------------------------------------------------------------------------
Lcd_03::Lcd_03() : 
  Lcd(), uart_(NULL)
{
  UartManagerCL* uartMgr = UartManagerCL::instanceNoCheck();
  if (!uartMgr) {
    uartMgr = new UartManagerCL();
  }
  uart_ = uartMgr->getUartById(UART_LCD_03);
  if (uart_ != NULL) {
    init_ = true;
    LOG_OK("Initialization Done\n");
  } else {
    init_ = false;
    LOG_ERROR("Lcd device not found!\n");
  }
}

// ---------------------------------------------------------------------------
// Lcd_03::~Lcd_03
// ---------------------------------------------------------------------------
Lcd_03::~Lcd_03()
{
}

// ---------------------------------------------------------------------------
// Lcd_03::ping
// ---------------------------------------------------------------------------
bool Lcd_03::ping()
{
  if (uart_) {
    return uart_->ping();
  } else {
    LOG_ERROR("Lcd device not found and not pinging\n");
    return false;
  }
}

// ---------------------------------------------------------------------------
// Lcd_03::reset
// ---------------------------------------------------------------------------
// Reset the lcd uart
// ---------------------------------------------------------------------------
bool Lcd_03::reset()
{
  LOG_FUNCTION();
  if (!uart_) {
    uart_ = UartMgr->getUartById(UART_LCD_03);
    if (uart_ != NULL) {
      init_ = true;
      LOG_OK("Initialization Done\n");
    } else {
      init_ = false;
      LOG_ERROR("Lcd device not found!");
    }
    return init_;
  } else {
    init_ = uart_->reset();
    return init_;
  }
}

// ---------------------------------------------------------------------------
// Lcd_03::print
// ---------------------------------------------------------------------------
// This function prints a message on the lcd (2x16). Use it like 
// printf.
// ---------------------------------------------------------------------------
void Lcd_03::print(const char* fmt, ...)
{
  if (!uart_) return;
  char txt[256];
  unsigned char data[2];
  bool status=false;
  va_list argp;
  va_start(argp, fmt);
  vsprintf(txt, fmt, argp);
  va_end(argp);
  // to be sure not exceeding the lcd display
  txt[33]=0; // not 32 because of the first \n
  
  uart_->write(LCD_PRINTF);
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
  uart_->write(LCD_PRINTF_EOF); 
  status = uart_->read(data);

  txt[i]='\n'; txt[i+1]=0;
  LOG_INFO("LCD:\n%s%s%s\n", KB_BLUE, txt, KB_RESTORE);

  if (!status || data[0] != LCD_PRINTF) {
    LOG_ERROR("lcd_printf read status=%s, read value = %d\n", 
	      status?"OK":"ERR", data[0]);
  }

}

// ---------------------------------------------------------------------------
// Lcd_03::clear
// ---------------------------------------------------------------------------
// This function erase the lcd screen
// ---------------------------------------------------------------------------

void Lcd_03::clear()
{
  if (!uart_) return;
  unsigned char data[2];
  bool status = false;
  uart_->write(LCD_CLEAR);
  status = uart_->read(data);

  if (!status || data[0] != LCD_CLEAR) {
    LOG_ERROR("lcd_clear read status=%s, read value = %d\n", 
	      status?"OK":"ERR", data[0]);
  }
}

// ---------------------------------------------------------------------------
// Lcd_03::printMessage
// ---------------------------------------------------------------------------
// This function prints a message on the lcd (2x16). Use it 
// like printf.
// ---------------------------------------------------------------------------
void Lcd_03::printMessage(LcdMessage message)
{
  if (!uart_) return;
  bool status = false;
  unsigned char data[2];
  unsigned char req = LCD_PRINT_MESSAGE+(unsigned char)message;
  char txt[50];
  sprintf(txt, "%s\n",(char*)(lcdMessages[message]));
  LOG_INFO("LCD:\n%s%s%s\n", KB_BLUE, txt, KB_RESTORE);
  uart_->write(req);
  status = uart_->read(data);

  if (!status || data[0] != req) {
    LOG_ERROR("lcd_printMessage read status=%s, read value = 0x%x, sent=0x%x\n", 
	      status?"OK":"ERR", data[0], req);
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
  Lcd_03 lcd;
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

#endif
