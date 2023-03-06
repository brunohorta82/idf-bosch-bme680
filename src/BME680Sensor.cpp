#include "BME680Sensor.hpp"
#include "BoschBme680Interface.hpp"

namespace Environment
{
    esp_err_t BME680Sensor::init()
    {
        esp_err_t result = ESP_OK;
        result = bme688Init(this);
        return result;
    }

    esp_err_t BME680Sensor::readAir()
    {
        return bme688Read();
    }
}
