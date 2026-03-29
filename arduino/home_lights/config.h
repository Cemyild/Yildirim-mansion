#ifndef CONFIG_H
#define CONFIG_H

#include <avr/pgmspace.h>

// ================================================================
//  TOUCH CALIBRATION
//  Upload arduino/calibration/calibration.ino first to find
//  the correct values for YOUR specific shield.
// ================================================================
#define TS_LEFT    165
#define TS_RIGHT   941
#define TS_TOP     167
#define TS_BOTTOM  945
#define TS_PRESSURE_MIN 200
#define TS_PRESSURE_MAX 1000

// Touch pins - update these with values from calibration sketch!
// Most common for 2.4" shields: YP=A1, XM=A2, YM=7, XP=6
#define YP A1
#define XM A2
#define YM 7
#define XP 6

// ================================================================
//  COLOR THEME (RGB565)
// ================================================================
#define C_BG       0x10A2  // dark gray background
#define C_CARD     0x2945  // card background
#define C_HEADER   0x000F  // dark blue header
#define C_ON       0x2DC9  // green (light ON)
#define C_OFF      0x632C  // muted gray (light OFF)
#define C_TEXT     0xFFFF  // white text
#define C_DIM      0x8C51  // dimmed text
#define C_ACCENT   0x34DF  // blue accent
#define C_TIMER    0xFD20  // orange (timer)
#define C_DANGER   0xF800  // red
#define C_BORDER   0x3186  // subtle border

// ================================================================
//  SCREEN
// ================================================================
#define SCR_W 240
#define SCR_H 320

// ================================================================
//  NETWORK (ESP-01 via SoftwareSerial)
//  Set NETWORK_ENABLED to 1 when you have an ESP-01 connected
// ================================================================
#define NETWORK_ENABLED 0

#define ESP_RX   10
#define ESP_TX   11
#define ESP_BAUD 9600

#define WIFI_SSID   "YOUR_WIFI_SSID"
#define WIFI_PASS   "YOUR_WIFI_PASS"
#define SERVER_IP   "192.168.1.100"
#define SERVER_PORT 5000

// ================================================================
//  ROOM & LIGHT CONFIGURATION
//  Edit these to match your actual rooms and devices.
// ================================================================
#define NUM_ROOMS   4
#define MAX_LIGHTS  4   // max lights per room

// Room names (stored in flash via PROGMEM)
const char r0[] PROGMEM = "Living Room";
const char r1[] PROGMEM = "Bedroom";
const char r2[] PROGMEM = "Kitchen";
const char r3[] PROGMEM = "Bathroom";
const char* const roomNames[] PROGMEM = { r0, r1, r2, r3 };

// How many lights each room has
const uint8_t roomLightCount[NUM_ROOMS] PROGMEM = { 3, 2, 2, 1 };

// Light names per room (PROGMEM)
const char l00[] PROGMEM = "Ceiling";
const char l01[] PROGMEM = "Floor Lamp";
const char l02[] PROGMEM = "Wall Light";
const char l10[] PROGMEM = "Main Light";
const char l11[] PROGMEM = "Bed Lamp";
const char l20[] PROGMEM = "Main Light";
const char l21[] PROGMEM = "Counter";
const char l30[] PROGMEM = "Mirror Light";

const char* const lightNames[NUM_ROOMS][MAX_LIGHTS] PROGMEM = {
  { l00, l01, l02, NULL },
  { l10, l11, NULL, NULL },
  { l20, l21, NULL, NULL },
  { l30, NULL, NULL, NULL }
};

// Device IDs for bridge server API - match to your Tuya/eWeLink device IDs
const char d00[] PROGMEM = "dev_living_ceiling";
const char d01[] PROGMEM = "dev_living_floor";
const char d02[] PROGMEM = "dev_living_wall";
const char d10[] PROGMEM = "dev_bed_main";
const char d11[] PROGMEM = "dev_bed_lamp";
const char d20[] PROGMEM = "dev_kitchen_main";
const char d21[] PROGMEM = "dev_kitchen_counter";
const char d30[] PROGMEM = "dev_bath_mirror";

const char* const deviceIds[NUM_ROOMS][MAX_LIGHTS] PROGMEM = {
  { d00, d01, d02, NULL },
  { d10, d11, NULL, NULL },
  { d20, d21, NULL, NULL },
  { d30, NULL, NULL, NULL }
};

// ================================================================
//  TIMER
// ================================================================
#define MAX_TIMERS 8

// ================================================================
//  UI LAYOUT CONSTANTS
// ================================================================
// Home screen room grid
#define GRID_X1     8
#define GRID_X2     126
#define GRID_Y1     52
#define GRID_Y2     162
#define GRID_W      108
#define GRID_H      102
#define GRID_GAP    5

// Bottom bar
#define BAR_Y       272
#define BAR_H       48

// Room screen light rows
#define LIGHT_Y_START 52
#define LIGHT_ROW_H   58
#define TOGGLE_X      165
#define TOGGLE_W      65
#define TOGGLE_H      34

// Header
#define HDR_H       45

// Touch debounce (ms)
#define TOUCH_DEBOUNCE 300

#endif
