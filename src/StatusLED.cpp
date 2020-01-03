/*******************************************************************************
 *
 *  name: StatusLED.cpp
 *  date: Dec 13, 2019
 *  auth: andrew
 *  desc:
 *
 ******************************************************************************/

// ----------------------------------------------------------------- includes --

#include <Arduino.h>

#include "StatusLED.h"
#include "global.h"

// ---------------------------------------------------------- private defines --

#define PULSE_DELTA     0x08
#define PULSE_DIMMEST   0x20
#define PULSE_BRIGHTEST 0xFF

// ----------------------------------------------------------- private macros --

/* nothing */

// ------------------------------------------------------------ private types --

/* nothing */

// ------------------------------------------------------- exported variables --

/* nothing */

// -------------------------------------------------------- private variables --

/* nothing */

// ---------------------------------------------- private function prototypes --

inline uint8_t _colorClip(int16_t const c)
{
  if (c < 0)
    { return 0x00; }
  else if (c > 255)
    { return 0xFF; }
  else
    { return (uint8_t)c; }
}

// ------------------------------------------------------- exported functions --

StatusLED::StatusLED(
    uint16_t const dataPin,
    uint16_t const clockPin,
    StatusLEDMode const mode,
    bool const show,
    StatusRGB const color,
    uint8_t const bright,
    uint32_t const delay
):
  _dataPin(dataPin),
  _clockPin(clockPin),
  _mode(mode),
  _show(show),
  _color(color),
  _bright(bright),
  _delay(delay),
  _last(0U),
  _pulse(color),
  _wheel(color),
  _change(true),
  _chmod(true)
{
  FastLED.addLeds<APA102, DOTSTAR_DATA_PIN, DOTSTAR_CLOCK_PIN, DOTSTAR_RGB_ORDER>(&_led, 1);
  FastLED.setBrightness(bright);
}

void StatusLED::update(void)
{
  static int8_t  dir = -1;
  static uint8_t pos =  0;

  if (!_show)
    { return; }

  uint32_t curr = millis();
  if (curr - _last < _delay)
    { return; }
  _last = curr;

  StatusRGB color;
  switch (_mode) {
    default:
    case StatusLEDMode::Fixed:
      color = _color;
      break;

    case StatusLEDMode::Pulse:
      if (_chmod)
        { _pulse = _color; }
      dir = nextPulse(dir);
      color = _pulse;
      _change = true;
      break;

    case StatusLEDMode::Fabulous:
      if (0U != _chmod)
        { _wheel = _color; }
      pos = nextWheel(pos);
      color = _wheel;
      _change = true;
      break;
  }

  if (!_change)
    { return; }

  _led.setRGB(
      _colorClip(color.red),
      _colorClip(color.green),
      _colorClip(color.blue)
  );
  FastLED.show();
  _change = false;
  _chmod  = false;
}

void StatusLED::setShow(bool const show)
{
  if (show == _show)
    { return; }

  if (!show) {
    _led = CRGB::Black;
    FastLED.show();
  }

  _show   = show;
  _change = true;
  _chmod  = true;
  update();
}

void StatusLED::setColor(StatusRGB const color, uint8_t const bright)
{
  if ((StatusLEDMode::Fixed != _mode) || (color != _color)) {
    _mode   = StatusLEDMode::Fixed;
    _color  = color;
    _change = true;
    _chmod  = true;
  }
  _show = true;

  if (bright != _bright) {
    _bright = bright;
    FastLED.setBrightness(bright);
  }
  update();
}

void StatusLED::setPulse(StatusRGB const color, uint8_t const bright, uint32_t const delay)
{
  if ((StatusLEDMode::Pulse != _mode) || (color != _color)) {
    _mode   = StatusLEDMode::Pulse;
    _color  = color;
    _change = true;
    _chmod  = true;
  }
  _show = true;
  _delay = delay;

  if (bright != _bright) {
    _bright = bright;
    FastLED.setBrightness(bright);
  }
  update();
}

void StatusLED::setFabulous(uint8_t const bright, uint32_t const delay)
{
  if (StatusLEDMode::Fabulous != _mode) {
    _mode   = StatusLEDMode::Fabulous;
    _change = true;
    _chmod  = true;
  }
  _show = true;
  _delay = delay;

  if (bright != _bright) {
    _bright = bright;
    FastLED.setBrightness(bright);
  }
  update();
}

// -------------------------------------------------------- private functions --

int8_t StatusLED::nextPulse(int8_t const dir)
{
#define DIMMEST(c) \
  ((PULSE_DELTA >= (c)) || ((c) <= PULSE_DIMMEST))
#define DIMMEST_COLOR(c) \
    (DIMMEST((c).red) && DIMMEST((c).green) && DIMMEST((c).blue))

#define BRIGHTEST(c) \
    ((PULSE_DELTA <= (c)) || ((c) >= PULSE_BRIGHTEST))
#define BRIGHTEST_COLOR(c) \
    (BRIGHTEST((c).red) && BRIGHTEST((c).green) && BRIGHTEST((c).blue))

  if (dir < 0) { // fade
    _pulse.red   -= PULSE_DELTA;
    _pulse.green -= PULSE_DELTA;
    _pulse.blue  -= PULSE_DELTA;
    if (DIMMEST_COLOR(_pulse))
      { return 1; }
  }
  else { // brighten
    _pulse.red   += PULSE_DELTA;
    _pulse.green += PULSE_DELTA;
    _pulse.blue  += PULSE_DELTA;
    if (BRIGHTEST_COLOR(_pulse))
      { return -1; }
  }
  return dir;

#undef DIMMEST
#undef BRIGHTEST
}

uint8_t StatusLED::nextWheel(uint8_t const pos)
{
  uint8_t curr = pos;
  uint8_t next = pos + 1;

  curr = 0xFF - curr;
  if (curr < 0x55) {
    _wheel.red   = 0x03 * curr;
    _wheel.green = 0xFF - curr * 0x03;
    _wheel.blue  = 0x00;
  }
  else if (curr < 0xAA) {
    curr -= 0x55;
    _wheel.red   = 0xFF - curr * 0x03;
    _wheel.green = 0x00;
    _wheel.blue  = 0x03 * curr;
  }
  else {
    curr -= 0xAA;
    _wheel.red   = 0x00;
    _wheel.green = 0x03 * curr;
    _wheel.blue  = 0xFF - curr * 0x03;
  }
  return next;
}
