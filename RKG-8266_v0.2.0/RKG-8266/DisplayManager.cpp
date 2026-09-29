#include "DisplayManager.h"

DisplayManager::DisplayManager() : oled_(SCREEN_W, SCREEN_H, &Wire, -1) {}

// The Adafruit library does not reliably report a missing display,
// so we ask the I2C bus ourselves: "does anything answer at this address?"
bool DisplayManager::probe(uint8_t addr) {
  Wire.beginTransmission(addr);
  return Wire.endTransmission() == 0;
}

bool DisplayManager::begin() {
  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  Wire.setClock(I2C_CLOCK_HZ);

  uint8_t found = 0;
  if (probe(OLED_ADDR_PRIMARY)) found = OLED_ADDR_PRIMARY;
  else if (probe(OLED_ADDR_ALT)) found = OLED_ADDR_ALT;
  if (!found) { ok_ = false; return false; }

  if (!oled_.begin(SSD1306_SWITCHCAPVCC, found)) { ok_ = false; return false; }

  addr_ = found;
  ok_ = true;
  oled_.setTextWrap(false);
  oled_.setTextColor(SSD1306_WHITE);
  oled_.clearDisplay();
  oled_.display();
  return true;
}
