#pragma once
#include <Arduino.h>
#include "Config.h"

enum class UiEvent : uint8_t { None, Up, Down, Ok, Back, Home };
enum ButtonId : uint8_t { BTN_UP, BTN_BACK, BTN_DOWN, BTN_OK, BTN_COUNT };

class ButtonManager {
 public:
  uint8_t begin();      // returns bitmask of buttons that were already LOW at start
  void update();        // call every loop
  UiEvent next();       // pop one event (UiEvent::None if empty)
  void flush();
 private:
  struct Btn {
    bool stable, lastRead, longFired, ignore;
    uint32_t changedAt, pressedAt, nextRepeat;
  };
  void push(UiEvent e);
  Btn b_[BTN_COUNT];
  UiEvent q_[8];
  uint8_t qHead_ = 0, qTail_ = 0;
};
