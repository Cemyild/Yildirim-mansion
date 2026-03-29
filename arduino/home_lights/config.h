#ifndef CONFIG_H
#define CONFIG_H

#include <avr/pgmspace.h>

// ================================================================
//  TOUCH CALIBRATION (from calibration sketch)
// ================================================================
#define TS_LEFT    150
#define TS_RIGHT   950
#define TS_TOP     150
#define TS_BOTTOM  950
#define TS_PRESSURE_MIN 10
#define TS_PRESSURE_MAX 1000

// Touch pins (calibrated for this shield)
#define YP A1
#define XM A2
#define YM 7
#define XP 6

// ================================================================
//  COLOR THEME - Modern Dark
// ================================================================
#define C_BG       0x0841  // near-black background
#define C_CARD     0x18E3  // dark charcoal card
#define C_CARD_HI  0x2124  // lighter card (hover/active)
#define C_HEADER   0x0841  // same as bg (seamless)
#define C_ON       0xFE60  // warm amber/gold for ON
#define C_OFF      0x4208  // dark gray for OFF
#define C_TEXT     0xDEFB  // soft white
#define C_DIM      0x6B6D  // muted gray
#define C_ACCENT   0x2CFF  // soft blue accent
#define C_TIMER    0xFD20  // orange
#define C_DANGER   0xC000  // dark red
#define C_BORDER   0x2104  // subtle border
#define C_DIVIDER  0x1082  // thin divider line

// ================================================================
//  SCREEN
// ================================================================
#define SCR_W 240
#define SCR_H 320

// ================================================================
//  NETWORK (ESP-01 via SoftwareSerial)
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
// ================================================================
#define NUM_ROOMS   4
#define MAX_LIGHTS  4

const char r0[] PROGMEM = "Living Room";
const char r1[] PROGMEM = "Bedroom";
const char r2[] PROGMEM = "Kitchen";
const char r3[] PROGMEM = "Bathroom";
const char* const roomNames[] PROGMEM = { r0, r1, r2, r3 };

const uint8_t roomLightCount[NUM_ROOMS] PROGMEM = { 3, 2, 2, 1 };

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
#define HDR_H       42

// Home screen room grid
#define GRID_X1     8
#define GRID_X2     124
#define GRID_Y1     50
#define GRID_Y2     168
#define GRID_W      108
#define GRID_H      110

// Bottom bar
#define BAR_Y       280
#define BAR_H       40

// Room screen light rows
#define LIGHT_Y_START 50
#define LIGHT_ROW_H   56
#define TOGGLE_X      172
#define TOGGLE_W      58
#define TOGGLE_H      28

// Touch debounce (ms)
#define TOUCH_DEBOUNCE 300

#endif
