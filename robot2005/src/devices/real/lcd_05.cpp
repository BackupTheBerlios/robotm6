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
     va_list argp;
    va_start(argp, fmt);
    vsprintf(txt, fmt, argp);
    va_end(argp);
    // to be sure not exceeding the lcd display
    txt[33]=0; // not 32 because of the first \n
    
    device_->write(LCD_REQ_PRINTF);
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
    device_->write(LCD_REQ_PRINTF_EOF);

    txt[i]='\n'; txt[i+1]=0;
    if (Log) Log->lcdMessage(txt);
}
void Lcd_05::clear() 
{
    if (!device_) return;
    device_->write(LCD_REQ_CLEAR);
    if (Log) Log->lcdMessage("");
}
/**
 * @brief This function turns on the backlight (retro eclairage)
 * Not implemented on the board we use
 */
void Lcd_05::backlightOn()
{
    if (!device_) return;
    device_->write(LCD_REQ_BACKLIGHT_ON);
}
/**
 * @brief This function turns off the backlight (retro eclairage)
 * Not implemented on the board we use
 */
void Lcd_05::backlightOff()
{
    if (!device_) return;
    device_->write(LCD_REQ_BACKLIGHT_OFF);
}
/**
 * @brief This function turns on the automatic backlight (retro eclairage)
 */
void Lcd_05::automaticBacklightOn()
{
    if (!device_) return;
    device_->write(LCD_REQ_AUTOMATIC_BL_ON);
}
/**
 * @brief This function turns off the automatic backlight (retro eclairage)
 */
void Lcd_05::automaticBacklightOff()
{
    if (!device_) return;
    device_->write(LCD_REQ_AUTOMATIC_BL_OFF);
}
/**
 * @brief This function turn on the led ledId
 */
void Lcd_05::setLed(LcdLed ledId, LcdLedMode mode)
{
    if (!device_) return;
    if (mode == LCD_LED_ON) {
      device_->write(LCD_REQ_LED_ON+ledId);
    } else if (mode == LCD_LED_OFF) {
      device_->write(LCD_REQ_LED_OFF+ledId);
    } else if (mode == LCD_LED_BLINK) {
      device_->write(LCD_REQ_LED_BLINK+ledId);
    }  
}
/**
 * @brief This function set led blinking alternatively
 */
void Lcd_05::ledPolice()
{
    if (!device_) return;
    device_->write(LCD_REQ_POLICE);
}

#ifdef TEST_MAIN

#include "io/serialPort.h"
int main(int argc, char* argv[]) 
{
    IoManager->submitIoHost(new SerialPort(0, SERIAL_SPEED_38400));
#ifndef GUMSTIX
    IoManager->submitIoHost(new SerialPort(1, SERIAL_SPEED_38400));
#endif
    IoManager->submitIoHost(new SerialPort(2, SERIAL_SPEED_38400));
    IoManager->submitIoHost(new SerialPort(3, SERIAL_SPEED_38400));

    Lcd_05 lcd;
    lcd.print("Hello world\n%s", "test");
    sleep(5);
    lcd.setLed(LCD_LED_GREEN, LCD_LED_ON);
    lcd.clear();
    sleep(5);
    lcd.print("led red");
    lcd.setLed(LCD_LED_GREEN, LCD_LED_OFF);
    lcd.setLed(LCD_LED_RED, LCD_LED_ON);
    sleep(5);
    lcd.print("led off");
    lcd.setLed(LCD_LED_RED, LCD_LED_OFF);
    sleep(5);
    lcd.print("led red blink");
    lcd.setLed(LCD_LED_RED, LCD_LED_BLINK);
    sleep(5);
    lcd.print("led green blink");
    lcd.setLed(LCD_LED_RED, LCD_LED_OFF);
    lcd.setLed(LCD_LED_GREEN, LCD_LED_BLINK);
    sleep(5);
    lcd.print("led police");
    lcd.ledPolice();
    return 0;
}

#endif

#ifdef LCD_PRINT_MAIN

#include "io/serialPort.h"
int main(int argc, char* argv[]) 
{
    ClassConfig::find(CLASS_SERIAL_DEVICE)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_SERIAL_PORT)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_IO_MANAGER)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_UBART)->setVerboseLevel(VERBOSE_NO_MESSAGE);
    ClassConfig::find(CLASS_LCD)->setVerboseLevel(VERBOSE_NO_MESSAGE);

    IoManager->submitIoHost(new SerialPort(0, SERIAL_SPEED_38400));
#ifndef GUMSTIX
    IoManager->submitIoHost(new SerialPort(1, SERIAL_SPEED_38400));
#endif
    IoManager->submitIoHost(new SerialPort(2, SERIAL_SPEED_38400));
    IoManager->submitIoHost(new SerialPort(3, SERIAL_SPEED_38400));
    
    if (argc==1) {
      printf("Usage: %s ligne_1 ligne_2_bla_bla\n", argv[0]);
    }
    if (!IoManager->getIoDevice(IO_ID_LCD_05)) {
      printf("Cannot connect to LCD device\n");
      return -1;
    }
    Lcd_05 lcd;
    char txt[60];
    int k=0;
    unsigned int i=0;
    if (argc>1) {
      for(i=0; i<LCD_CHAR_PER_LINE && argv[1][i] != 0; i++) {
	if (argv[1][i] == '_') {
	  txt[k++]=' ';
	} else {
	  txt[k++]=argv[1][i];
	}
      }
      txt[k++]='\n';
    }
    if (argc>2) {
      for(i=0; i<LCD_CHAR_PER_LINE && argv[2][i] != 0; i++) {
	if (argv[1][i] == '_') {
	  txt[k++]=' ';
	} else {
	  txt[k++]=argv[2][i];
	}
      }
    }
    txt[k++]=0;
    lcd.print(txt);
    return 0;
}

#endif
