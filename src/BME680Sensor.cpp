#include "BME680Sensor.hpp"
#include "BoschBme680Interface.hpp"

namespace Environment
{
    esp_err_t BME680Sensor::init()
    {
        return bme688Init(this);
    }

    esp_err_t BME680Sensor::readAir()
    {
        return bme688Read();
    }
}
