/*******************************************************************************
 *
 *  name: upd-layman.ino
 *  date: Dec 30, 2019
 *  auth: ardnew
 *  desc:
 *
 ******************************************************************************/

// ----------------------------------------------------------------- includes --

#include <Wire.h>

#include <Adafruit_INA260.h>

#include <LayoutManager.h>
#include <STUSB4500.h>

#include "src/global.h"
#include "src/PowerMeter.h"

// ------------------------------------------------------------------ defines --

#define COLOR_PANEL  COLOR_NAVY
#define COLOR_TEXT   COLOR_CYAN
#define COLOR_HILITE COLOR_WHITE

// --------------------------------------------------------- private typedefs --


// -------------------------------------------------------- private variables --

void initGPIO(void);
void initPeripherals(void);

void usbpdCableAttached(void);
void usbpdCableDetached(void);
void usbpdCapabilitiesReceived(void);

void updateCableField(void);

// ---- VOLTAGE/CURRENT SENSOR ----

PowerMeter *meter;

// ---- USB PD sink controller ----

STUSB4500 *usbpd;

// ---- TFT DISPLAY ----

LayoutManager *man;

Panel *commandPanel;
Field *resetField;
Field *setPowerField;

Panel *srcCapPanel;
Field *srcCapField[NVM_SRC_PDO_MAX];

Panel *usbDataPanel;

Panel *powerPanel;
Field *powerField;

Panel *cablePanel;
Field *cableField;

// screen size
static uint16_t const SCREEN_WIDTH  = 320;
static uint16_t const SCREEN_HEIGHT = 240;
static uint8_t  const PANEL_MARGIN  = 6;
static uint8_t  const PANEL_RADIUS  = 6;

void setup()
{
  initGPIO();
  initPeripherals();

  meter = new PowerMeter();
  if (!meter->deviceReady()) {
    Serial.printf("failed to initialize power sensor\n");
    while (1) { delay(1000); }
  }

  usbpd = new STUSB4500(USBPD_RST_PIN);
  usbpd->setCableAttached(usbpdCableAttached);
  usbpd->setCableDetached(usbpdCableDetached);
  usbpd->setSourceCapabilitiesReceived(usbpdCapabilitiesReceived);

  man = new LayoutManager(
      TFT_CS_PIN, TFT_DC_PIN, TOUCH_CS_PIN, TOUCH_IRQ_PIN,
      SCREEN_WIDTH, SCREEN_HEIGHT, Orientation::Landscape, COLOR_BLACK);

  commandPanel = man->addPanel(0,
      PANEL_MARGIN,
      SCREEN_HEIGHT - PANEL_MARGIN - 46,
      SCREEN_WIDTH - 2 * PANEL_MARGIN,
      46,
      PANEL_RADIUS, COLOR_BLACK
  );

  commandPanel->setMargin(0);
  commandPanel->setPadding(8);

  resetField = man->addField(commandPanel,
      "Reset",
      2,
      COLOR_TEXT, COLOR_PANEL,
      PANEL_RADIUS,
      COLOR_PANEL, COLOR_TEXT,
      PANEL_RADIUS,
      0,
      COLOR_TEXT, COLOR_PANEL
  );

  setPowerField = man->addField(commandPanel,
      "Set Power",
      2,
      COLOR_TEXT, COLOR_PANEL,
      PANEL_RADIUS,
      COLOR_PANEL, COLOR_TEXT,
      PANEL_RADIUS,
      0,
      COLOR_TEXT, COLOR_PANEL
  );

  srcCapPanel = man->addPanel(0,
      PANEL_MARGIN,
      PANEL_MARGIN,
      SCREEN_WIDTH - 2 * PANEL_MARGIN,
      72,
      PANEL_RADIUS, COLOR_BLACK, PANEL_RADIUS, 0, COLOR_TEXT
  );

  srcCapPanel->setMargin(8);
  srcCapPanel->setPadding(4);

  usbDataPanel = man->addPanel(0,
      SCREEN_WIDTH - PANEL_MARGIN - 110,
      srcCapPanel->frame()->bottom() + PANEL_MARGIN,
      110,
      commandPanel->frame()->top() - srcCapPanel->frame()->bottom() - 2 * PANEL_MARGIN,
      PANEL_RADIUS, COLOR_BLACK, PANEL_RADIUS, 0, COLOR_TEXT
  );

  powerPanel = man->addPanel(0,
      PANEL_MARGIN,
      srcCapPanel->frame()->bottom() + PANEL_MARGIN + 28,
      SCREEN_WIDTH - usbDataPanel->frame()->size().width() - 3 * PANEL_MARGIN,
      32,
      PANEL_RADIUS, COLOR_TEXT
  );

  powerPanel->setMargin(0);
  powerPanel->setPadding(0);

  powerField = man->addField(powerPanel,
      "--",
      3,
      COLOR_HILITE,
      PANEL_RADIUS,
      COLOR_BLACK//, PANEL_RADIUS, 0, COLOR_PANEL
  );

  cablePanel = man->addPanel(0,
      PANEL_MARGIN,
      powerPanel->frame()->bottom() + PANEL_MARGIN,
      powerPanel->frame()->size().width(),
      12,
      PANEL_RADIUS, COLOR_BLACK
  );

  cablePanel->setMargin(0);
  cablePanel->setPadding(0);

  cableField = man->addField(cablePanel,
      "--",
      1,
      COLOR_TEXT,
      PANEL_RADIUS,
      COLOR_BLACK//, PANEL_RADIUS, 0, COLOR_PANEL
  );

  if (man->begin()) {
    Serial.printf("ILI9341-Layout-Manager v%s\n", man->version());
  }
  else {
    Serial.printf("failed to initialize ILI9341-Layout-Manager\n");
    while (1) { delay(1000); }
  }

  if (usbpd->begin(USBPD_ALRT_PIN, USBPD_ATCH_PIN)) {
    Serial.printf("STUSB4500 v%s\n", usbpd->version());

    updateCableField();
  }
  else {
    Serial.printf("failed to initialize STUSB4500\n");
    while (1) { delay(1000); }
  }
}

void loop()
{
  uint32_t time = millis();

  usbpd->update();

  man->draw();

  if (meter->ready(time)) {
    static char powerBuf[64] = { '\0' };
    snprintf(powerBuf, 64, "%s %s",
        meter->voltageStr().c_str(), meter->currentStr().c_str());
    powerField->setText(powerBuf);
  }
}

// --------------------------------------------------------- private routines --

void initGPIO(void)
{
#if DEFINED_VAL(TFT_RST_PIN)
  // TFT reset is active low, i'm choosing to keep it high with a digital pin
  // instead of a fixed pullup so that we can actually reset it if needed.
  pinMode(TFT_RST_PIN, OUTPUT);
  digitalWrite(TFT_RST_PIN, LOW);
  delay(100);
  digitalWrite(TFT_RST_PIN, HIGH);
#endif
}

void initPeripherals(void)
{
#if defined(WAIT_FOR_SERIAL)
  while (!Serial) { continue; }
#else
  while (!Serial && millis() < 2000) { continue; }
#endif
  Serial.begin(SERIAL_BAUD_RATE_BPS);

  asm(".global _printf_float"); // enable float support in snprintf()
}

void usbpdCableAttached(void)
{
  Serial.println("attached");

  updateCableField();

  usbpd->setPowerDefaultUSB();
  usbpd->updateSinkCapabilities();
  usbpd->requestSourceCapabilities();
}

void usbpdCableDetached(void)
{
  Serial.println("detached");

  updateCableField();

  srcCapPanel->clearFields();
}

void usbpdCapabilitiesReceived(void)
{
  Serial.println("source capabilities received:");

  srcCapPanel->clearFields();

#define PDO_STR_LEN 16
  static char pdoStr[PDO_STR_LEN] = { '\0' };

  size_t n = usbpd->sourcePDOCount();
  for (size_t i = 0U; i < n; ++i) {
    PDO pdo = usbpd->sourcePDO(i);
    Serial.printf("  %u: %umV %umA\n",
        pdo.number, pdo.voltage_mV, pdo.current_mA);

    snprintf(pdoStr, PDO_STR_LEN, "%sV\n%sA",
        String((float)pdo.voltage_mV / 1000.0F, 1).c_str(),
        String((float)pdo.current_mA / 1000.0F, 1).c_str()
    );

    srcCapField[i] = man->addField(srcCapPanel,
        pdoStr,
        2,
        COLOR_TEXT, COLOR_PANEL,
        PANEL_RADIUS,
        COLOR_PANEL, COLOR_TEXT
    );
  }
#undef PDO_STR_LEN

  Serial.println("sink capabilities:");

  size_t m = usbpd->sinkPDOCount();
  for (size_t i = 0U; i < m; ++i) {
    PDO pdo = usbpd->sinkPDO(i);
    Serial.printf("  %u: %umV %umA\n",
        pdo.number, pdo.voltage_mV, pdo.current_mA);
  }
}

void updateCableField(void)
{
  CableStatus cable = usbpd->cableStatus();

  switch (cable) {
    case CableStatus::CC1Connected:
      cableField->setText("Connected (CC1)");
      cableField->setColorText(COLOR_CYAN);
      break;
    case CableStatus::CC2Connected:
      cableField->setText("Connected (CC2)");
      cableField->setColorText(COLOR_CYAN);
      break;
    default:
      cableField->setText("Not connected");
      cableField->setColorText(COLOR_ORANGE);
      break;
  }
}
