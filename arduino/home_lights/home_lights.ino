// ================================================================
//  Yildirim Mansion - Home Lights Control Panel
//  Arduino UNO + 2.4" ILI9341 TFT Touch Shield
//
//  Libraries required (install via Arduino Library Manager):
//    - MCUFRIEND_kbv
//    - Adafruit GFX Library
//    - TouchScreen
// ================================================================

#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include "config.h"

#if NETWORK_ENABLED
#include <SoftwareSerial.h>
SoftwareSerial espSerial(ESP_RX, ESP_TX);
#endif

// ================================================================
//  GLOBAL STATE
// ================================================================

MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Screen navigation
enum Screen : uint8_t {
  SCR_HOME,
  SCR_ROOM,
  SCR_TIMER_SET,
  SCR_TIMER_LIST
};

Screen currentScreen = SCR_HOME;
uint8_t selectedRoom = 0;
uint8_t selectedLight = 0;

// Light states: 0=OFF, 1=ON
uint8_t lightState[NUM_ROOMS][MAX_LIGHTS];

// Timer data
struct Timer {
  uint8_t room;
  uint8_t light;
  uint8_t action;         // 0=turn OFF, 1=turn ON
  unsigned long triggerAt; // millis() timestamp when timer fires
  bool active;
};

Timer timers[MAX_TIMERS];
uint8_t timerCount = 0;

// Timer setup temporary values
uint8_t tmpTimerHours = 0;
uint8_t tmpTimerMins = 30;
uint8_t tmpTimerAction = 0;  // 0=OFF, 1=ON

// Touch debounce
unsigned long lastTouchTime = 0;

// Network
bool wifiConnected = false;

// Shared buffer for reading PROGMEM strings
char pgmBuf[20];

// ================================================================
//  HELPERS
// ================================================================

void readPgm(const char* pgmStr, char* buf, uint8_t maxLen) {
  strncpy_P(buf, pgmStr, maxLen - 1);
  buf[maxLen - 1] = '\0';
}

uint8_t getLightCount(uint8_t room) {
  return pgm_read_byte(&roomLightCount[room]);
}

const char* getRoomName(uint8_t room) {
  return (const char*)pgm_read_ptr(&roomNames[room]);
}

const char* getLightName(uint8_t room, uint8_t light) {
  return (const char*)pgm_read_ptr(&lightNames[room][light]);
}

const char* getDeviceId(uint8_t room, uint8_t light) {
  return (const char*)pgm_read_ptr(&deviceIds[room][light]);
}

// Count how many lights are ON in a room
uint8_t countOnLights(uint8_t room) {
  uint8_t count = 0;
  uint8_t total = getLightCount(room);
  for (uint8_t i = 0; i < total; i++) {
    if (lightState[room][i]) count++;
  }
  return count;
}

// Toggle a single light
void toggleLight(uint8_t room, uint8_t light) {
  lightState[room][light] = !lightState[room][light];
#if NETWORK_ENABLED
  sendToggle(room, light, lightState[room][light]);
#endif
}

// Set all lights in a room
void setRoomLights(uint8_t room, uint8_t state) {
  uint8_t total = getLightCount(room);
  for (uint8_t i = 0; i < total; i++) {
    if (lightState[room][i] != state) {
      lightState[room][i] = state;
#if NETWORK_ENABLED
      sendToggle(room, i, state);
#endif
    }
  }
}

// Set ALL lights in ALL rooms
void setAllLights(uint8_t state) {
  for (uint8_t r = 0; r < NUM_ROOMS; r++) {
    setRoomLights(r, state);
  }
}

// ================================================================
//  SETUP & LOOP
// ================================================================

void setup() {
  Serial.begin(9600);

  // Initialize display
  uint16_t id = tft.readID();
  if (id == 0xD3D3) id = 0x9481;
  tft.begin(id);
  tft.setRotation(0);  // portrait mode
  tft.fillScreen(C_BG);

  // Clear state
  memset(lightState, 0, sizeof(lightState));
  memset(timers, 0, sizeof(timers));

  // Splash screen
  drawSplash();
  delay(2000);

#if NETWORK_ENABLED
  espSerial.begin(ESP_BAUD);
  initNetwork();
#endif

  // Draw home screen
  drawHomeScreen();
}

void loop() {
  handleTouch();
  checkTimers();

#if NETWORK_ENABLED
  // Periodically refresh states from server (every 30s)
  static unsigned long lastRefresh = 0;
  if (millis() - lastRefresh > 30000) {
    refreshStates();
    lastRefresh = millis();
  }
#endif

  delay(50);
}
