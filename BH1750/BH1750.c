/**
 * @file    : BH1750.c
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

#include "BH1750.h"
#include "driver/i2c.h"


#define BH1750_POWER_OFF                    0x00
#define BH1750_POWER_ON                     0x01
#define BH1750_RESET                        0x07
#define BH1750_MEASUREMENT_CONTINUOUSLY     0x10
#define BH1750_MEASUREMENT_SINGLE           0x20


/**
 *  Used for I2C data transmission.
 */
static int i2c_master_port;


/**
 * @brief: Requests a single read from the sensor.
 * 
 * @param _this: The sensor that should get used for measurement.
 */
static void vRequestMeasurement(sBH1750_t const* const _this);

/**
 * @brief: Returns the last measurement of the given sensor.
 *          Sets the sensor into power-on mode after readout.
 * 
 * @param _this: The sensor that should be used to measure.
 * 
 * @return uint16_t: Measurement of the single read.
 *                    Must be calculated into correct lux value.
 */
static uint16_t u16ReturnMeasurement(sBH1750_t const* const _this);



/* *** public functions *** */

void vBH1750_Init(sBH1750_t const* const _this)
{
    // Initialize I2C
    i2c_master_port = 0;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = _this->I2C_SDA_Pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = _this->I2C_SCL_Pin,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
        .clk_flags = 0
    };
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

f32_t f32BH1750_GetAmbientLight(sBH1750_t const* const _this)
{
    // Get lux measurement from sensor
    vRequestMeasurement(_this);
    uint16_t lux_measurement = u16ReturnMeasurement(_this);

    // Calculate correct lux value
    f32_t lux_value;
    if(_this->MODE == BH1750_MODE_HIGH2)
    {
        // Source: datasheet p.12
        lux_value = (((0.5F * (lux_measurement & 1U)) + (lux_measurement >> 1U)) / 1.2F);
    }
    else
    {
        // Source: datasheet p.7
        lux_value = (lux_measurement / 1.2F);
    }

    return lux_value;
}



/* *** private functions *** */

static void vRequestMeasurement(sBH1750_t const* const _this)
{
    // Start data transfer.
    i2c_cmd_handle_t i2c_cmd = i2c_cmd_link_create();

    // Measure one time with correct mode.
    i2c_master_start(i2c_cmd);
    i2c_master_write_byte(i2c_cmd, (_this->I2C_DeviceAddr << 1U) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_write_byte(i2c_cmd, (BH1750_MEASUREMENT_SINGLE | _this->MODE), I2C_MASTER_ACK); 
    i2c_master_stop(i2c_cmd);

    // End data transfer.
    i2c_master_cmd_begin(i2c_master_port, i2c_cmd, portMAX_DELAY);
    i2c_cmd_link_delete(i2c_cmd);
}

static uint16_t u16ReturnMeasurement(sBH1750_t const* const _this)
{
    // Note: The sensor needs different cooldown times, depending on the mode.
    if(_this->MODE == BH1750_MODE_LOW)
    {
        vTaskDelay(24U/portTICK_PERIOD_MS);
    }
    else
    {
        vTaskDelay(180U/portTICK_PERIOD_MS);
    }

    // Start data transfer
    i2c_cmd_handle_t i2c_cmd = i2c_cmd_link_create();

    // Read lux measurement
    uint16_t lux_measurement;
    i2c_master_start(i2c_cmd);
    i2c_master_write_byte(i2c_cmd, (_this->I2C_DeviceAddr << 1U) | I2C_MASTER_READ, I2C_MASTER_ACK);
    i2c_master_read(i2c_cmd, (uint8_t*)&lux_measurement, 2U, I2C_MASTER_LAST_NACK);
    i2c_master_stop(i2c_cmd);

    // End data transfer
    i2c_master_cmd_begin(i2c_master_port, i2c_cmd, portMAX_DELAY);
    i2c_cmd_link_delete(i2c_cmd);

    return lux_measurement;
}
