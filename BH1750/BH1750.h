/**
 * @file    : BH1750.h
 * @author  : Christian Roth.
 * @brief   : Implements the BH1750 Ambient Light Sensor for the ESP32S2.
 *            It takes notes for device specific timing.
 *            ToDo: Let user define which ESP32S2 I2C_Device_Port should get used (currently default = 0). 
 *            ToDo: ERROR detection and handnling.
 * 
 * @version : 0.1
 * @date    : 08.04.2023
 * 
 * @copyright Copyright (c) 2023
 */

#ifndef BH1750_H
#define BH1750_H

#include <stdint.h>
typedef float  f32_t;
typedef double f64_t;


#define BH1750_MODE_HIGH1   0U  //   1 lux resolution 
#define BH1750_MODE_HIGH2   1U  // 0.5 lux resolution
#define BH1750_MODE_LOW     3U  //   4 lux resolution


typedef struct
{
    uint8_t MODE;           // Resolution mode.

    uint8_t I2C_DeviceAddr; // 7bit i2c device address.

    uint8_t I2C_SDA_Pin;
    uint8_t I2C_SCL_Pin;
    
} sBH1750_t;


/**
 * @brief: Initialized the BH1750 device with the given configuration.
 * 
 * @param _this[in] : The configuration for the BH1750 device.
 * @param _this[out]: The BH1750 device handler used for other functions.
 */
void vBH1750_Init(sBH1750_t const* const _this);

/**
 * @brief: Measures the current ambient light and returns the current light in lux.
 *         Note: Each measurement will take 24ms for low resolution or
 *                180ms for high resolution for device specific reasons.
 * 
 * @param _this: The sBH1750_t device handler.
 * 
 * @return f32_t: The current ambient light in lux.
 */
f32_t f32BH1750_GetAmbientLight(sBH1750_t const* const _this);


#endif
