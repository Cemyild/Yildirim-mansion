// ================================================================
//  Touch Calibration Tool for 2.4" ILI9341 TFT Shield
//
//  Upload this sketch FIRST to find your touchscreen values.
//  Then copy the values into home_lights/config.h
//
//  HOW TO USE:
//  1. Upload this sketch
//  2. Touch the screen - raw X, Y, Z values appear on display
//  3. Touch the TOP-LEFT corner    -> note the X and Y values
//  4. Touch the BOTTOM-RIGHT corner -> note the X and Y values
//  5. Touch the TOP-RIGHT corner   -> note the X and Y values
//  6. Touch the BOTTOM-LEFT corner -> note the X and Y values
//  7. Copy values into config.h (see instructions on screen)
//
//  If nothing shows when you touch, try changing the touch pins
//  (YP, XM, YM, XP) - common alternatives listed below.
// ================================================================

#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>

// ---- TRY THESE PIN COMBINATIONS IF TOUCH DOESN'T WORK ----
// Option A (most common):
#define YP A3
#define XM A2
#define YM 9
#define XP 8

// Option B (some shields):
// #define YP A1
// #define XM A2
// #define YM 7
// #define XP 6

// Option C (other shields):
// #define YP A2
// #define XM A3
// #define YM 8
// #define XP 9

#define TS_PRESSURE_MIN 10
#define TS_PRESSURE_MAX 1000

MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Store corner values
int16_t tlX = 0, tlY = 0;  // top-left
int16_t brX = 0, brY = 0;  // bottom-right
uint8_t step = 0;           // calibration step

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

  drawInstructions();
}

void drawInstructions() {
  tft.fillScreen(0x0000);

  tft.setTextColor(0x07E0);  // green
  tft.setTextSize(2);
  tft.setCursor(20, 10);
  tft.print(F("TOUCH CALIBR."));

  tft.setTextColor(0xFFFF);
  tft.setTextSize(1);

  tft.setCursor(10, 50);
  tft.print(F("Touch the screen anywhere."));
  tft.setCursor(10, 65);
  tft.print(F("Raw values shown below."));

  tft.setTextColor(0xFFE0);  // yellow
  tft.setCursor(10, 95);
  tft.print(F("Then touch each corner:"));

  tft.setTextColor(0xFFFF);
  tft.setCursor(10, 115);
  tft.print(F("1) Touch TOP-LEFT corner"));
  tft.setCursor(10, 130);
  tft.print(F("   (hold 2 sec)"));
  tft.setCursor(10, 150);
  tft.print(F("2) Touch BOTTOM-RIGHT corner"));
  tft.setCursor(10, 165);
  tft.print(F("   (hold 2 sec)"));

  // Draw corner targets
  tft.drawRect(0, 0, 20, 20, 0xF800);        // top-left
  tft.drawLine(0, 0, 19, 19, 0xF800);
  tft.drawLine(19, 0, 0, 19, 0xF800);

  tft.drawRect(220, 300, 20, 20, 0xF800);     // bottom-right
  tft.drawLine(220, 300, 239, 319, 0xF800);
  tft.drawLine(239, 300, 220, 319, 0xF800);

  // Status area label
  tft.setTextColor(0x07E0);
  tft.setCursor(10, 195);
  tft.print(F("--- LIVE VALUES ---"));

  tft.setTextColor(0xBDF7);
  tft.setCursor(10, 280);
  tft.print(F("Also printed to Serial"));
  tft.setCursor(10, 295);
  tft.print(F("Monitor (9600 baud)"));
}

// Clear the value display area only
void clearValues() {
  tft.fillRect(0, 210, 240, 60, 0x0000);
}

void loop() {
  TSPoint tp = ts.getPoint();

  // Restore pin modes for LCD
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (tp.z > TS_PRESSURE_MIN && tp.z < TS_PRESSURE_MAX) {
    clearValues();

    // Show raw values on screen
    tft.setTextColor(0xFFFF);
    tft.setTextSize(2);

    tft.setCursor(10, 212);
    tft.print(F("X: "));
    tft.print(tp.x);

    tft.setCursor(10, 234);
    tft.print(F("Y: "));
    tft.print(tp.y);

    tft.setCursor(10, 256);
    tft.print(F("Z: "));
    tft.print(tp.z);

    // Print to Serial too
    Serial.print(F("X="));
    Serial.print(tp.x);
    Serial.print(F("  Y="));
    Serial.print(tp.y);
    Serial.print(F("  Z="));
    Serial.println(tp.z);

    // Auto-detect corners based on extreme values
    // After collecting enough touches, print the config values
    static int16_t minX = 9999, maxX = 0, minY = 9999, maxY = 0;
    static uint16_t sampleCount = 0;

    if (tp.x < minX) minX = tp.x;
    if (tp.x > maxX) maxX = tp.x;
    if (tp.y < minY) minY = tp.y;
    if (tp.y > maxY) maxY = tp.y;
    sampleCount++;

    // After 20+ touches, show the detected range
    if (sampleCount >= 20) {
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
      Serial.println(F("\nNOTE: TS_LEFT/RIGHT map to X,"));
      Serial.println(F("TS_TOP/BOTTOM map to Y."));
      Serial.println(F("If touch is mirrored, swap LEFT/RIGHT"));
      Serial.println(F("or TOP/BOTTOM values."));
      Serial.println(F("==============================\n"));
    }
  }

  delay(100);
}
