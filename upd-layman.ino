/*******************************************************************************
 *
 *  name: upd-layman.ino
 *  date: Dec 30, 2019
 *  auth: ardnew
 *  desc:
 *
 ******************************************************************************/

// ----------------------------------------------------------------- includes --

#include <LayoutManager.h>

#include "src/global.h"

// ------------------------------------------------------------------ defines --

#define COLOR_BORDER COLOR_NAVY
#define COLOR_ACCENT COLOR_CYAN
#define COLOR_HILITE COLOR_WHITE

// -------------------------------------------------------- private variables --

void initGPIO(void);
void initPeripherals(void);

LayoutManager *man;

Panel *commandPanel;
Field *resetField;
Field *setPowerField;

Panel *srcCapPanel;

Panel *snkCapPanel;

Panel *powerPanel;
Field *powerField;

Panel *cablePanel;
Field *cableField;

void setup()
{
  // screen size
  uint16_t width = 320;
  uint16_t height = 240;
  uint16_t margin = 6;
  uint16_t radius = 6;

  man = new LayoutManager(
      TFT_CS_PIN, TFT_DC_PIN, TOUCH_CS_PIN, TOUCH_IRQ_PIN,
      width, height, Orientation::Landscape, COLOR_BLACK);

  commandPanel = man->addPanel(0,
      margin,
      height - margin - 46,
      width - 2 * margin,
      46,
      radius, COLOR_BLACK
  );

  commandPanel->setMargin(0);
  commandPanel->setPadding(8);

  resetField = man->addField(commandPanel,
      "Reset",
      2,
      COLOR_ACCENT, COLOR_BORDER,
      radius,
      COLOR_BORDER, COLOR_ACCENT,
      radius,
      0,
      COLOR_ACCENT, COLOR_BORDER
  );

  setPowerField = man->addField(commandPanel,
      "Set Power",
      2,
      COLOR_ACCENT, COLOR_BORDER,
      radius,
      COLOR_BORDER, COLOR_ACCENT,
      radius,
      0,
      COLOR_ACCENT, COLOR_BORDER
  );

  srcCapPanel = man->addPanel(0,
      margin,
      margin,
      width - 2 * margin,
      72,
      radius, COLOR_BLACK, radius, 0, COLOR_ACCENT
  );

  snkCapPanel = man->addPanel(0,
      width - margin - 110,
      srcCapPanel->frame()->bottom() + margin,
      110,
      commandPanel->frame()->top() - srcCapPanel->frame()->bottom() - 2 * margin,
      radius, COLOR_BLACK, radius, 0, COLOR_ACCENT
  );

  powerPanel = man->addPanel(0,
      margin,
      srcCapPanel->frame()->bottom() + margin + 28,
      width - snkCapPanel->frame()->size().width() - 3 * margin,
      32,
      radius, COLOR_BLACK
  );

  powerPanel->setMargin(0);
  powerPanel->setPadding(0);

  powerField = man->addField(powerPanel,
      "20V 1.5A",
      3,
      COLOR_HILITE,
      radius,
      COLOR_BLACK//, radius, 0, COLOR_BORDER
  );

  cablePanel = man->addPanel(0,
      margin,
      powerPanel->frame()->bottom() + margin,
      powerPanel->frame()->size().width(),
      12,
      radius, COLOR_BLACK
  );

  cablePanel->setMargin(0);
  cablePanel->setPadding(0);

  cableField = man->addField(cablePanel,
      "CC1 Connected",
      1,
      COLOR_ACCENT,
      radius,
      COLOR_BLACK//, radius, 0, COLOR_BORDER
  );

  if (man->begin()) {
    INFO("ILI9341-Layout-Manager v%s", man->version());
  }
  else {
    BAIL("failed to initialize layout manager");
  }
}

void loop()
{
  man->draw();
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
  while (!Serial && millis() < 1000) { continue; }
#endif
  Serial.begin(SERIAL_BAUD);
}
