/*******************************************************************************
 *
 *  name: global.h
 *  date: Dec 30, 2019
 *  auth: ardnew
 *  desc:
 *
 ******************************************************************************/
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

// ----------------------------------------------------------------- includes --

#include <cstdarg>

// ------------------------------------------------------------------ defines --

#define DEBUG

#define WAIT_FOR_SERIAL
#define SERIAL_BAUD 115200

// --------------------------------------------------------------------- GPIO --

//#define GRAND_CENTRAL_M4
#define ITSY_BITSY_M4

// TFT Touchscreen GPIO pin definitions
#if defined(GRAND_CENTRAL_M4)
#define TFT_VCC_PIN    /* 3v3 */
#define TFT_GND_PIN    /* GND */
#define TFT_CS_PIN          7
#define TFT_RST_PIN    /* 3v3 */
#define TFT_DC_PIN          5
#define TFT_MOSI_PIN       51
#define TFT_SCK_PIN        52
#define TFT_LED_PIN    /* 3v3 */
#define TFT_MISO_PIN       50
#define TOUCH_CLK_PIN      52
#define TOUCH_CS_PIN        4
#define TOUCH_MOSI_PIN     51
#define TOUCH_MISO_PIN     50
#define TOUCH_IRQ_PIN       3
#elif defined(ITSY_BITSY_M4)
#define TFT_VCC_PIN    /* 3v3 */
#define TFT_GND_PIN    /* GND */
#define TFT_CS_PIN         10 // 16 // A2
#define TFT_RST_PIN    /* 3v3 */
#define TFT_DC_PIN         17 // A3
#define TFT_MOSI_PIN       25
#define TFT_SCK_PIN        24
#define TFT_LED_PIN    /* 3v3 */
#define TFT_MISO_PIN       23
#define TOUCH_CLK_PIN      24
#define TOUCH_CS_PIN        2
#define TOUCH_MOSI_PIN     25
#define TOUCH_MISO_PIN     23
#define TOUCH_IRQ_PIN       7
#endif

// ------------------------------------------------------------------- macros --

#define EXPAND(x) x ## 1
#define DEFINED_VAL(x) 1 != EXPAND(x)

#if defined(DEBUG)
#define __OUT(pre, fmt, ...) \
    Serial.printf(pre " %s(%u):\r\n" fmt "\r\n\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define __OUT(pre, fmt, ...) /**/
#endif

#define INFO(fmt, ...) __OUT("[ ]", fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) __OUT("[*]", fmt, ##__VA_ARGS__)
#define BAIL(fmt, ...) __OUT("[!]", fmt, ##__VA_ARGS__); while (1) { delay(1000); }

#endif // __GLOBAL_H__