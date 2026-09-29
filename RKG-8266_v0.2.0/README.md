# RKG-8266 — RKG SYSTEM Firmware v0.2.0 (Phase 1 + 2)

Handheld Wi-Fi lab / network-diagnostic device firmware for **NodeMCU ESP8266** + **SSD1306 128x64 OLED** + **4 buttons**.
This release = hardware foundation + GUI core (boot animation, main menu, navigation). Wi-Fi features start in Phase 3.

## What's in the zip
```
README.md
RKG-8266/            <- open THIS folder's .ino in Arduino IDE
  RKG-8266.ino       main sketch (setup/loop, boot flow, OLED-failure recovery)
  Config.h           ALL pins, timings, layout, version, DEBUG switch
  DisplayManager.*   OLED init (I2C probe 0x3C/0x3D), contrast/invert
  ButtonManager.*    debounce, auto-repeat (UP/DOWN), long-press BACK = Home
  GUI.*              header, footer, menu, icons, progress, signal bar, graph, dialogs, text wrap
  Animations.*       boot animation, wipe transition, dots/success/alert animations
  UIManager.*        screen state machine + navigation stack
```

## Wiring
| Part | Pin |
|---|---|
| OLED SDA | D2 (GPIO4) |
| OLED SCL | D1 (GPIO5) |
| OLED VCC / GND | 3.3V / GND |
| B1 UP | D5 (GPIO14) -> GND |
| B2 BACK | D3 (GPIO0) -> GND |
| B3 DOWN | D6 (GPIO12) -> GND |
| B4 OK | D7 (GPIO13) -> GND |

Buttons use internal pull-ups (no resistors). Pins are defined only in `Config.h`.

**B2 / GPIO0 warning:** do NOT hold B2 while powering on or pressing RST — it puts the ESP8266 into flash mode. Pressing it after boot is fine.

## One-time setup
1. Arduino IDE -> **File > Preferences** -> *Additional boards manager URLs*:
   `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
2. **Tools > Board > Boards Manager** -> install **esp8266 by ESP8266 Community** (3.x).
3. **Sketch > Include Library > Manage Libraries** -> install **Adafruit SSD1306** (click *Install all* to also get Adafruit GFX + BusIO).
4. If no COM port appears: install the CH340 or CP2102 USB driver. Use a *data* USB cable.

## Upload
1. Unzip. Keep the folder named `RKG-8266` (must match `RKG-8266.ino`).
2. Open `RKG-8266/RKG-8266.ino`.
3. **Tools** settings:
   - Board: **NodeMCU 1.0 (ESP-12E Module)**
   - Upload Speed: 115200 (921600 if stable)
   - CPU Frequency: 80 MHz
   - Flash Size: **4MB (FS:2MB OTA:~1019KB)**
   - Erase Flash: Only Sketch
   - Port: your board's COM port
4. Click **Verify** (check mark), then **Upload** (arrow).
5. Open **Tools > Serial Monitor** at **115200 baud**.

## How to use the device
| Button | Action |
|---|---|
| B1 (UP) | Move up (hold = auto-repeat) |
| B3 (DOWN) | Move down (hold = auto-repeat) |
| B4 (OK) | Open selected item / dismiss dialog |
| B2 (BACK) | Short press = back to previous screen, hold 0.7 s = Home |

Any button during the boot animation skips it. Menu items are placeholders until later phases (About is functional).

## Test checklist
- [ ] Boot animation plays (~3 s): RKG -> SYSTEM -> frame -> scan line -> glitch -> checks -> "SYSTEM ONLINE"
- [ ] Checks show DISPLAY OK, BUTTONS INIT, WIFI CORE SKIP, MEMORY OK, SYSTEM OK
- [ ] Menu wipes in; UP/DOWN scroll smoothly and wrap; scrollbar moves
- [ ] OK opens a screen; BACK returns to the same menu item; long BACK goes Home
- [ ] One press = one action
- [ ] Serial: "Display initialized", "System ready", heartbeat every 10 s with stable heap

## Troubleshooting
- **Upload: "Failed to connect"** -> different cable/port, lower upload speed, make sure B2/GPIO0 isn't stuck LOW.
- **Blank OLED + LED blinks 3x repeatedly** -> OLED not found on I2C: check SDA=D2, SCL=D1, 3.3V, GND (some modules order pins GND VCC SCL SDA).
- **Garbled/shifted display** -> may be an SH1106 (1.3") panel, not SSD1306.
- **Menu skips 2 items per press** -> raise `DEBOUNCE_MS` in `Config.h` (e.g. 35).
- **"Button held at boot" warning** -> a button reads LOW at power-up (short or stuck).
- **`Adafruit_SSD1306.h: No such file`** -> install the library (setup step 3).
- **`'D2' was not declared`** -> wrong board selected; choose NodeMCU 1.0.
- Set `DEBUG_MODE 0` in `Config.h` to silence Serial logs.

## Roadmap
Phase 3 Wi-Fi scanner/signal/channel map -> 4 network tools -> 5 Lab AP -> 6 web dashboard -> 7 storage -> 8 polish -> v1.0.0.
Scope: passive diagnostics and authorized lab networks only.
