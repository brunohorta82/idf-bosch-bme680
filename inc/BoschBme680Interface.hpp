#pragma once
#include "BME680Sensor.hpp"
#include "../driver/bme68x.h"
#include "I2C.hpp"
#include <memory>
using namespace std;
namespace Environment
{
    esp_err_t bme688Init(BME680Sensor *environmentSensor);
    esp_err_t bme688Read();

}
