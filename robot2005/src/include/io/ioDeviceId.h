/**
 * @file ioDeviceId.h
 *
 * @author Florian Loitsch
 *
 * a list of IoDeviceIds and their attached Infos.
 */

#pragma once

#include "robotTypes.h"
#include "types.h"
/**
 * @brief list of all recognize-able peripherals
 */
enum IoId {
    IO_ID_NONE=0,
    IO_ID_LCD_03,
    IO_ID_COLOR_03,
    IO_ID_VOLTAGE_03,
    IO_ID_SERVO_03,
    IO_ID_ENV_DETECTOR_03,

    IO_ID_LCD_04,
    IO_ID_ODOMETER_04,
    IO_ID_SERVO_04,
    IO_ID_VOLTAGE_04,
    IO_ID_ENV_DETECTOR_04_0,
    IO_ID_ENV_DETECTOR_04_1,
    IO_ID_GONIO_04,  
    IO_ID_CATAPULT_04,
    IO_ID_K2000_04,

    IO_ID_MOTOR_ODOM_05,
    IO_ID_LCD_05,
    IO_ID_BUMPER_05,
    IO_ID_SOUND_05,
    IO_ID_ENV_05,
    IO_ID_SERVO_05,
    IO_ID_ALIM_05,
    IO_ID_TESLA_05,
    IO_ID_SKITTLE_DETECTOR_05,
    IO_ID_CRANE_05,
    
    IO_ID_NBR
};

// TODO: maybe put IoInfo into its own header, and put ioInfos_ into its own cpp-file. [flo]
/**
 * @brief information that is useful in treating a connected peripheral
 * (without knowing what it actually does).
 * In the future this could potentially also contain stuff like
 * the ping-delay, ...
 * ATM the pingId is used as scanInfo too.
 */
struct IoInfo {
    IoId id;
    // TODO: seperate type for pingId?
    IoByte pingId;
    char name[64];
};

/**
 * @brief an array of know ioInfos.
 */
static const IoInfo ioInfos_[IO_ID_NBR] = {
    // IoId                    pingId   name
    // --------------------------------------------
    {IO_ID_NONE,               0x00,    "IO_NONE"},
    
    {IO_ID_LCD_03,             0xA2,    "IO_LCD_03"},
    {IO_ID_COLOR_03,           0xA3,    "IO_COLOR_03"},
    {IO_ID_VOLTAGE_03,         0xAB,    "IO_VOLTAGE_03"},
    {IO_ID_SERVO_03,           0xA4,    "IO_SERVO_03"},
    {IO_ID_ENV_DETECTOR_03,    0xA9,    "IO_ENV_DETECTOR_03"},
    
    {IO_ID_LCD_04,             0xB2,    "IO_LCD_04"},
    {IO_ID_ODOMETER_04,        0xB7,    "IO_ODOMETER_04"},
    {IO_ID_SERVO_04,           0xB4,    "IO_SERVO_04"},
    {IO_ID_VOLTAGE_04,         0xBB,    "IO_VOLTAGE_04"},
    {IO_ID_ENV_DETECTOR_04_0,  0xB8,    "IO_ENV_DETECTOR_04_0"},
    {IO_ID_ENV_DETECTOR_04_1,  0xB9,    "IO_ENV_DETECTOR_04_1"},
    {IO_ID_GONIO_04,           0xB5,    "IO_GONIO_04"},
    {IO_ID_CATAPULT_04,        0xB1,    "IO_CATAPULT_04"},
    {IO_ID_K2000_04,           0xBC,    "IO_K2000_04"},

    {IO_ID_MOTOR_ODOM_05,      0xC0,    "IO_MOTOR_ODOM_05"},
    {IO_ID_LCD_05,             0xC2,    "IO_LCD_05"},
    {IO_ID_BUMPER_05,          0xC1,    "IO_BUMPER_05"},
    {IO_ID_SOUND_05,           0xC3,    "IO_SOUND_05"},
    {IO_ID_ENV_05,             0xCF,    "IO_ENV_DECTECTOR_05"},
    {IO_ID_SERVO_05,           0xCB,    "IO_SERVO_05"},
    {IO_ID_ALIM_05,            0xCD,    "IO_ALIM_05"},
    {IO_ID_TESLA_05,           0xC4,    "IO_TESLA_05"},
    {IO_ID_SKITTLE_DETECTOR_05,0xC5,    "IO_SKITTLE_DETECTOR_05"},
    {IO_ID_CRANE_05,           0xC6,    "IO_CRANE_05"}
};
