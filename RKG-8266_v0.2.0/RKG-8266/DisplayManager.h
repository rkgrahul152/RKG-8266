#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Config.h"

class DisplayManager {
 public:
  DisplayManager();
  bool begin();                       // false = OLED not found
  void show() { if (ok_) oled_.display(); }
  void setInverted(bool inv) { oled_.invertDisplay(inv); }
  void setContrast(uint8_t c) { oled_.ssd1306_command(SSD1306_SETCONTRAST); oled_.ssd1306_command(c); }
  bool isOk() const { return ok_; }
  uint8_t address() const { return addr_; }
  Adafruit_SSD1306* gfx() { return &oled_; }
 private:
  bool probe(uint8_t addr);
  Adafruit_SSD1306 oled_;
  bool ok_ = false;
  uint8_t addr_ = 0;
};
