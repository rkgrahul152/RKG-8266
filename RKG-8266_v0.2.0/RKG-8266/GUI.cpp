#include "GUI.h"

static Adafruit_SSD1306* d = nullptr;

namespace GUI {

void begin(Adafruit_SSD1306* disp) { d = disp; }
Adafruit_SSD1306* gfx() { return d; }
void clear() { d->clearDisplay(); }

// ---------------- text ----------------
void text(int16_t x, int16_t y, const char* s, uint8_t size, uint16_t color) {
  d->setTextSize(size);
  d->setTextColor(color);
  d->setCursor(x, y);
  d->print(s);
  d->setTextSize(1);
}
void textCentered(int16_t cx, int16_t y, const char* s, uint8_t size, uint16_t color) {
  int16_t w = strlen(s) * 6 * size;
  text(cx - w / 2, y, s, size, color);
}
void textRight(int16_t rx, int16_t y, const char* s, uint8_t size, uint16_t color) {
  int16_t w = strlen(s) * 6 * size;
  text(rx - w, y, s, size, color);
}

// Word-wraps into fixed 6px characters, no heap use. Returns lines drawn.
uint8_t wrapText(int16_t x, int16_t y, int16_t w, const char* s, uint8_t maxLines) {
  uint8_t cpl = w / 6;
  if (cpl > 23) cpl = 23;
  char line[24];
  uint8_t lines = 0;
  while (*s && lines < maxLines) {
    while (*s == ' ') s++;
    if (!*s) break;
    uint8_t len = 0, lastSpace = 0;
    while (s[len] && len < cpl) { if (s[len] == ' ') lastSpace = len; len++; }
    uint8_t brk = len;
    if (s[len] && s[len] != ' ' && lastSpace > 0) brk = lastSpace;
    memcpy(line, s, brk);
    line[brk] = 0;
    text(x, y + lines * 9, line);
    s += brk;
    lines++;
  }
  return lines;
}

// ---------------- chrome ----------------
void drawHeader(const char* title, const char* right) {
  d->fillRect(0, 0, SCREEN_W, HEADER_H, SSD1306_BLACK);
  text(2, 2, title);
  if (right) textRight(SCREEN_W - 2, 2, right);
  d->drawFastHLine(0, HEADER_H - 1, SCREEN_W, SSD1306_WHITE);
}

void drawFooter(const char* left, const char* center, const char* right) {
  d->fillRect(0, FOOTER_Y, SCREEN_W, SCREEN_H - FOOTER_Y, SSD1306_BLACK);
  d->drawFastHLine(0, FOOTER_Y, SCREEN_W, SSD1306_WHITE);
  if (left)   text(2, 56, left);
  if (center) textCentered(SCREEN_W / 2, 56, center);
  if (right)  textRight(SCREEN_W - 2, 56, right);
}

// ---------------- icons (8x8, drawn with primitives = zero RAM) ----------------
void drawIcon(IconId id, int16_t x, int16_t y, uint16_t c) {
  switch (id) {
    case ICON_WIFI:
      d->drawCircleHelper(x + 4, y + 7, 4, 0x3, c);
      d->drawCircleHelper(x + 4, y + 7, 2, 0x3, c);
      d->drawPixel(x + 4, y + 7, c);
      break;
    case ICON_SIGNAL:
      for (uint8_t i = 0; i < 4; i++) { int16_t h = 2 + 2 * i; d->fillRect(x + i * 2, y + 8 - h, 1, h, c); }
      break;
    case ICON_CHANNEL:
      d->fillRect(x, y + 3, 2, 5, c); d->fillRect(x + 3, y, 2, 8, c); d->fillRect(x + 6, y + 4, 2, 4, c);
      break;
    case ICON_NETWORK:
      d->drawCircle(x + 4, y + 4, 3, c);
      d->drawFastHLine(x + 1, y + 4, 7, c);
      d->drawFastVLine(x + 4, y + 1, 7, c);
      break;
    case ICON_TOOLS:
      d->drawLine(x + 1, y + 7, x + 5, y + 3, c);
      d->fillCircle(x + 5, y + 2, 2, c);
      break;
    case ICON_AP:
      d->fillCircle(x + 4, y + 3, 1, c);
      d->drawCircleHelper(x + 4, y + 3, 3, 0x9, c);
      d->drawCircleHelper(x + 4, y + 3, 3, 0x6, c);
      d->drawFastVLine(x + 4, y + 5, 3, c);
      break;
    case ICON_LOGS:
      d->drawRect(x + 1, y, 6, 8, c);
      d->drawFastHLine(x + 2, y + 2, 4, c); d->drawFastHLine(x + 2, y + 4, 4, c); d->drawFastHLine(x + 2, y + 6, 4, c);
      break;
    case ICON_SETTINGS:
      d->drawCircle(x + 4, y + 4, 2, c);
      d->drawFastVLine(x + 4, y, 2, c);     d->drawFastVLine(x + 4, y + 6, 2, c);
      d->drawFastHLine(x, y + 4, 2, c);     d->drawFastHLine(x + 6, y + 4, 2, c);
      d->drawPixel(x + 1, y + 1, c); d->drawPixel(x + 7, y + 1, c);
      d->drawPixel(x + 1, y + 7, c); d->drawPixel(x + 7, y + 7, c);
      break;
    case ICON_ABOUT:
      d->drawCircle(x + 4, y + 4, 4, c);
      d->drawPixel(x + 4, y + 2, c);
      d->drawFastVLine(x + 4, y + 4, 3, c);
      break;
    case ICON_BACK:
      d->drawFastHLine(x + 1, y + 4, 6, c);
      d->drawLine(x + 1, y + 4, x + 4, y + 1, c);
      d->drawLine(x + 1, y + 4, x + 4, y + 7, c);
      break;
    case ICON_WARNING:
      d->drawTriangle(x + 3, y, x, y + 7, x + 7, y + 7, c);
      d->drawFastVLine(x + 3, y + 2, 3, c);
      d->drawPixel(x + 3, y + 6, c);
      break;
    case ICON_OK:
      d->drawLine(x, y + 4, x + 2, y + 6, c);
      d->drawLine(x + 2, y + 6, x + 7, y + 1, c);
      break;
    default: break;
  }
}

// ---------------- menu / scroll list ----------------
void menuMove(MenuState& m, uint8_t count, int8_t dir) {
  if (!count) return;
  int16_t s = (int16_t)m.selected + dir;
  if (s < 0) s = count - 1;            // wrap around
  else if (s >= count) s = 0;
  m.selected = (uint8_t)s;
}

static bool ease(int16_t& v, int16_t target) {
  int16_t diff = target - v;
  if (!diff) return false;
  int16_t step = diff / 2;
  if (!step) step = (diff > 0) ? 1 : -1;
  v += step;
  return true;
}

bool updateMenu(MenuState& m, uint8_t count, bool animate) {
  if (m.selected >= count) m.selected = 0;
  int16_t top = (m.selected > 0) ? m.selected - 1 : 0;
  int16_t maxTop = (count > LIST_ROWS) ? count - LIST_ROWS : 0;
  if (top > maxTop) top = maxTop;
  int16_t tScroll = top * ROW_H;
  int16_t tCursor = m.selected * ROW_H;
  if (!animate) {
    bool changed = (m.scrollPx != tScroll) || (m.cursorPx != tCursor);
    m.scrollPx = tScroll; m.cursorPx = tCursor;
    return changed;
  }
  bool a = ease(m.scrollPx, tScroll);
  bool b = ease(m.cursorPx, tCursor);
  return a || b;
}

void drawMenu(const MenuItem* items, uint8_t count, const MenuState& m) {
  for (uint8_t i = 0; i < count; i++) {
    int16_t y = LIST_Y + i * ROW_H - m.scrollPx;
    if (y <= -ROW_H || y >= FOOTER_Y) continue;          // off-screen
    drawIcon(items[i].icon, 3, y + 1);
    text(16, y + 1, items[i].label);
  }
  // Selection cursor: an INVERSE rectangle flips whatever is under it,
  // so it works at any animated position without recolouring text.
  int16_t cy = LIST_Y + m.cursorPx - m.scrollPx;
  d->fillRect(0, cy, SCREEN_W - 6, ROW_H - 1, SSD1306_INVERSE);

  if (count > LIST_ROWS) {                               // scrollbar
    int16_t trackH = LIST_ROWS * ROW_H;
    for (int16_t yy = LIST_Y; yy < LIST_Y + trackH; yy += 2) d->drawPixel(126, yy, SSD1306_WHITE);
    int16_t thumbH = trackH * LIST_ROWS / count;
    if (thumbH < 6) thumbH = 6;
    int16_t thumbY = LIST_Y + (int32_t)(trackH - thumbH) * m.cursorPx / ((count - 1) * ROW_H);
    d->fillRect(125, thumbY, 3, thumbH, SSD1306_WHITE);
  }
}

// ---------------- widgets ----------------
void drawProgress(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t pct) {
  if (pct > 100) pct = 100;
  d->drawRect(x, y, w, h, SSD1306_WHITE);
  int16_t fill = (int32_t)(w - 4) * pct / 100;
  if (fill > 0) d->fillRect(x + 2, y + 2, fill, h - 4, SSD1306_WHITE);
}

void drawSignalBar(int16_t x, int16_t y, int16_t rssi) {
  uint8_t lvl = (rssi > -55) ? 4 : (rssi > -65) ? 3 : (rssi > -75) ? 2 : (rssi > -85) ? 1 : 0;
  for (uint8_t i = 0; i < 4; i++) {
    int16_t h = 2 + 2 * i;
    if (i < lvl) d->fillRect(x + i * 3, y + 8 - h, 2, h, SSD1306_WHITE);
    else         d->drawFastHLine(x + i * 3, y + 7, 2, SSD1306_WHITE);
  }
}

void drawStatus(int16_t x, int16_t y, bool on) {
  if (on) d->fillCircle(x, y, 2, SSD1306_WHITE);
  else    d->drawCircle(x, y, 2, SSD1306_WHITE);
}

void drawGraph(int16_t x, int16_t y, int16_t w, int16_t h,
               const int8_t* data, uint8_t n, int8_t vmin, int8_t vmax) {
  d->drawFastVLine(x, y, h, SSD1306_WHITE);
  d->drawFastHLine(x, y + h - 1, w, SSD1306_WHITE);
  if (n < 2 || vmax <= vmin) return;
  int16_t px = 0, py = 0;
  for (uint8_t i = 0; i < n; i++) {
    int16_t v = data[i];
    if (v < vmin) v = vmin;
    if (v > vmax) v = vmax;
    int16_t gx = x + 1 + (int32_t)i * (w - 2) / (n - 1);
    int16_t gy = y + h - 2 - (int32_t)(v - vmin) * (h - 3) / (vmax - vmin);
    if (i > 0) d->drawLine(px, py, gx, gy, SSD1306_WHITE);
    px = gx; py = gy;
  }
}

void drawDialog(const char* title, const char* msg, IconId icon) {
  d->fillRect(6, 8, 116, 48, SSD1306_BLACK);
  d->drawRect(6, 8, 116, 48, SSD1306_WHITE);
  d->fillRect(6, 8, 116, 11, SSD1306_WHITE);
  if (icon != ICON_NONE) drawIcon(icon, 9, 9, SSD1306_BLACK);
  text(icon != ICON_NONE ? 21 : 10, 10, title, 1, SSD1306_BLACK);
  wrapText(12, 23, 104, msg, 2);
  d->fillRect(44, 42, 40, 11, SSD1306_WHITE);
  textCentered(64, 44, "OK", 1, SSD1306_BLACK);
}

void drawConfirm(const char* title, const char* msg,
                 const char* label0, const char* label1, uint8_t selected) {
  d->fillRect(6, 8, 116, 48, SSD1306_BLACK);
  d->drawRect(6, 8, 116, 48, SSD1306_WHITE);
  d->fillRect(6, 8, 116, 11, SSD1306_WHITE);
  text(10, 10, title, 1, SSD1306_BLACK);
  wrapText(12, 23, 104, msg, 2);
  for (uint8_t i = 0; i < 2; i++) {
    int16_t bx = 12 + i * 55;
    const char* lb = i ? label1 : label0;
    if (i == selected) { d->fillRect(bx, 42, 50, 11, SSD1306_WHITE); textCentered(bx + 25, 44, lb, 1, SSD1306_BLACK); }
    else               { d->drawRect(bx, 42, 50, 11, SSD1306_WHITE); textCentered(bx + 25, 44, lb); }
  }
}

}  // namespace GUI
