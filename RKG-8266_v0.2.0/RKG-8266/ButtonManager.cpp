#include "ButtonManager.h"

static const uint8_t kPins[BTN_COUNT] = { PIN_BTN_UP, PIN_BTN_BACK, PIN_BTN_DOWN, PIN_BTN_OK };

uint8_t ButtonManager::begin() {
  qHead_ = qTail_ = 0;
  for (uint8_t i = 0; i < BTN_COUNT; i++) pinMode(kPins[i], INPUT_PULLUP);
  delay(5);                                   // let pull-ups settle
  uint8_t held = 0;
  uint32_t now = millis();
  for (uint8_t i = 0; i < BTN_COUNT; i++) {
    bool pressed = (digitalRead(kPins[i]) == LOW);   // LOW = pressed
    // A button already held at boot is ignored until released (prevents event spam).
    b_[i] = Btn{ pressed, pressed, pressed, pressed, now, now, now };
    if (pressed) held |= (1 << i);
  }
  return held;
}

void ButtonManager::push(UiEvent e) {
  uint8_t n = (qHead_ + 1) % 8;
  if (n == qTail_) return;                    // queue full: drop (never crash)
  q_[qHead_] = e;
  qHead_ = n;
}

UiEvent ButtonManager::next() {
  if (qTail_ == qHead_) return UiEvent::None;
  UiEvent e = q_[qTail_];
  qTail_ = (qTail_ + 1) % 8;
  return e;
}

void ButtonManager::flush() { qHead_ = qTail_ = 0; }

void ButtonManager::update() {
  uint32_t now = millis();
  for (uint8_t i = 0; i < BTN_COUNT; i++) {
    Btn& b = b_[i];
    bool raw = (digitalRead(kPins[i]) == LOW);

    // Debounce: a change only counts after the pin stays the same for DEBOUNCE_MS.
    if (raw != b.lastRead) { b.lastRead = raw; b.changedAt = now; }
    if (raw != b.stable && (now - b.changedAt) >= DEBOUNCE_MS) {
      b.stable = raw;
      if (raw) {                                            // ---- pressed
        b.pressedAt = now;
        b.longFired = false;
        b.nextRepeat = now + REPEAT_DELAY_MS;
        if (!b.ignore) {
          if (i == BTN_UP)   push(UiEvent::Up);
          if (i == BTN_DOWN) push(UiEvent::Down);
          if (i == BTN_OK)   push(UiEvent::Ok);
        }
      } else {                                              // ---- released
        // BACK fires on release so a long press can be told apart from a short one.
        if (!b.ignore && i == BTN_BACK && !b.longFired) push(UiEvent::Back);
        b.ignore = false;
      }
    }

    // Held-button behaviour
    if (b.stable && !b.ignore) {
      if ((i == BTN_UP || i == BTN_DOWN) && (int32_t)(now - b.nextRepeat) >= 0) {
        push(i == BTN_UP ? UiEvent::Up : UiEvent::Down);
        b.nextRepeat = now + REPEAT_INTERVAL_MS;
      }
      if (i == BTN_BACK && !b.longFired && (now - b.pressedAt) >= LONG_PRESS_MS) {
        b.longFired = true;
        push(UiEvent::Home);
      }
    }
  }
}
