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

/* nothing */

// ------------------------------------------------------------------ defines --

#define DEBUG

//#define WAIT_FOR_SERIAL
#define SERIAL_BAUD_RATE_BPS 115200

#define I2C_CLOCK_FREQ_HZ    400000

// --------------------------------------------------------------------- GPIO --

// TFT touchscreen
#define TFT_VCC_PIN    /* 3v3 */
#define TFT_GND_PIN    /* GND */
#define TFT_CS_PIN         10
#define TFT_RST_PIN    /* 3v3 */
#define TFT_DC_PIN         17
#define TFT_MOSI_PIN       25
#define TFT_SCK_PIN        24
#define TFT_LED_PIN    /* 3v3 */
#define TFT_MISO_PIN       23
#define TOUCH_CLK_PIN      24
#define TOUCH_CS_PIN        2
#define TOUCH_MOSI_PIN     25
#define TOUCH_MISO_PIN     23
#define TOUCH_IRQ_PIN       7

// voltage/current sensor
#define METER_SDA_PIN      21
#define METER_SCL_PIN      22

// USB PD controller
#define USBPD_GND_PIN  /* GND */
#define USBPD_SCL_PIN      22
#define USBPD_SDA_PIN      21
#define USBPD_RST_PIN      11
#define USBPD_VPP_PIN  /* 3v3 */
#define USBPD_VCC_PIN  /* GND */
#define USBPD_ATCH_PIN     12
#define USBPD_ALRT_PIN     13

#define OUTPUT_EN_PIN       9

// ------------------------------------------------------------------- macros --

#define EXPAND(x) x ## 1
#define DEFINED_VAL(x) 1 != EXPAND(x)

// ------------------------------------------------------- exported functions --

/* nothing */

#endif // __GLOBAL_H__