/*******************************************************************************
 *
 *  name: StatusLED.h
 *  date: Dec 13, 2019
 *  auth: andrew
 *  desc:
 *
 ******************************************************************************/

#ifndef __STATUS_LED_H__
#define __STATUS_LED_H__

// ----------------------------------------------------------------- includes --

#include <FastLED.h>

#include "global.h"

// ------------------------------------------------------------------ defines --

// ------------------------------------------------------------------- macros --



// ----------------------------------------------------------- exported types --

class StatusRGB {
public:
  // 16-bit to support the pulse fading logic
  int16_t red;
  int16_t green;
  int16_t blue;

  StatusRGB(void):
    red(0), green(0), blue(0)
  {}
  StatusRGB(int16_t r, int16_t g, int16_t b):
    red(r), green(g), blue(b)
  {}
  StatusRGB(CRGB const &rgb):
    red(rgb.red), green(rgb.green), blue(rgb.blue)
  {}

  inline bool operator==(StatusRGB const &rgb) const
    { return (red == rgb.red) && (green == rgb.green) && (blue == rgb.blue); }
  inline bool operator!=(StatusRGB const &rgb) const
    { return (red != rgb.red) || (green != rgb.green) || (blue != rgb.blue); }
};

enum class StatusLEDMode {
  NONE = -1,
  Fixed,    // = 0
  Pulse,    // = 1
  Fabulous, // = 2
  COUNT     // = 3
};

class StatusLED
{
private:
  CRGB           _led;
  uint16_t       _dataPin;
  uint16_t       _clockPin;
  StatusLEDMode  _mode;   // the color change mode
  bool           _show;   // whether or not neopixel is showing
  StatusRGB      _color;  // current color of pixel
  uint8_t        _bright; // total brightness (0x00 - 0xFF)
  uint32_t       _delay;  // time to wait between color updates
  uint32_t       _last;   // time of last color update
  StatusRGB      _pulse;  // current pulse color
  StatusRGB      _wheel;  // current wheel color
  bool           _change; // flag indicating we need to call show()
  bool           _chmod;  // flag indicating we need to reset pulse/wheel

  int8_t nextPulse(int8_t const dir);
  uint8_t nextWheel(uint8_t const pos);

public:
  StatusLED(
      uint16_t const dataPin,
      uint16_t const clockPin,
      StatusLEDMode const mode,
      bool const show,
      StatusRGB const color,
      uint8_t const bright,
      uint32_t const delay
  );

  void update(void);

  void setShow(bool const show);
  void setColor(StatusRGB const color, uint8_t const bright);
  void setPulse(StatusRGB const color, uint8_t const bright, uint32_t const delay);
  void setFabulous(uint8_t const bright, uint32_t const delay);

};

#endif // __STATUS_LED_H__