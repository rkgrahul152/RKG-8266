#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Config.h"

enum IconId : uint8_t {
  ICON_WIFI, ICON_SIGNAL, ICON_CHANNEL, ICON_NETWORK, ICON_TOOLS, ICON_AP,
  ICON_LOGS, ICON_SETTINGS, ICON_ABOUT, ICON_BACK, ICON_WARNING, ICON_OK, ICON_NONE
};

struct MenuItem { const char* label; IconId icon; };

struct MenuState {
  uint8_t selected = 0;   // logical selection
  int16_t scrollPx = 0;   // animated list scroll (pixels)
  int16_t cursorPx = 0;   // animated cursor position inside the list (pixels)
};

namespace GUI {
  void begin(Adafruit_SSD1306* disp);
  Adafruit_SSD1306* gfx();
  void clear();

  // text
  void text(int16_t x, int16_t y, const char* s, uint8_t size = 1, uint16_t color = SSD1306_WHITE);
  void textCentered(int16_t cx, int16_t y, const char* s, uint8_t size = 1, uint16_t color = SSD1306_WHITE);
  void textRight(int16_t rx, int16_t y, const char* s, uint8_t size = 1, uint16_t color = SSD1306_WHITE);
  uint8_t wrapText(int16_t x, int16_t y, int16_t w, const char* s, uint8_t maxLines);

  // chrome (draw AFTER list content: they cover the top/bottom bands)
  void drawHeader(const char* title, const char* right = nullptr);
  void drawFooter(const char* left, const char* center, const char* right);

  // menu / scroll list
  void menuMove(MenuState& m, uint8_t count, int8_t dir);
  bool updateMenu(MenuState& m, uint8_t count, bool animate);   // true = still moving
  void drawMenu(const MenuItem* items, uint8_t count, const MenuState& m);

  // widgets
  void drawIcon(IconId id, int16_t x, int16_t y, uint16_t color = SSD1306_WHITE);  // 8x8
  void drawProgress(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t pct);
  void drawSignalBar(int16_t x, int16_t y, int16_t rssi);
  void drawStatus(int16_t x, int16_t y, bool on);
  void drawGraph(int16_t x, int16_t y, int16_t w, int16_t h,
                 const int8_t* data, uint8_t n, int8_t vmin, int8_t vmax);
  void drawDialog(const char* title, const char* msg, IconId icon = ICON_NONE);
  void drawConfirm(const char* title, const char* msg,
                   const char* label0, const char* label1, uint8_t selected);
}
