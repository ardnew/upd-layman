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

// voltage/current sensor
#define METER_SDA_PIN      62
#define METER_SCL_PIN      63

// USB PD controller
#define USBPD_GND_PIN  /* GND */
#define USBPD_SCL_PIN      63
#define USBPD_SDA_PIN      62
#define USBPD_RST_PIN      22
#define USBPD_VPP_PIN  /* 3v3 */
#define USBPD_VCC_PIN  /* GND */
#define USBPD_ATCH_PIN     24
#define USBPD_ALRT_PIN     26

#define OUTPUT_EN_PIN      23

// ------------------------------------------------------------------- macros --

#define EXPAND(x) x ## 1
#define DEFINED_VAL(x) 1 != EXPAND(x)

// ------------------------------------------------------- exported functions --

/* nothing */

#endif // __GLOBAL_H__