
#define  LCD_COM_INFO
#include "lcdCom_03.h"
#include "lcd.h"
#include "log.h"
#include "simulatorClient.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

// =========================================================================
// class LcdSimu
// =========================================================================

// ---------------------------------------------------------------------------
// Lcd::Lcd
// ---------------------------------------------------------------------------
LcdSimu::LcdSimu() : 
  Lcd() 
{
  init_=true;
}

// ---------------------------------------------------------------------------
// Lcd::Lcd
// ---------------------------------------------------------------------------
LcdSimu::~LcdSimu()
{
}
 
// ---------------------------------------------------------------------------
// Lcd::print
// ---------------------------------------------------------------------------
// This function prints a message on the lcd (2x16). Use it like 
// printf.
// ---------------------------------------------------------------------------
void LcdSimu::print(const char* fmt, ...)
{
  char txt[100];
  va_list argp;
  va_start(argp, fmt);
  vsprintf(txt, fmt, argp);
  va_end(argp);
  // to be sure not exceeding the lcd display
  txt[33]=0; // not 32 because of the first \n
  Log->lcdMessage(txt);
  Simulator->setLCDMessage(txt);
}


// ---------------------------------------------------------------------------
// Lcd::printMessage
// ---------------------------------------------------------------------------
// This function prints a message on the lcd (2x16). Use it 
// like printf.
// ---------------------------------------------------------------------------
void LcdSimu::printMessage(LcdMessage message)
{
  LOG_INFO("LCD: \n%s%s%s\n", KB_BLUE, lcdMessages[message], KB_RESTORE);
}
