#include "devices/implementation/lcd05.h"
#include "io/ioDevice.h"
#include "io/ioManager.h"
#include "driver/lcdCom_05.h"
#include "log.h"
#include <stdarg.h>
#include <stdio.h>

Lcd_05::Lcd_05() : device_(NULL)
{
    device_ = IoManager->getIoDevice(IO_ID_LCD_05);
    if (device_ != NULL) {
	if (device_->open()) {
	    LOG_OK("Initialization Done\n");
	} else {
	    device_=NULL;
	    LOG_ERROR("device-open for lcd 05 failed.\n");
	}
    } else {
        LOG_ERROR("lcd 05 device not found!\n");
    } 
}
Lcd_05::~Lcd_05()
{
    if (device_) device_->close();
}
void Lcd_05::print(const char* fmt, ...)
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
void Lcd_05::clear() 
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
