#include "UIManager.h"
#include "Animations.h"

static const MenuItem kMenu[] = {
  { "WiFi Scanner",   ICON_WIFI },
  { "Signal Monitor", ICON_SIGNAL },
  { "Channel Map",    ICON_CHANNEL },
  { "Network Info",   ICON_NETWORK },
  { "Device Tools",   ICON_TOOLS },
  { "Lab AP",         ICON_AP },
  { "Logs",           ICON_LOGS },
  { "Settings",       ICON_SETTINGS },
  { "About",          ICON_ABOUT },
};
static const ScreenId kTargets[] = {
  SCR_WIFI_SCANNER, SCR_SIGNAL, SCR_CHANNEL, SCR_NETINFO,
  SCR_TOOLS, SCR_LAB_AP, SCR_LOGS, SCR_SETTINGS, SCR_ABOUT
};
static const uint8_t kMenuCount = sizeof(kMenu) / sizeof(kMenu[0]);

void UIManager::begin(DisplayManager* dm, ButtonManager* bm, bool animations) {
  dm_ = dm; bm_ = bm; anim_ = animations;
  cur_ = SCR_HOME; sp_ = 0; menu_ = MenuState();
  GUI::updateMenu(menu_, kMenuCount, false);       // snap to start position
  startWipe();
  dirty_ = true;
}

void UIManager::startWipe() { wiping_ = anim_; wipeX_ = 0; }

// ---------------- navigation ----------------
void UIManager::navigate(ScreenId next) {
  if (next == cur_) { dirty_ = true; return; }
  if (sp_ >= UI_STACK_DEPTH) { DBG("UI: nav stack full"); return; }
  stack_[sp_++] = cur_;
  cur_ = next;
  DBG("UI: -> screen %u (depth %u)", (unsigned)cur_, (unsigned)sp_);
  startWipe(); dirty_ = true;
}

void UIManager::back() {
  if (sp_ == 0) return;                            // already at root
  cur_ = stack_[--sp_];
  DBG("UI: <- screen %u (depth %u)", (unsigned)cur_, (unsigned)sp_);
  startWipe(); dirty_ = true;
}

void UIManager::goHome() {
  if (cur_ == SCR_HOME && sp_ == 0) return;
  sp_ = 0; cur_ = SCR_HOME;
  DBG("UI: home");
  startWipe(); dirty_ = true;
}

void UIManager::showMessage(const char* title, const char* msg, uint32_t timeoutMs) {
  snprintf(msgTitle_, sizeof(msgTitle_), "%s", title);
  snprintf(msgBody_,  sizeof(msgBody_),  "%s", msg);
  msgTimeout_ = timeoutMs > 0;
  msgUntil_ = millis() + timeoutMs;
  navigate(SCR_MESSAGE);
  dirty_ = true;
}

// ---------------- input ----------------
// The current screen decides what each button means.
void UIManager::handleEvent(UiEvent ev) {
  if (ev == UiEvent::Home) { goHome(); return; }
  if (ev == UiEvent::Back) { back();   return; }

  switch (cur_) {
    case SCR_HOME:
      if (ev == UiEvent::Up)   { GUI::menuMove(menu_, kMenuCount, -1); dirty_ = true; }
      if (ev == UiEvent::Down) { GUI::menuMove(menu_, kMenuCount, +1); dirty_ = true; }
      if (ev == UiEvent::Ok)   { navigate(kTargets[menu_.selected]); }
      break;
    case SCR_MESSAGE:
      if (ev == UiEvent::Ok) back();               // OK dismisses the dialog
      break;
    default:
      break;                                       // placeholders/About: only BACK/HOME matter
  }
}

// ---------------- main tick ----------------
void UIManager::update() {
  bm_->update();
  UiEvent ev;
  while ((ev = bm_->next()) != UiEvent::None) handleEvent(ev);

  uint32_t now = millis();
  if (cur_ == SCR_MESSAGE && msgTimeout_ && (int32_t)(now - msgUntil_) >= 0) back();
  if (cur_ == SCR_ABOUT && now - lastTick_ >= 1000) { lastTick_ = now; dirty_ = true; }

  if (dirty_ && (now - lastFrame_) >= FRAME_MS) {
    lastFrame_ = now;
    dirty_ = false;
    if (cur_ == SCR_HOME && GUI::updateMenu(menu_, kMenuCount, anim_)) dirty_ = true;
    render();
    if (wiping_) {
      wipeX_ += WIPE_STEP_PX;
      dirty_ = true;
      if (wipeX_ >= SCREEN_W) wiping_ = false;     // one more clean frame follows
    }
  }
}

// ---------------- drawing ----------------
void UIManager::render() {
  GUI::clear();
  switch (cur_) {
    case SCR_HOME:    drawHome(); break;
    case SCR_ABOUT:   drawAbout(); break;
    case SCR_MESSAGE: GUI::drawDialog(msgTitle_, msgBody_, ICON_WARNING); break;
    default:          drawPlaceholder(); break;
  }
  if (wiping_) Anim::wipeOverlay(wipeX_);
  dm_->show();
}

void UIManager::drawHome() {
  GUI::drawMenu(kMenu, kMenuCount, menu_);         // list first...
  char ver[12];
  snprintf(ver, sizeof(ver), "v%s", RKG_FW_VERSION);
  GUI::drawHeader(RKG_DEVICE, ver);                // ...then header/footer cover the edges
  GUI::drawFooter("UP/DN", "OK:OPEN", nullptr);
}

void UIManager::drawAbout() {
  GUI::textCentered(64, 16, RKG_DEVICE, 2);
  GUI::textCentered(64, 35, RKG_BRAND);
  char buf[28];
  snprintf(buf, sizeof(buf), "Firmware v%s", RKG_FW_VERSION);
  GUI::textCentered(64, 44, buf);
  GUI::drawHeader("ABOUT");
  GUI::drawIcon(ICON_ABOUT, 118, 2);
  snprintf(buf, sizeof(buf), "HEAP %u", (unsigned)ESP.getFreeHeap());
  GUI::drawFooter(buf, nullptr, "BACK");
}

void UIManager::drawPlaceholder() {
  const char* title; const char* body; IconId icon;
  switch (cur_) {
    case SCR_WIFI_SCANNER: title = "WIFI SCANNER";   body = "Arrives in Phase 3 (Wi-Fi system)";    icon = ICON_WIFI; break;
    case SCR_SIGNAL:       title = "SIGNAL MONITOR"; body = "Arrives in Phase 3 (Wi-Fi system)";    icon = ICON_SIGNAL; break;
    case SCR_CHANNEL:      title = "CHANNEL MAP";    body = "Arrives in Phase 3 (Wi-Fi system)";    icon = ICON_CHANNEL; break;
    case SCR_NETINFO:      title = "NETWORK INFO";   body = "Arrives in Phase 4 (network tools)";   icon = ICON_NETWORK; break;
    case SCR_TOOLS:        title = "DEVICE TOOLS";   body = "Arrives in Phase 4 (network tools)";   icon = ICON_TOOLS; break;
    case SCR_LAB_AP:       title = "LAB AP";         body = "Arrives in Phase 5 (access point)";    icon = ICON_AP; break;
    case SCR_LOGS:         title = "LOGS";           body = "Arrives in Phase 7 (storage)";         icon = ICON_LOGS; break;
    case SCR_SETTINGS:     title = "SETTINGS";       body = "Arrives in Phase 7 (storage)";         icon = ICON_SETTINGS; break;
    default:               title = "UNKNOWN";        body = "Screen not implemented";               icon = ICON_WARNING; break;
  }
  GUI::textCentered(64, 17, "NOT INSTALLED YET");
  GUI::wrapText(4, 29, 120, body, 2);
  GUI::drawHeader(title);
  GUI::drawIcon(icon, 118, 2);
  GUI::drawFooter(nullptr, nullptr, "BACK");
}
