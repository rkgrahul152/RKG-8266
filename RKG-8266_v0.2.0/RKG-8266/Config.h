#pragma once
#include <Arduino.h>

// ---------- Identity ----------
#define RKG_BRAND       "RKG SYSTEM"
#define RKG_DEVICE      "RKG-8266"
#define RKG_FW_NAME     "RKG-8266 Firmware"
#define RKG_FW_VERSION  "0.2.0"

// ---------- Debug ----------
#define DEBUG_MODE 1            // 1 = Serial logs on, 0 = off
#if DEBUG_MODE
  #define DBG(fmt, ...) Serial.printf_P(PSTR(fmt "\n"), ##__VA_ARGS__)
#else
  #define DBG(fmt, ...) do {} while (0)
#endif
constexpr uint32_t SERIAL_BAUD = 115200;

// ---------- Pins (ONLY place GPIOs are defined) ----------
constexpr uint8_t PIN_OLED_SDA  = D2;   // GPIO4
constexpr uint8_t PIN_OLED_SCL  = D1;   // GPIO5
constexpr uint8_t PIN_BTN_UP    = D5;   // GPIO14  B1
constexpr uint8_t PIN_BTN_BACK  = D3;   // GPIO0   B2 (boot-strap pin!)
constexpr uint8_t PIN_BTN_DOWN  = D6;   // GPIO12  B3
constexpr uint8_t PIN_BTN_OK    = D7;   // GPIO13  B4
constexpr uint8_t PIN_STATUS_LED = LED_BUILTIN; // active LOW, used for OLED-failure blink

// ---------- Display ----------
constexpr uint8_t  SCREEN_W = 128;
constexpr uint8_t  SCREEN_H = 64;
constexpr uint8_t  OLED_ADDR_PRIMARY = 0x3C;
constexpr uint8_t  OLED_ADDR_ALT     = 0x3D;
constexpr uint32_t I2C_CLOCK_HZ = 400000;

// ---------- Layout (128x64) ----------
constexpr int16_t HEADER_H  = 12;   // header: y 0..11 (line at y=11)
constexpr int16_t LIST_Y    = 13;   // list area starts here
constexpr int16_t ROW_H     = 10;
constexpr int16_t LIST_ROWS = 4;    // 13..52
constexpr int16_t FOOTER_Y  = 53;   // footer line at y=53, text at y=56

// ---------- Buttons ----------
constexpr uint32_t DEBOUNCE_MS        = 25;
constexpr uint32_t LONG_PRESS_MS      = 700;   // BACK long = Home
constexpr uint32_t REPEAT_DELAY_MS    = 400;   // UP/DOWN auto-repeat start
constexpr uint32_t REPEAT_INTERVAL_MS = 130;

// ---------- UI ----------
constexpr uint32_t FRAME_MS        = 30;
constexpr int16_t  WIPE_STEP_PX    = 20;
constexpr uint8_t  UI_STACK_DEPTH  = 8;
constexpr bool     DEFAULT_BOOT_ANIM = true;
constexpr bool     DEFAULT_UI_ANIM   = true;

// ---------- Memory ----------
constexpr uint32_t LOW_HEAP_BYTES = 12000;
