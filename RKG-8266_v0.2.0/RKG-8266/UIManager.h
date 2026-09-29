#pragma once
#include <Arduino.h>
#include "Config.h"
#include "DisplayManager.h"
#include "ButtonManager.h"
#include "GUI.h"

enum ScreenId : uint8_t {
  SCR_HOME = 0, SCR_WIFI_SCANNER, SCR_SIGNAL, SCR_CHANNEL, SCR_NETINFO,
  SCR_TOOLS, SCR_LAB_AP, SCR_LOGS, SCR_SETTINGS, SCR_ABOUT, SCR_MESSAGE
};

class UIManager {
 public:
  void begin(DisplayManager* dm, ButtonManager* bm, bool animations);
  void update();                                   // call every loop()
  void showMessage(const char* title, const char* msg, uint32_t timeoutMs);
  ScreenId current() const { return cur_; }

 private:
  void navigate(ScreenId next);    // open a screen (remembers where we came from)
  void back();                     // return to previous logical screen
  void goHome();
  void handleEvent(UiEvent ev);
  void startWipe();
  void render();
  void drawHome();
  void drawAbout();
  void drawPlaceholder();

  DisplayManager* dm_ = nullptr;
  ButtonManager*  bm_ = nullptr;
  bool anim_ = true;

  ScreenId cur_ = SCR_HOME;
  ScreenId stack_[UI_STACK_DEPTH];
  uint8_t  sp_ = 0;
  MenuState menu_;

  bool dirty_ = true;
  uint32_t lastFrame_ = 0, lastTick_ = 0;
  bool wiping_ = false;
  int16_t wipeX_ = 0;

  char msgTitle_[20] = "";
  char msgBody_[40]  = "";
  bool msgTimeout_ = false;
  uint32_t msgUntil_ = 0;
};
