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
#include "src/StatusLED.h"
#include "src/PowerMeter.h"

#include "src/font/DroidSansMonoDotted_8.h"
#include "src/font/DroidSansMonoDotted_12.h"
#include "src/font/DroidSansMonoDotted_14.h"

// ------------------------------------------------------------------ defines --

#define COLOR_PANEL  COLOR_NAVY
#define COLOR_TEXT   COLOR_CYAN
#define COLOR_HILITE COLOR_WHITE

#define RESET_DELAY_MS  200

// --------------------------------------------------------- private typedefs --


// -------------------------------------------------------- private variables --

static bool _outputEnable = false;
static bool _applyMode = false;

void initGPIO(void);
void initPeripherals(void);

void outputEnable(bool const enable);

void usbpdCableAttached(void);
void usbpdCableDetached(void);
void usbpdCapabilitiesReceived(void);

void updateCableField(void);
void updateCapabilityFields(void);

void outputEnablePress(const Frame &frame, const Touch &touch);
void resetApplyPress(const Frame &frame, const Touch &touch);
void capabilityPress(const Frame &frame, const Touch &touch);


// ---- status LED ----

StatusLED *led;

// ---- VOLTAGE/CURRENT SENSOR ----

PowerMeter *meter;

// ---- USB PD sink controller ----

STUSB4500 *usbpd;
PDO _requestedPDO;
PDO _selectedPDO;

// ---- TFT DISPLAY ----

LayoutManager *man;

Panel *commandPanel;
Field *outputEnableField;
Field *resetApplyField;

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

  led = new StatusLED(
      DOTSTAR_DATA_PIN,
      DOTSTAR_CLOCK_PIN,
      StatusLEDMode::Fabulous,
      true,
      StatusRGB(CRGB::Green),
      100,
      5
  );

  meter = new PowerMeter(1.0);
  if (!meter->deviceReady()) {
    Serial.printf("failed to initialize power sensor\n");
    while (1) { delay(1000); }
  }

  usbpd = new STUSB4500(USBPD_RST_PIN);
  usbpd->setMaxSourceCapabilityRequests(200);
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

  outputEnableField = man->addField(commandPanel,
      "--",
      1,
      &DroidSansMonoDotted12pt7b,
      COLOR_TEXT, COLOR_PANEL,
      PANEL_RADIUS,
      COLOR_PANEL, COLOR_TEXT,
      PANEL_RADIUS,
      0,
      COLOR_TEXT, COLOR_PANEL
  );
  outputEnableField->setTouchPress(outputEnablePress);

  resetApplyField = man->addField(commandPanel,
      "Reset",
      1,
      &DroidSansMonoDotted12pt7b,
      COLOR_TEXT, COLOR_PANEL,
      PANEL_RADIUS,
      COLOR_PANEL, COLOR_TEXT,
      PANEL_RADIUS,
      0,
      COLOR_TEXT, COLOR_PANEL
  );
  resetApplyField->setTouchPress(resetApplyPress);

  srcCapPanel = man->addPanel(0,
      PANEL_MARGIN,
      PANEL_MARGIN,
      SCREEN_WIDTH - 2 * PANEL_MARGIN,
      72,
      PANEL_RADIUS, COLOR_BLACK, PANEL_RADIUS, 0, COLOR_TEXT
  );
  srcCapPanel->setMargin(8);
  srcCapPanel->setPadding(4);
  srcCapPanel->setAllowsMultipleSelection(false);

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
      1,
      &DroidSansMonoDotted14pt7b,
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
  }
  else {
    Serial.printf("failed to initialize STUSB4500\n");
  }

  outputEnable(_outputEnable);
  updateCableField();
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

  led->update();
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

  pinMode(OUTPUT_EN_PIN, OUTPUT);
  digitalWrite(OUTPUT_EN_PIN, LOW); // make sure output is disabled on startup
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

void outputEnable(bool const enable)
{
  char *buttonText;
  if (enable) {
    buttonText = "Disable";
    digitalWrite(OUTPUT_EN_PIN, HIGH);
  }
  else {
    buttonText = "Enable";
    digitalWrite(OUTPUT_EN_PIN, LOW);
  }
  if (nullptr != outputEnableField) {
    outputEnableField->setText(buttonText);
  }
  _outputEnable = enable;
}

void usbpdCableAttached(void)
{
  Serial.println("attached");

  if (!usbpd->started()) {
    (void)usbpd->begin(USBPD_ALRT_PIN, USBPD_ATCH_PIN);
    delay(250);
  }

  updateCableField();

  if ((_selectedPDO.voltage_mV > 0U) && (_selectedPDO.current_mA > 0U))
    { usbpd->setPower(_selectedPDO.voltage_mV, _selectedPDO.current_mA); }
  else
    { usbpd->setPowerDefaultUSB(); }
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
  updateCapabilityFields();
}

void updateCableField(void)
{
  CableStatus cable = usbpd->cableStatus();

  if (nullptr != cableField) {
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
}

void updateCapabilityFields(void)
{
  Serial.println("requested capability:");

  _requestedPDO = usbpd->requestedPDO();
  Serial.printf("  %u: %umV %umA\n",
      _requestedPDO.number, _requestedPDO.voltage_mV, _requestedPDO.current_mA);

  Serial.println("source capabilities received:");

  srcCapPanel->clearFields();

#define PDO_STR_LEN 16
  static char pdoStr[PDO_STR_LEN] = { '\0' };

  size_t n = usbpd->sourcePDOCount();
  for (size_t i = 0U; i < n; ++i) {
    PDO pdo = usbpd->sourcePDO(i);
    Serial.printf("  %u: %umV %umA\n",
        pdo.number, pdo.voltage_mV, pdo.current_mA);

    snprintf(pdoStr, PDO_STR_LEN, "%.1fV\n%.1fA",
        pdo.voltage_mV / 1000.0F,
        pdo.current_mA / 1000.0F
    );

    if ((pdo == _requestedPDO) || (pdo == _selectedPDO)) {
      srcCapField[i] = man->addField(srcCapPanel,
          pdoStr,
          1,
          &DroidSansMonoDotted8pt7b,
          COLOR_PANEL, COLOR_PANEL,
          PANEL_RADIUS,
          COLOR_HILITE, COLOR_TEXT
      );
    }
    else {
      srcCapField[i] = man->addField(srcCapPanel,
          pdoStr,
          1,
          &DroidSansMonoDotted8pt7b,
          COLOR_TEXT, COLOR_PANEL,
          PANEL_RADIUS,
          COLOR_PANEL, COLOR_TEXT
      );
    }

    srcCapField[i]->setLineSpacing(2);
    srcCapField[i]->setIsMomentary(false);
    srcCapField[i]->setTouchPress(capabilityPress);
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

void outputEnablePress(const Frame &frame, const Touch &touch)
{
  outputEnable(!_outputEnable);
}

void resetApplyPress(const Frame &frame, const Touch &touch)
{
  if (_applyMode) {
    size_t n = usbpd->sourcePDOCount();
    for (size_t i = 0U; i < n; ++i) {
      if (srcCapField[i]->isSelected())
        { _selectedPDO = usbpd->sourcePDO(i); }
      srcCapField[i]->setIsSelected(false);
    }
    Serial.printf("SEL = (%u) %umV %umA\n",
        _selectedPDO.number, _selectedPDO.voltage_mV, _selectedPDO.current_mA);

    usbpd->setPower(_selectedPDO.voltage_mV, _selectedPDO.current_mA);

    updateCapabilityFields();

    resetApplyField->setText("Reset");
    _applyMode = false;
  }
  else {
    if (_outputEnable) {
      outputEnable(false);
      delay(RESET_DELAY_MS);
    }
    outputEnable(true);
  }
}

void capabilityPress(const Frame &frame, const Touch &touch)
{
  if (frame.isSelected()) {
    resetApplyField->setText("Apply");
    _applyMode = true;
  }
  else {
    resetApplyField->setText("Reset");
    _applyMode = false;
  }
}