# Yildirim Mansion - Home Lights Control Panel

Physical touchscreen controller for smart home lights using Arduino UNO and a 2.4" TFT LCD Shield. Controls Tuya and eWeLink devices that are already set up in Google Home.

## Architecture

```
Arduino UNO + TFT Shield  <--Serial-->  ESP-01 WiFi  <--HTTP-->  Bridge Server (Python)
     (Touchscreen UI)                  (SoftwareSerial)           (controls Tuya/eWeLink)
```

## Hardware Required

| Component | Description |
|-----------|-------------|
| Arduino UNO | Main microcontroller |
| 2.4" TFT LCD Shield | ILI9341 driver, 240x320, resistive touch |
| ESP-01 (ESP8266) | WiFi module (optional - UI works without it) |
| 3.3V regulator | To power ESP-01 from Arduino 5V |
| Level shifter | 5V <-> 3.3V for serial communication (or resistor divider) |
| PC / Raspberry Pi | To run the bridge server |

## Wiring (ESP-01 to Arduino)

| ESP-01 Pin | Connect To |
|------------|------------|
| VCC | 3.3V (use regulator, NOT Arduino 3.3V pin - insufficient current) |
| GND | GND |
| TX | Arduino D10 (ESP_RX) |
| RX | Arduino D11 (ESP_TX) via voltage divider (5V -> 3.3V) |
| CH_PD | 3.3V (pull high) |

## Project Structure

```
arduino/home_lights/
  config.h          - Pin config, colors, room/light setup, WiFi credentials
  home_lights.ino   - Main sketch (setup, loop, state management)
  ui.ino            - Screen drawing (home, room, timer screens)
  touch.ino         - Touchscreen input handling
  network.ino       - ESP-01 AT command WiFi communication
  timers.ino        - Countdown timer management
server/
  bridge.py         - Flask REST API server
  config_example.json - Example device configuration
  requirements.txt  - Python dependencies
```

## Quick Start

### 1. Arduino Setup (Standalone - no WiFi needed)

1. Install Arduino IDE
2. Install libraries via Library Manager (Sketch > Include Library > Manage Libraries):
   - Search **MCUFRIEND_kbv** and install
   - Search **Adafruit GFX Library** and install
   - Search **Adafruit TouchScreen** and install (the header is `TouchScreen.h` but the library name in the manager is "Adafruit TouchScreen")
3. Open `arduino/home_lights/home_lights.ino`
4. Upload to Arduino UNO
5. The touchscreen UI will work immediately (toggles on-screen state only)

### 2. Touch Calibration

The touch coordinates vary per shield unit. A calibration sketch is included:

1. Open `arduino/calibration/calibration.ino` and upload it
2. Touch the screen in various spots - raw X, Y, Z values will display
3. Touch all 4 corners of the screen (at least 20 touches total)
4. Open **Serial Monitor** (9600 baud) - calibration values will be printed
5. Copy the values into `config.h`:
   ```cpp
   #define TS_LEFT    <your_value>
   #define TS_RIGHT   <your_value>
   #define TS_TOP     <your_value>
   #define TS_BOTTOM  <your_value>
   ```
6. If touch feels mirrored/rotated, swap `tp.x`/`tp.y` in `touch.ino`
7. If no touch is detected at all, try different pin combinations in `calibration.ino` (Options A/B/C are listed in the file)

### 3. Customize Rooms & Lights

Edit `config.h` to match your home:

```cpp
const char r0[] PROGMEM = "Living Room";  // rename rooms
const char r1[] PROGMEM = "Bedroom";

const uint8_t roomLightCount[NUM_ROOMS] PROGMEM = { 3, 2, 2, 1 }; // lights per room

const char l00[] PROGMEM = "Ceiling";     // rename lights
const char l01[] PROGMEM = "Floor Lamp";
```

### 4. Bridge Server (for actual device control)

```bash
cd server
cp config_example.json config.json
# Edit config.json with your Tuya device IDs and local keys
pip install -r requirements.txt
python bridge.py
```

### 5. Enable WiFi on Arduino

1. Wire ESP-01 module (see wiring table above)
2. In `config.h`, set:
   ```cpp
   #define NETWORK_ENABLED 1
   #define WIFI_SSID "YourWiFiName"
   #define WIFI_PASS "YourWiFiPassword"
   #define SERVER_IP "192.168.1.100"  // IP of the PC/RPi running bridge.py
   ```
3. Re-upload sketch

## Getting Tuya Device Keys

To control Tuya devices locally (without cloud), you need each device's local key:

1. Install tinytuya: `pip install tinytuya`
2. Run the wizard: `python -m tinytuya wizard`
3. Follow prompts to link your Tuya developer account
4. Copy device IDs, IPs, and local keys into `server/config.json`

## UI Screens

- **Home Screen**: 2x2 grid of rooms with light status, ALL ON/OFF, Timers button
- **Room Screen**: Individual light toggles, tap light name to set timer
- **Timer Screen**: Set countdown timer (hours/minutes) to auto turn lights on/off
- **Timer List**: View and cancel active timers

## Features

- Touch-friendly UI with large buttons
- Toggle individual lights or all lights per room
- Countdown timers (e.g., "turn off bedroom in 30 minutes")
- Works standalone (UI demo) or connected (actual device control)
- Dark theme optimized for always-on display
- Memory-efficient design for Arduino UNO (2KB SRAM)
