#pragma once
#include <Arduino.h>
#include "Config.h"
#include "DisplayManager.h"

enum BootResult : uint8_t { BOOT_OK, BOOT_INIT, BOOT_WARN, BOOT_FAIL, BOOT_SKIP };

struct BootReport {
  BootResult r[5];   // DISPLAY, BUTTONS, WIFI CORE, MEMORY, SYSTEM
};

namespace Anim {
  // ~3 s boot sequence. abortFn() returning true skips it.
  void bootSequence(DisplayManager* dm, const BootReport& rep, bool (*abortFn)());

  // Screen transition: hides everything right of x and draws a scan line.
  void wipeOverlay(int16_t x);

  // Small reusable animations (used from Phase 3 on)
  void drawDots(int16_t x, int16_t y, uint32_t ms);          // "connecting..."
  void drawSuccess(int16_t cx, int16_t cy, uint8_t pct);     // check mark drawing itself
  void drawAlert(int16_t cx, int16_t cy, uint32_t ms);       // blinking warning triangle
}
