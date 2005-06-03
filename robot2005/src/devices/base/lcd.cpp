#include "devices/lcd.h"
#include <stdarg.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// Interface avec l'afficheur LCD
// ---------------------------------------------------------------------------
LcdCL* LcdCL::lcd_=NULL;

// ---------------------------------------------------------------------------
// Lcd::Lcd
// ---------------------------------------------------------------------------
LcdCL::LcdCL():
  RobotDeviceCL("Lcd", CLASS_LCD) 
{
  assert(!lcd_);
  lcd_=this;
}

// ---------------------------------------------------------------------------
// Lcd::Lcd
// ---------------------------------------------------------------------------
LcdCL::~LcdCL()
{
  lcd_ = NULL;
}

void LcdCL::print(const char* fmt, ...)
{
    // Nevertheless print to stdout (at least we could potentially start with
    // a computer next to the robot).
    va_list argp;
    va_start(argp, fmt);
    vprintf(fmt, argp);
    va_end(argp);
}
