
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "../BH1750/BH1750.h"

#define BH1750_I2C_ADDR_H   0x5C    // ADDR pin HIGH
#define BH1750_I2C_ADDR_L   0x23    // ADDR pin LOW


void app_main(void)
{
	// Sensor configuration.
    sBH1750_t mySensor;
    mySensor.I2C_DeviceAddr = BH1750_I2C_ADDR_H;
    mySensor.I2C_SCL_Pin = 5U;
    mySensor.I2C_SDA_Pin = 6U;

    mySensor.MODE = BH1750_MODE_HIGH1;

    // Sensor initialization.
    vBH1750_Init(&mySensor);

    while(1)
    {
        printf("\n");

        printf("BH1750::  current ambient light: %.2f lux\n", f32BH1750_GetAmbientLight(&mySensor));

        vTaskDelay(100);
    }

    vTaskDelay(10000);
}