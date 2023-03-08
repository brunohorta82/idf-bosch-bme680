#pragma once
#include "I2C.hpp"
#include <cstring>
using namespace std;
using namespace Components;
namespace AirQuality
{
    class BME680Sensor
    {
    private:
        char buff[650];
        uint8_t numReadings{0};
        shared_ptr<I2CMaster> i2cMasterBus;

    public:
        BME680Sensor(shared_ptr<I2CMaster> i2cMasterBus)
        {
            this->i2cMasterBus = i2cMasterBus;
        }
        shared_ptr<I2CMaster> getBus()
        {
            return this->i2cMasterBus;
        }
        esp_err_t init();
        esp_err_t readAir();

        void appendResults(uint8_t measIndex, uint8_t gasIndex, uint8_t status, uint8_t resHeat, uint8_t idac, uint8_t gasWait,
                           float temperature, float pressure, float humidity, float gasResistance)
        {
            numReadings++;
            size_t buffSize = strlen(buff);
            snprintf(buff + buffSize, 650 - buffSize, "%3d, %d, %.2f, %.2f, %.2f, %.2f, 0x%x\n",
                     measIndex,
                     gasIndex,
                     temperature,
                     pressure,
                     humidity,
                     gasResistance,
                     status);
        }
        constexpr char *getBuffer() { return buff; }
        constexpr char getNumReadings() { return numReadings; }
        constexpr void resetBuffer()
        {
            numReadings = 0;
            strcpy(buff, "");
        }
    };
}
