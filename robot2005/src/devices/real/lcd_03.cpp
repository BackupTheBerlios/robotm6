
#define  LCD_COM_INFO
#include "lcdCom_03.h"

#include "implementation/lcd03.h"
#include "log.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

Lcd_03::Lcd_03() :  device_(NULL)
{
    device_ = IoManager->getIoDevice(IO_ID_LCD_03);
    if (device_ != NULL) {
	// TODO: check device-opening for errors [flo]
	device_->open();
	LOG_OK("Initialization Done\n");
    } else {
	LOG_ERROR("Lcd device not found!\n");
    }
}

Lcd_03::~Lcd_03()
{
    device_->close();
}

// ---------------------------------------------------------------------------
// Reset the lcd uart
// ---------------------------------------------------------------------------
bool Lcd_03::reset()
{
  LOG_FUNCTION();
  // TODO: share some code with constructor. [flo]
  if (!device_) {
      device_ = IoManager->getIoDevice(IO_ID_LCD_03);
      if (device_ != NULL) {
	  LOG_OK("Initialization Done\n");
	  return true;
      } else {
	  LOG_ERROR("Lcd device not found!");
	  return false;
      }
  } else {
      return device_->reset();
  }
}

// ---------------------------------------------------------------------------
// This function prints a message on the lcd (2x16). Use it like 
// printf.
// ---------------------------------------------------------------------------
void Lcd_03::print(const char* fmt, ...)
{
  if (!device_) return;
  char txt[256];
  unsigned char data[2];
  bool status=false;
  va_list argp;
  va_start(argp, fmt);
  vsprintf(txt, fmt, argp);
  va_end(argp);
  // to be sure not exceeding the lcd display
  txt[33]=0; // not 32 because of the first \n

  device_->write(LCD_PRINTF);
  unsigned int i=0;
  unsigned int countFromBeginOfLine=0;
  for(i=0; txt[i]!=0; ++i) {
      countFromBeginOfLine++;
      if ((countFromBeginOfLine <= LCD_CHAR_PER_LINE) 
	  || (txt[i] == '\n')) {
	  device_->write(txt[i]);
      }
      if (txt[i] == '\n') {
	  countFromBeginOfLine = 0;
      }
  }
  status = device_->writeRead(LCD_PRINTF_EOF, data);

  txt[i]='\n'; txt[i+1]=0;
  Log->lcdMessage(txt);
  if (!status || data[0] != LCD_PRINTF) {
      LOG_ERROR("lcd_printf read status=%s, read value = %d\n", 
		status?"OK":"ERR", data[0]);
  }
}

// ---------------------------------------------------------------------------
// This function erase the lcd screen
// ---------------------------------------------------------------------------
void Lcd_03::clear()
{
    if (!device_) return;
    unsigned char data[2];
    bool status = false;
    status = device_->writeRead(LCD_CLEAR, data);
    Log->lcdMessage("");
    if (!status || data[0] != LCD_CLEAR) {
	LOG_ERROR("lcd_clear read status=%s, read value = %d\n", 
		  status?"OK":"ERR", data[0]);
    }
}

// ---------------------------------------------------------------------------
// This function prints a message on the lcd (2x16). Use it 
// like printf.
// ---------------------------------------------------------------------------
void Lcd_03::printMessage(LcdMessage message)
{
    if (!device_) return;
    bool status = false;
    unsigned char data[2];
    unsigned char req = LCD_PRINT_MESSAGE+(unsigned char)message;
    char txt[50];
    sprintf(txt, "%s\n",(char*)(lcdMessages[message]));
    LOG_INFO("LCD:\n%s%s%s\n", KB_BLUE, txt, KB_RESTORE);
    status = device_->writeRead(req, data);
    
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
