#pragma once
#include "BME680Sensor.hpp"
using namespace std;
namespace AirQuality
{
    esp_err_t bme688Init(BME680Sensor *environmentSensor);
    esp_err_t bme688Read();

}
