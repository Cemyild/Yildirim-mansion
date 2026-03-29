// ================================================================
//  Touch Auto-Detect & Calibration Tool
//  for 2.4" ILI9341 TFT Shield (MCUFRIEND)
//
//  This sketch automatically tries different pin configurations
//  to find which one your touchscreen uses, then enters
//  calibration mode to collect min/max values.
//
//  Just upload and touch the screen when prompted!
// ================================================================

#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>

MCUFRIEND_kbv tft;

// ---- PIN CONFIGURATIONS TO TRY ----
// Each config: {XP, YP, XM, YM}
// We try the most common ones for 2.4" shields

struct PinConfig {
  uint8_t xp, yp, xm, ym;
  const char* label;
};

const PinConfig configs[] = {
  { 6,  A1, A2, 7,  "YP=A1 XM=A2 YM=7 XP=6"  },  // Most common 2.4"
  { 8,  A3, A2, 9,  "YP=A3 XM=A2 YM=9 XP=8"  },  // Some shields
  { 9,  A2, A3, 8,  "YP=A2 XM=A3 YM=8 XP=9"  },  // Alternate wiring
  { 6,  A2, A1, 7,  "YP=A2 XM=A1 YM=7 XP=6"  },  // Swapped analog
};
const uint8_t NUM_CONFIGS = sizeof(configs) / sizeof(configs[0]);

#define PRESSURE_MIN 200
#define PRESSURE_MAX 1000
#define CONFIRM_READS 3  // require 3 consecutive reads to confirm real touch

// Found config index (-1 = not found yet)
int8_t foundConfig = -1;

// Calibration tracking
int16_t minX = 9999, maxX = 0, minY = 9999, maxY = 0;
uint16_t sampleCount = 0;

void setup() {
  Serial.begin(9600);

  uint16_t id = tft.readID();
  if (id == 0xD3D3) id = 0x9481;
  tft.begin(id);
  tft.setRotation(0);

  Serial.print(F("TFT ID: 0x"));
  Serial.println(id, HEX);
  Serial.print(F("Screen: "));
  Serial.print(tft.width());
  Serial.print(F("x"));
  Serial.println(tft.height());

  scanForTouch();
}

// ---- PHASE 1: Auto-detect which pin config works ----
void scanForTouch() {
  tft.fillScreen(0x0000);
  tft.setTextColor(0x07E0);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print(F("AUTO-DETECT"));
  tft.setTextColor(0xFFFF);
  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.print(F("Hold your finger on the"));
  tft.setCursor(10, 55);
  tft.print(F("screen and KEEP PRESSING..."));

  Serial.println(F("\nScanning pin configurations..."));
  Serial.println(F("Press and hold the screen!\n"));

  for (uint8_t i = 0; i < NUM_CONFIGS; i++) {
    // Show which config we're testing
    tft.fillRect(0, 80, 240, 100, 0x0000);
    tft.setTextColor(0xFFE0);  // yellow
    tft.setTextSize(2);
    tft.setCursor(10, 85);
    tft.print(F("Config "));
    tft.print(i + 1);
    tft.print(F("/"));
    tft.print(NUM_CONFIGS);

    tft.setTextColor(0xBDF7);
    tft.setTextSize(1);
    tft.setCursor(10, 115);
    tft.print(configs[i].label);

    tft.setTextColor(0x07E0);
    tft.setTextSize(2);
    tft.setCursor(10, 145);
    tft.print(F("Touch NOW..."));

    Serial.print(F("Testing config "));
    Serial.print(i + 1);
    Serial.print(F(": "));
    Serial.println(configs[i].label);

    // Create a TouchScreen with this config and test for 4 seconds
    TouchScreen testTs(configs[i].xp, configs[i].yp, configs[i].xm, configs[i].ym, 300);
    uint8_t confirmCount = 0;

    // First, check for false positives WITHOUT touching (1 second)
    unsigned long start = millis();
    bool falsePositive = false;
    while (millis() - start < 1000) {
      TSPoint tp = testTs.getPoint();
      pinMode(configs[i].xm, OUTPUT);
      pinMode(configs[i].yp, OUTPUT);
      if (tp.z > PRESSURE_MIN && tp.z < PRESSURE_MAX) {
        falsePositive = true;
        break;
      }
      delay(50);
    }
    if (falsePositive) {
      tft.fillRect(10, 145, 220, 20, 0x0000);
      tft.setTextColor(0xF800);
      tft.setTextSize(1);
      tft.setCursor(10, 150);
      tft.print(F("False reads - skipping"));
      Serial.println(F("  -> False positives, skipping"));
      delay(500);
      continue;
    }

    // Now prompt user to touch (3 seconds)
    tft.fillRect(10, 145, 220, 20, 0x0000);
    tft.setTextColor(0x07E0);
    tft.setTextSize(2);
    tft.setCursor(10, 145);
    tft.print(F("Touch NOW..."));

    start = millis();
    while (millis() - start < 3000) {
      TSPoint tp = testTs.getPoint();

      // Restore pin modes for LCD
      pinMode(configs[i].xm, OUTPUT);
      pinMode(configs[i].yp, OUTPUT);

      if (tp.z > PRESSURE_MIN && tp.z < PRESSURE_MAX) {
        confirmCount++;
      } else {
        confirmCount = 0;  // reset - need consecutive reads
      }

      if (confirmCount >= CONFIRM_READS) {
        // Confirmed real touch!
        foundConfig = i;

        Serial.print(F("** FOUND! Config "));
        Serial.print(i + 1);
        Serial.println(F(" works! **"));
        Serial.print(F("Pins: "));
        Serial.println(configs[i].label);
        Serial.print(F("First reading: X="));
        Serial.print(tp.x);
        Serial.print(F(" Y="));
        Serial.print(tp.y);
        Serial.print(F(" Z="));
        Serial.println(tp.z);

        showFound(i);
        return;
      }

      delay(50);
    }

    // Show "no touch" for this config
    tft.fillRect(10, 145, 220, 20, 0x0000);
    tft.setTextColor(0xF800);  // red
    tft.setTextSize(1);
    tft.setCursor(10, 150);
    tft.print(F("No touch detected"));
    Serial.println(F("  -> No touch detected"));
    delay(500);
  }

  // None found
  showNotFound();
}

void showFound(uint8_t idx) {
  tft.fillScreen(0x0000);

  tft.setTextColor(0x07E0);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print(F("FOUND!"));

  tft.setTextColor(0xFFFF);
  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.print(F("Working config:"));

  tft.setTextColor(0xFFE0);
  tft.setTextSize(1);
  tft.setCursor(10, 58);
  tft.print(configs[idx].label);

  tft.setTextColor(0xFFFF);
  tft.setCursor(10, 80);
  tft.print(F("Copy to config.h:"));

  tft.setTextColor(0x07E0);
  tft.setTextSize(1);
  char buf[30];

  tft.setCursor(10, 98);
  snprintf(buf, sizeof(buf), "#define YP A%d", configs[idx].yp - A0);
  tft.print(buf);

  tft.setCursor(10, 113);
  snprintf(buf, sizeof(buf), "#define XM A%d", configs[idx].xm - A0);
  tft.print(buf);

  tft.setCursor(10, 128);
  snprintf(buf, sizeof(buf), "#define YM %d", configs[idx].ym);
  tft.print(buf);

  tft.setCursor(10, 143);
  snprintf(buf, sizeof(buf), "#define XP %d", configs[idx].xp);
  tft.print(buf);

  // Draw separator
  tft.drawFastHLine(0, 160, 240, 0x3186);

  tft.setTextColor(0xFFE0);
  tft.setCursor(10, 170);
  tft.print(F("Now touch corners to calibrate"));
  tft.setCursor(10, 185);
  tft.print(F("Raw X/Y/Z shown below:"));

  // Draw corner targets
  tft.drawRect(0, 0, 8, 8, 0xF800);
  tft.drawRect(232, 312, 8, 8, 0xF800);

  tft.setTextColor(0xBDF7);
  tft.setCursor(10, 290);
  tft.print(F("Values also on Serial Monitor"));
  tft.setCursor(10, 305);
  tft.print(F("Touch 20+ spots for best result"));
}

void showNotFound() {
  tft.fillScreen(0x0000);
  tft.setTextColor(0xF800);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print(F("NOT FOUND"));

  tft.setTextColor(0xFFFF);
  tft.setTextSize(1);
  tft.setCursor(10, 45);
  tft.print(F("No touch detected on any"));
  tft.setCursor(10, 60);
  tft.print(F("pin configuration."));

  tft.setCursor(10, 90);
  tft.print(F("Possible causes:"));
  tft.setCursor(10, 110);
  tft.print(F("- Shield has no touchscreen"));
  tft.setCursor(10, 125);
  tft.print(F("- Touchscreen is damaged"));
  tft.setCursor(10, 140);
  tft.print(F("- Uses SPI touch (XPT2046)"));
  tft.setCursor(10, 155);
  tft.print(F("  not resistive 4-wire"));

  tft.setCursor(10, 185);
  tft.print(F("Press RESET to try again"));
  tft.setCursor(10, 200);
  tft.print(F("(hold finger while scanning)"));

  Serial.println(F("\nNo working configuration found."));
  Serial.println(F("Make sure you are pressing the screen during scanning."));
  Serial.println(F("Press RESET on Arduino to try again."));
}

// ---- PHASE 2: Live calibration mode ----
void loop() {
  if (foundConfig < 0) {
    delay(1000);
    return;  // stuck on "not found" screen
  }

  // Read touch with the working config
  TouchScreen ts(configs[foundConfig].xp, configs[foundConfig].yp,
                 configs[foundConfig].xm, configs[foundConfig].ym, 300);

  TSPoint tp = ts.getPoint();

  // Restore pin modes
  pinMode(configs[foundConfig].xm, OUTPUT);
  pinMode(configs[foundConfig].yp, OUTPUT);

  if (tp.z > PRESSURE_MIN && tp.z < PRESSURE_MAX) {
    // Clear value display area
    tft.fillRect(0, 205, 240, 80, 0x0000);

    // Show raw values
    tft.setTextColor(0xFFFF);
    tft.setTextSize(2);

    tft.setCursor(10, 208);
    tft.print(F("X:"));
    tft.print(tp.x);
    tft.print(F("   "));

    tft.setCursor(10, 230);
    tft.print(F("Y:"));
    tft.print(tp.y);
    tft.print(F("   "));

    tft.setCursor(10, 252);
    tft.print(F("Z:"));
    tft.print(tp.z);
    tft.print(F("   "));

    // Show sample count
    tft.setTextSize(1);
    tft.setTextColor(0xBDF7);
    tft.setCursor(160, 270);
    tft.print(F("#"));
    tft.print(sampleCount + 1);
    tft.print(F("  "));

    // Serial output
    Serial.print(F("X="));
    Serial.print(tp.x);
    Serial.print(F("  Y="));
    Serial.print(tp.y);
    Serial.print(F("  Z="));
    Serial.println(tp.z);

    // Track min/max
    if (tp.x < minX) minX = tp.x;
    if (tp.x > maxX) maxX = tp.x;
    if (tp.y < minY) minY = tp.y;
    if (tp.y > maxY) maxY = tp.y;
    sampleCount++;

    // Print calibration results after enough samples
    if (sampleCount >= 20 && sampleCount % 10 == 0) {
      Serial.println(F("\n===== CALIBRATION VALUES ====="));
      Serial.println(F("Copy these into config.h:\n"));
      Serial.print(F("#define TS_LEFT    "));
      Serial.println(minX);
      Serial.print(F("#define TS_RIGHT   "));
      Serial.println(maxX);
      Serial.print(F("#define TS_TOP     "));
      Serial.println(minY);
      Serial.print(F("#define TS_BOTTOM  "));
      Serial.println(maxY);
      Serial.println(F("==============================\n"));
    }
  }

  delay(100);
}
