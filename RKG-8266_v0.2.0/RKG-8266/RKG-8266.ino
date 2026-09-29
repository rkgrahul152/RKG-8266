#include <Arduino.h>
#include "Config.h"
#include "DisplayManager.h"
#include "ButtonManager.h"
#include "GUI.h"
#include "Animations.h"
#include "UIManager.h"

DisplayManager display;
ButtonManager  buttons;
UIManager      ui;

static uint32_t lastBeat = 0;
static uint32_t minHeap = 0xFFFFFFFF;
static bool lowHeapWarned = false;

// Any button press skips the boot animation.
static bool bootAbort() {
  buttons.update();
  return buttons.next() != UiEvent::None;
}

// OLED missing: blink the on-board LED 3x, log to Serial, retry every cycle.
// The device never hangs permanently; if you reseat the wires it recovers.
static void displayFailureLoop() {
  while (true) {
    DBG("ERROR: OLED not found. Check SDA=D2, SCL=D1, 3.3V, GND (I2C 0x3C/0x3D).");
    for (uint8_t i = 0; i < 3; i++) {
      digitalWrite(PIN_STATUS_LED, LOW);  delay(150);
      digitalWrite(PIN_STATUS_LED, HIGH); delay(150);
    }
    delay(1000);
    if (display.begin()) return;
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(50);
  DBG("");
  DBG("=== %s | %s | fw v%s ===", RKG_BRAND, RKG_DEVICE, RKG_FW_VERSION);

  pinMode(PIN_STATUS_LED, OUTPUT);
  digitalWrite(PIN_STATUS_LED, HIGH);              // LED off

  uint8_t held = buttons.begin();
  DBG("Buttons initialized (held-at-boot mask 0x%02X)", held);

  if (!display.begin()) displayFailureLoop();
  DBG("Display initialized (I2C 0x%02X)", display.address());
  GUI::begin(display.gfx());

  uint32_t heap = ESP.getFreeHeap();
  BootReport rep;
  rep.r[0] = BOOT_OK;                                      // OLED answered on I2C
  rep.r[1] = held ? BOOT_WARN : BOOT_INIT;                 // pins configured (wiring can't be proven)
  rep.r[2] = BOOT_SKIP;                                    // Wi-Fi not started until Phase 3
  rep.r[3] = (heap >= LOW_HEAP_BYTES) ? BOOT_OK : BOOT_WARN;   // real heap check
  bool anyBad = (rep.r[1] == BOOT_WARN) || (rep.r[3] == BOOT_WARN);
  rep.r[4] = anyBad ? BOOT_WARN : BOOT_OK;

  if (DEFAULT_BOOT_ANIM) Anim::bootSequence(&display, rep, bootAbort);
  buttons.flush();

  ui.begin(&display, &buttons, DEFAULT_UI_ANIM);
  if (held) ui.showMessage("WARNING", "Button held at boot", 4000);

  DBG("System ready. Free heap: %u", (unsigned)heap);
}

void loop() {
  ui.update();                                     // buttons + state machine + drawing

  uint32_t heap = ESP.getFreeHeap();
  if (heap < minHeap) minHeap = heap;
  if (!lowHeapWarned && heap < LOW_HEAP_BYTES) {
    lowHeapWarned = true;
    DBG("WARN: low heap %u", (unsigned)heap);
    ui.showMessage("LOW MEMORY", "Free heap is low", 4000);
  }

#if DEBUG_MODE
  if (millis() - lastBeat >= 10000) {
    lastBeat = millis();
    DBG("[hb] heap=%u min=%u maxblock=%u uptime=%us",
        (unsigned)heap, (unsigned)minHeap,
        (unsigned)ESP.getMaxFreeBlockSize(), (unsigned)(millis() / 1000));
  }
#endif
}
