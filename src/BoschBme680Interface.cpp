
#include "BoschBme680Interface.hpp"
#include "BME680Sensor.hpp"
#include "../driver/bme68x.h"
#include "../driver/bme68x_defs.h"
#include <esp_log.h>
#include "I2C.hpp"
#include <memory>
#include <cstring>

using namespace std;
namespace Environment
{
    struct bme68x_dev bme688Device;
    struct bme68x_conf conf;
    struct bme68x_heatr_conf heatr_conf;

    BME680Sensor *_bme680Sensor;
    void boschDelayUs(uint32_t period, void *intf_ptr)
    {
        esp_rom_delay_us(period);
    }

    int8_t boschI2cRead(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
    {
        uint8_t dev_addr = *(uint8_t *)intf_ptr;
        try
        {
            _bme680Sensor->getBus()->syncWrite(I2CAddress(dev_addr), {reg_addr});
            vector<uint8_t> data = _bme680Sensor->getBus()->syncRead(I2CAddress(dev_addr), len);
            memcpy(reg_data, data.data(), len);
            return ESP_OK;
        }
        catch (const I2CException &e)
        {
            ESP_LOGI("TAG", "I2C Exception with error: %s (0x%X)", e.what(), e.error);
            ESP_LOGI("TAG", "Couldn't read sensor!");
            return ESP_FAIL;
        }
    }

    int8_t boschI2cWrite(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
    {
        uint8_t dev_addr = *(uint8_t *)intf_ptr;
        try
        {
            vector<uint8_t> data;
            data.push_back(reg_addr);
            for (int i = 0; i < len; i++)
            {
                data.push_back(reg_data[i]);
            }
            _bme680Sensor->getBus()->syncWrite(I2CAddress(dev_addr), data);
            return ESP_OK;
        }
        catch (const I2CException &e)
        {
            ESP_LOGI("TAG", "I2C Exception with error: %s (0x%X)", e.what(), e.error);
            ESP_LOGI("TAG", "Couldn't write sensor!");
            return ESP_FAIL;
        }
    }
    
    void bme68x_check_rslt(const char api_name[], int8_t rslt)
    {
        switch (rslt)
        {
        case BME68X_OK:

            /* Do nothing */
            break;
        case BME68X_E_NULL_PTR:
            printf("API name [%s]  Error [%d] : Null pointer\r\n", api_name, rslt);
            break;
        case BME68X_E_COM_FAIL:
            printf("API name [%s]  Error [%d] : Communication failure\r\n", api_name, rslt);
            break;
        case BME68X_E_INVALID_LENGTH:
            printf("API name [%s]  Error [%d] : Incorrect length parameter\r\n", api_name, rslt);
            break;
        case BME68X_E_DEV_NOT_FOUND:
            printf("API name [%s]  Error [%d] : Device not found\r\n", api_name, rslt);
            break;
        case BME68X_E_SELF_TEST:
            printf("API name [%s]  Error [%d] : Self test error\r\n", api_name, rslt);
            break;
        case BME68X_W_NO_NEW_DATA:
            printf("API name [%s]  Warning [%d] : No new data found\r\n", api_name, rslt);
            break;
        default:
            printf("API name [%s]  Error [%d] : Unknown error code\r\n", api_name, rslt);
            break;
        }
    }
    esp_err_t bme688Init(BME680Sensor *environmentSensor)
    {

        _bme680Sensor = environmentSensor;

        int8_t rslt;

        /* Heater temperature in degree Celsius */
        uint16_t temp_prof[10] = {200, 240, 280, 320, 360, 360, 320, 280, 240, 200};

        /* Heating duration in milliseconds */
        uint16_t dur_prof[10] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

        static uint8_t dev_addr{CONFIG_BME688_ADDRESS};
        bme688Device.read = boschI2cRead;
        bme688Device.write = boschI2cWrite;
        bme688Device.intf = BME68X_I2C_INTF;
        bme688Device.delay_us = boschDelayUs;
        bme688Device.intf_ptr = &dev_addr;
        bme688Device.amb_temp = 25;
        rslt = bme68x_init(&bme688Device);
        bme68x_check_rslt("bme68x_init", rslt);

        /* Check if rslt == BME68X_OK, report or handle if otherwise */
        rslt = bme68x_get_conf(&conf, &bme688Device);
        bme68x_check_rslt("bme68x_get_conf", rslt);

        /* Check if rslt == BME68X_OK, report or handle if otherwise */
        conf.filter = BME68X_FILTER_OFF;
        conf.odr = BME68X_ODR_NONE; /* This parameter defines the sleep duration after each profile */
        conf.os_hum = BME68X_OS_16X;
        conf.os_pres = BME68X_OS_1X;
        conf.os_temp = BME68X_OS_2X;
        rslt = bme68x_set_conf(&conf, &bme688Device);
        bme68x_check_rslt("bme68x_set_conf", rslt);

        heatr_conf.enable = BME68X_ENABLE;
        heatr_conf.heatr_temp_prof = temp_prof;
        heatr_conf.heatr_dur_prof = dur_prof;
        heatr_conf.profile_len = 10;
        rslt = bme68x_set_heatr_conf(BME68X_SEQUENTIAL_MODE, &heatr_conf, &bme688Device);
        bme68x_check_rslt("bme68x_set_heatr_conf", rslt);
        rslt = bme68x_set_op_mode(BME68X_SEQUENTIAL_MODE, &bme688Device);
        bme68x_check_rslt("bme68x_set_op_mode", rslt);
        return ESP_OK;
    }

    esp_err_t bme688Read()
    {
        int8_t result;
        struct bme68x_data data[3];
        uint32_t del_period;
        uint8_t n_fields;

        ESP_LOGD("BME688", "Temperature(deg C), Pressure(Pa), Humidity(%%)");

        /* Calculate delay period in microseconds */
        del_period = bme68x_get_meas_dur(BME68X_SEQUENTIAL_MODE, &conf, &bme688Device) + (heatr_conf.heatr_dur_prof[0] * 1000);
        bme688Device.delay_us(del_period, bme688Device.intf_ptr);

        result = bme68x_get_data(BME68X_SEQUENTIAL_MODE, data, &n_fields, &bme688Device);
        bme68x_check_rslt("bme68x_get_data", result);

        ESP_LOGD("BME688", "Nº Fields %d", n_fields);
        for (uint8_t i = 0; i < n_fields; i++)
        {
            ESP_LOGD("BME688", "Temperature(deg C)  Pressure(Pa) Humidity(%%)");
            ESP_LOGD("BME688", "%.2f                %.2f         %.2f          | Gas resistance: %.2f ohm  Status: 0x%x  Profile index: %d  Measurement index: %d",
                     data[i].temperature,
                     data[i].pressure,
                     data[i].humidity,
                     data[i].gas_resistance,
                     data[i].status,
                     data[i].gas_index,
                     data[i].meas_index);
            _bme680Sensor->setResults(data[i]);
        }
        return result;
    }

}