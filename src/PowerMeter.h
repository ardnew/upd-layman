/*******************************************************************************
 *
 *  name: PowerMeter.h
 *  date: Dec 13, 2019
 *  auth: andrew
 *  desc:
 *
 ******************************************************************************/

#ifndef __INA260_H__
#define __INA260_H__

// ----------------------------------------------------------------- includes --

#include <Adafruit_INA260.h>

#include "global.h"

// ------------------------------------------------------------------ defines --

#define VOLTAGE_ZERO_MAX_MV        100
#define VOLTAGE_VBUS_USB_MIN_MV   4200
#define VOLTAGE_VBUS_USB_MAX_MV   5800
#define VOLTAGE_VBUS_9V_MIN_MV    8200
#define VOLTAGE_VBUS_9V_MAX_MV    9800
#define VOLTAGE_VBUS_12V_MIN_MV  11200
#define VOLTAGE_VBUS_12V_MAX_MV  12800
#define VOLTAGE_VBUS_15V_MIN_MV  14000
#define VOLTAGE_VBUS_15V_MAX_MV  16000
#define VOLTAGE_VBUS_20V_MIN_MV  19000
#define VOLTAGE_VBUS_20V_MAX_MV  21000

#define VOLTAGE_MIN_MV               0
#define VOLTAGE_MAX_MV           25000
#define CURRENT_MIN_MV               0
#define CURRENT_MAX_MV            6000

// ------------------------------------------------------------------- macros --

/* nothing */

// ----------------------------------------------------------- exported types --

class PowerMeter
{
private:
  Adafruit_INA260 _ina260;
  int32_t _voltage_mV;
  int32_t _current_mA;
  uint32_t _time;
  bool _deviceReady;

public:
  PowerMeter(void);
  bool init(bool reset);
  bool ready(uint32_t time);
  int32_t voltage() const { return _voltage_mV; }
  int32_t current() const { return _current_mA; }
  bool deviceReady() const { return _deviceReady; }
  String voltageStr() const;
  String currentStr() const;
};

#endif // __INA260_H__