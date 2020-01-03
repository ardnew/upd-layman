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

#define CONTINUOUS_CONVERSION

#define REFRESH_DEFAULT_MS  1000

#define FORMAT_BUF_SIZE       32
#define FORMAT_VAL_WIDTH       6

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
  _current_mA(CURRENT_MIN_MA),
  _time(0U),
  _refresh_ms(REFRESH_DEFAULT_MS),
  _deviceReady(false)
{
  init(false);
}

PowerMeter::PowerMeter(float const refresh_hz):
  _ina260(Adafruit_INA260()),
  _voltage_mV(VOLTAGE_MIN_MV),
  _current_mA(CURRENT_MIN_MA),
  _time(0U),
  _refresh_ms((uint32_t)(1.0F / refresh_hz * 1000.0F + 0.5F)),
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
#if defined(CONTINUOUS_CONVERSION)
    _ina260.setMode(INA260_MODE_CONTINUOUS);
#else
    _ina260.setCurrentConversionTime(INA260_TIME_8_244_ms);
    _ina260.setVoltageConversionTime(INA260_TIME_8_244_ms);
    _ina260.setAveragingCount(INA260_COUNT_16);
    _ina260.setMode(INA260_MODE_TRIGGERED);
#endif
  }

  return _deviceReady;
}

bool PowerMeter::ready(uint32_t time)
{
  if (_deviceReady) {
#if defined(CONTINUOUS_CONVERSION)
    if (time - _time > _refresh_ms) {
#else
    if (_ina260.conversionReady()) {
#endif
      Serial.printf("time=%lu, _time=%lu, _refresh_ms=%lu\n", time, _time, _refresh_ms);
      _voltage_mV = (int32_t)_ina260.readBusVoltage();
      _current_mA = (int32_t)_ina260.readCurrent();
      _time = time;
#if !defined(CONTINUOUS_CONVERSION)
      _ina260.setMode(INA260_MODE_TRIGGERED);
#endif
      return true;
    }
  }
  return false;
}

#define STR_LEN 16
String PowerMeter::voltageStr() const
{
  static char str[STR_LEN] = { '\0' };

  uint32_t voltage;
  if (_voltage_mV < 0)
    { voltage = 0U; }
  else
    { voltage = (uint32_t)_voltage_mV; }

  if (voltage < 1000U)
    { snprintf(str, STR_LEN, "%lumV", voltage); }
  else if (voltage <= VOLTAGE_MAX_MV)
    { snprintf(str, STR_LEN, "%4.1fV",
        (float)voltage / 1000.0F); }
  else
    { snprintf(str, STR_LEN, "(O/V)"); }

  return String(str);
}

String PowerMeter::currentStr() const
{
  static char str[STR_LEN] = { '\0' };

  uint32_t current;
  if (_current_mA < 0)
    { current = 0U; }
  else
    { current = (uint32_t)_current_mA; }

  if (current < 1000U)
    { snprintf(str, STR_LEN, "%lumA", current); }
  else if (current <= CURRENT_MAX_MA)
    { snprintf(str, STR_LEN, "%.2gA",
        (float)((uint32_t)(current / 10U)) / 100.0F); }
  else
    { snprintf(str, STR_LEN, "(O/C)"); }

  return String(str);
}
#undef STR_LEN

// -------------------------------------------------------- private functions --

/* nothing */
