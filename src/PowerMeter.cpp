/*******************************************************************************
 *
 *  name: ina260.cpp
 *  date: Dec 13, 2019
 *  auth: andrew
 *  desc:
 *
 ******************************************************************************/

// ----------------------------------------------------------------- includes --

#include <Arduino.h>

#include "PowerMeter.h"
#include "global.h"

// ---------------------------------------------------------- private defines --

#define FORMAT_BUF_SIZE   32
#define FORMAT_VAL_WIDTH  6

// ----------------------------------------------------------- private macros --

/* nothing */

// ------------------------------------------------------------ private types --

/* nothing */

// ------------------------------------------------------- exported variables --

/* nothing */

// -------------------------------------------------------- private variables --

/* nothing */

// ---------------------------------------------- private function prototypes --

/* nothing */

// ------------------------------------------------------- exported functions --

PowerMeter::PowerMeter(void):
  _ina260(Adafruit_INA260()),
  _voltage_mV(VOLTAGE_MIN_MV),
  _current_mA(CURRENT_MIN_MV),
  _time(0U),
  _deviceReady(false)
{
  init(false);
}

bool PowerMeter::init(bool reset)
{
  if (reset) {
    _ina260.reset();
  }

  _deviceReady = _ina260.begin();
  if (_deviceReady) {
    _ina260.setCurrentConversionTime(INA260_TIME_1_1_ms);
    _ina260.setVoltageConversionTime(INA260_TIME_1_1_ms);
    _ina260.setAveragingCount(INA260_COUNT_256);
    _ina260.setMode(INA260_MODE_TRIGGERED);
  }

  return _deviceReady;
}

bool PowerMeter::ready(uint32_t time)
{
  if (_deviceReady) {
    if (_ina260.conversionReady()) {
      _voltage_mV = (int32_t)_ina260.readBusVoltage();
      _current_mA = (int32_t)_ina260.readCurrent();
      _time = time;
      _ina260.setMode(INA260_MODE_TRIGGERED);
      return true;
    }
  }
  return false;
}

String PowerMeter::voltageStr() const
{
  float voltage;
  if (_voltage_mV < 0)
    { voltage = 0.0F; }
  else
    { voltage = (float)_voltage_mV; }

  String s;
  if (voltage < 1000U)
    { s = String(voltage, 0) + "mV"; }
  else
    { s = String(voltage / 1000.0F, 1) + "V"; }

  return s;
}

String PowerMeter::currentStr() const
{
  float current;
  if (_current_mA < 0)
    { current = 0.0F; }
  else
    { current = (float)_current_mA; }

  String s;
  if (current < 1000U)
    { s = String(current, 0) + "mA"; }
  else
    { s = String(current / 1000.0F, 3) + "A"; }

  return s;
}

// -------------------------------------------------------- private functions --

/* nothing */
