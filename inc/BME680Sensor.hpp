#pragma once
#include "I2C.hpp"
using namespace std;
using namespace Components;
namespace Environment
{
    class BME680Sensor
    {
    private:
        /*! Contains new_data, gasm_valid & heat_stab */
        uint8_t status{0};
        /*! The index of the heater profile used */
        uint8_t gasIndex{0};
        /*! Measurement index to track order */
        uint8_t measIndex{0};
        /*! Heater resistance */
        uint8_t resHeat{0};
        /*! Current DAC */
        uint8_t idac{0};
        /*! Gas wait period */
        uint8_t gasWait{0};
        /*! Temperature in degree celsius */
        float temperature{0};
        /*! Pressure in Pascal */
        float pressure{0};
        /*! Humidity in % relative humidity x1000 */
        float humidity{0};
        /*! Gas resistance in Ohms */
        float gasResistance{0};

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

        void setResults(bme68x_data data)
        {
            this->status = data.status;
            this->gasIndex = data.gas_index;
            this->measIndex = data.meas_index;
            this->resHeat = data.res_heat;
            this->idac = data.idac;
            this->gasWait = data.gas_wait;
            this->temperature = data.temperature;
            this->pressure = data.pressure;
            this->humidity = data.humidity;
            this->gasResistance = data.gas_resistance;
        }
        constexpr uint8_t getStatus() { return status; }
        constexpr uint8_t getGasIndex() { return gasIndex; }
        constexpr uint8_t getMeasIndex() { return measIndex; }
        constexpr uint8_t getResHeat() { return resHeat; }
        constexpr uint8_t getIdac() { return idac; }
        constexpr uint8_t getGasWait() { return gasWait; }
        constexpr float getTemperature() { return temperature; }
        constexpr float getPressure() { return pressure; }
        constexpr float getHumidity() { return humidity; }
        constexpr float getGasResistance() { return gasResistance; }
    };
}
