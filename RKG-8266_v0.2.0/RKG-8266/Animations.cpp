#include "Animations.h"
#include "GUI.h"

static const char* const kCheckNames[5] = { "DISPLAY", "BUTTONS", "WIFI CORE", "MEMORY", "SYSTEM" };

static const char* resultText(BootResult r) {
  switch (r) {
    case BOOT_OK:   return "[ OK ]";
    case BOOT_INIT: return "[INIT]";   // initialised but not physically verifiable
    case BOOT_WARN: return "[WARN]";
    case BOOT_FAIL: return "[FAIL]";
    default:        return "[SKIP]";
  }
}

// "RKG" (size 3) + spaced "SYSTEM" (size 1) wordmark
static void drawWordmark(Adafruit_SSD1306* d, uint8_t bigChars, uint8_t smallChars) {
  static const char big[3] = { 'R', 'K', 'G' };
  static const char sys[6] = { 'S', 'Y', 'S', 'T', 'E', 'M' };
  d->setTextColor(SSD1306_WHITE);
  d->setTextSize(3);
  for (uint8_t i = 0; i < bigChars && i < 3; i++) { d->setCursor(37 + i * 18, 14); d->write(big[i]); }
  d->setTextSize(1);
  for (uint8_t i = 0; i < smallChars && i < 6; i++) { d->setCursor(39 + i * 9, 42); d->write(sys[i]); }
}

// Frame that grows from its four corners toward the middle. p = 0..100
static void drawFrame(Adafruit_SSD1306* d, uint8_t p) {
  const int16_t x = 28, y = 8, w = 72, h = 44;
  int16_t lx = (w / 2) * p / 100, ly = (h / 2) * p / 100;
  if (lx > 0) {
    d->drawFastHLine(x, y, lx, SSD1306_WHITE);           d->drawFastHLine(x + w - lx, y, lx, SSD1306_WHITE);
    d->drawFastHLine(x, y + h - 1, lx, SSD1306_WHITE);   d->drawFastHLine(x + w - lx, y + h - 1, lx, SSD1306_WHITE);
  }
  if (ly > 0) {
    d->drawFastVLine(x, y, ly, SSD1306_WHITE);           d->drawFastVLine(x + w - 1, y, ly, SSD1306_WHITE);
    d->drawFastVLine(x, y + h - ly, ly, SSD1306_WHITE);  d->drawFastVLine(x + w - 1, y + h - ly, ly, SSD1306_WHITE);
  }
}

// Glitch = shift a few 8-pixel-tall bands sideways, directly in the framebuffer.
static void glitch(Adafruit_SSD1306* d) {
  uint8_t* buf = d->getBuffer();
  uint8_t n = 1 + (uint8_t)random(3);
  for (uint8_t k = 0; k < n; k++) {
    uint8_t page = (uint8_t)random(8);
    int8_t shift = (int8_t)random(-8, 9);
    uint8_t* row = buf + page * SCREEN_W;
    if (shift > 0)      { memmove(row + shift, row, SCREEN_W - shift); memset(row, 0, shift); }
    else if (shift < 0) { int s = -shift; memmove(row, row + s, SCREEN_W - s); memset(row + SCREEN_W - s, 0, s); }
  }
}

static void drawChecks(const BootReport& rep, uint32_t local) {
  GUI::drawHeader(RKG_BRAND, "BOOT");
  for (uint8_t i = 0; i < 5; i++) {
    uint32_t rowT = i * 150UL;
    if (local < rowT) break;
    int16_t y = 14 + i * 9;
    GUI::text(2, y, kCheckNames[i]);
    GUI::textRight(126, y, (local - rowT >= 90) ? resultText(rep.r[i]) : "[ .. ]");
  }
}

static void drawReady(Adafruit_SSD1306* d, uint32_t local) {
  GUI::textCentered(64, 6, RKG_BRAND);
  d->drawFastHLine(20, 16, 88, SSD1306_WHITE);
  GUI::textCentered(64, 24, RKG_DEVICE, 2);
  int16_t len = (int32_t)88 * local / 300;
  if (len > 88) len = 88;
  if (len > 0) d->drawFastHLine(64 - len / 2, 44, len, SSD1306_WHITE);
  if (local > 200) GUI::textCentered(64, 49, "SYSTEM ONLINE");
}

namespace Anim {

void bootSequence(DisplayManager* dm, const BootReport& rep, bool (*abortFn)()) {
  Adafruit_SSD1306* d = dm->gfx();
  randomSeed(micros());
  const uint32_t t0 = millis();
  for (;;) {
    uint32_t t = millis() - t0;
    if (t >= 3000) break;
    if (abortFn && abortFn()) break;
    d->clearDisplay();

    if (t < 120) {                                   // stage 1: init mark
      int16_t s = 1 + t / 40;
      d->drawRect(64 - s, 32 - s, 2 * s, 2 * s, SSD1306_WHITE);
      d->drawPixel(64, 32, SSD1306_WHITE);
    } else if (t < 700) {                            // stage 2: R -> RK -> RKG -> SYSTEM
      uint8_t big = 1 + (t - 120) / 120; if (big > 3) big = 3;
      uint8_t small = (t < 450) ? 0 : 1 + (t - 450) / 40; if (small > 6) small = 6;
      drawWordmark(d, big, small);
    } else if (t < 950) {                            // stage 3: logo frame
      drawWordmark(d, 3, 6);
      drawFrame(d, (t - 700) * 100 / 250);
    } else if (t < 1350) {                           // stage 4: scan line
      drawWordmark(d, 3, 6);
      drawFrame(d, 100);
      int16_t y = 9 + (int32_t)(t - 950) * 42 / 400;
      d->drawFastHLine(29, y, 70, SSD1306_INVERSE);
    } else if (t < 1650) {                           // stage 5: glitch
      drawWordmark(d, 3, 6);
      drawFrame(d, 100);
      if (t < 1560) glitch(d);
    } else if (t < 2500) {                           // stage 6: system checks
      drawChecks(rep, t - 1650);
    } else {                                         // stage 7: ready
      drawReady(d, t - 2500);
    }
    dm->show();
    yield();
  }
}

void wipeOverlay(int16_t x) {
  Adafruit_SSD1306* d = GUI::gfx();
  if (x >= SCREEN_W) return;
  d->fillRect(x + 1, 0, SCREEN_W - x - 1, SCREEN_H, SSD1306_BLACK);
  d->drawFastVLine(x, 0, SCREEN_H, SSD1306_WHITE);
}

void drawDots(int16_t x, int16_t y, uint32_t ms) {
  Adafruit_SSD1306* d = GUI::gfx();
  uint8_t n = (ms / 300) % 4;
  for (uint8_t i = 0; i < n; i++) d->fillRect(x + i * 4, y, 2, 2, SSD1306_WHITE);
}

void drawSuccess(int16_t cx, int16_t cy, uint8_t pct) {
  Adafruit_SSD1306* d = GUI::gfx();
  d->drawCircle(cx, cy, 8, SSD1306_WHITE);
  if (pct > 40) d->drawLine(cx - 4, cy, cx - 1, cy + 3, SSD1306_WHITE);
  if (pct > 70) d->drawLine(cx - 1, cy + 3, cx + 4, cy - 3, SSD1306_WHITE);
}

void drawAlert(int16_t cx, int16_t cy, uint32_t ms) {
  if ((ms / 300) % 2) return;                        // blink
  GUI::drawIcon(ICON_WARNING, cx - 4, cy - 4);
}

}  // namespace Anim
