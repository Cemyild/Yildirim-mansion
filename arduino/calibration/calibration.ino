// ================================================================
//  Touch Calibration Tool for MCUFRIEND 2.4" ILI9341 TFT Shield
//
//  1. Upload this sketch
//  2. Open Serial Monitor (9600 baud)
//  3. Touch the screen - X/Y/Z values appear on screen + Serial
//  4. Touch all 4 corners and the center
//  5. After 20+ touches, calibration values print on Serial
//  6. Copy the #define lines into home_lights/config.h
//
//  IF NO TOUCH IS DETECTED: Try uncommenting a different pin
//  configuration below (Option B, C, or D) and re-upload.
// ================================================================

#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>

MCUFRIEND_kbv tft;

// ================================================================
//  PIN CONFIGURATION - Try one at a time, re-upload if no touch
// ================================================================

// Option A: Standard for ILI9341 2.4" shields (try this first)
#define YP A1
#define XM A2
#define YM 7
#define XP 6

// Option B: If Option A gives no touch response, comment out A
//           and uncomment these:
// #define YP A3
// #define XM A2
// #define YM 9
// #define XP 8

// Option C:
// #define YP A2
// #define XM A3
// #define YM 8
// #define XP 9

// Option D:
// #define YP A2
// #define XM A1
// #define YM 7
// #define XP 6

// ================================================================

#define PRESSURE_MIN 10
#define PRESSURE_MAX 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Calibration tracking
int16_t minX = 9999, maxX = 0, minY = 9999, maxY = 0;
uint16_t sampleCount = 0;
bool screenDirty = false;

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
  Serial.println();
  Serial.println(F("Touch the screen. Values will appear here."));
  Serial.println(F("Touch all 4 corners for best calibration."));
  Serial.println();

  drawScreen();
}

void drawScreen() {
  tft.fillScreen(0x0000);

  // Title
  tft.setTextColor(0x07E0);  // green
  tft.setTextSize(2);
  tft.setCursor(15, 10);
  tft.print(F("CALIBRATION"));

  // Instructions
  tft.setTextColor(0xFFFF);
  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.print(F("Touch the screen anywhere."));
  tft.setCursor(10, 55);
  tft.print(F("Values shown below + Serial."));

  tft.setTextColor(0xFFE0);  // yellow
  tft.setCursor(10, 75);
  tft.print(F("Touch all 4 corners for"));
  tft.setCursor(10, 88);
  tft.print(F("best calibration results."));

  // Corner targets
  tft.fillCircle(10, 110, 4, 0xF800);
  tft.fillCircle(229, 110, 4, 0xF800);
  tft.fillCircle(10, 260, 4, 0xF800);
  tft.fillCircle(229, 260, 4, 0xF800);

  // Value labels
  tft.setTextColor(0x8410);
  tft.setTextSize(1);
  tft.setCursor(10, 125);
  tft.print(F("--- RAW VALUES ---"));

  // Pin config info at bottom
  tft.setTextColor(0x8410);
  tft.setCursor(10, 280);
  tft.print(F("Pins: YP=A"));
  tft.print(YP - A0);
  tft.print(F(" XM=A"));
  tft.print(XM - A0);
  tft.print(F(" YM="));
  tft.print(YM);
  tft.print(F(" XP="));
  tft.print(XP);

  tft.setCursor(10, 295);
  tft.print(F("No touch? Try other pin option"));
  tft.setCursor(10, 308);
  tft.print(F("in sketch & re-upload."));
}

void loop() {
  // Read touch
  TSPoint tp = ts.getPoint();

  // CRITICAL: restore shared pin modes for LCD
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (tp.z > PRESSURE_MIN && tp.z < PRESSURE_MAX) {
    // Clear value area on screen
    tft.fillRect(0, 138, 240, 130, 0x0000);

    // Display on screen
    tft.setTextColor(0xFFFF, 0x0000);
    tft.setTextSize(3);

    tft.setCursor(10, 140);
    tft.print(F("X: "));
    tft.print(tp.x);
    tft.print(F("    "));

    tft.setCursor(10, 175);
    tft.print(F("Y: "));
    tft.print(tp.y);
    tft.print(F("    "));

    tft.setCursor(10, 210);
    tft.setTextSize(2);
    tft.print(F("Z: "));
    tft.print(tp.z);
    tft.print(F("   "));

    // Sample counter
    tft.setTextSize(1);
    tft.setTextColor(0xBDF7);
    tft.setCursor(10, 245);
    tft.print(F("Sample #"));
    tft.print(sampleCount + 1);
    tft.print(F("    "));

    // Print to Serial
    Serial.print(F("X="));
    Serial.print(tp.x);
    Serial.print(F("\tY="));
    Serial.print(tp.y);
    Serial.print(F("\tZ="));
    Serial.println(tp.z);

    // Track min/max
    if (tp.x < minX) minX = tp.x;
    if (tp.x > maxX) maxX = tp.x;
    if (tp.y < minY) minY = tp.y;
    if (tp.y > maxY) maxY = tp.y;
    sampleCount++;

    // Show calibration values on screen after enough samples
    if (sampleCount >= 20) {
      tft.setTextSize(1);
      tft.setTextColor(0x07E0);
      tft.setCursor(10, 258);
      tft.print(F("X:"));
      tft.print(minX);
      tft.print(F("-"));
      tft.print(maxX);
      tft.print(F("  Y:"));
      tft.print(minY);
      tft.print(F("-"));
      tft.print(maxY);
      tft.print(F("    "));
    }

    // Print calibration results to Serial every 10 samples after 20
    if (sampleCount >= 20 && sampleCount % 10 == 0) {
      Serial.println();
      Serial.println(F("===== CALIBRATION VALUES ====="));
      Serial.println(F("Copy these into config.h:"));
      Serial.println();
      Serial.print(F("#define TS_LEFT    "));
      Serial.println(minX);
      Serial.print(F("#define TS_RIGHT   "));
      Serial.println(maxX);
      Serial.print(F("#define TS_TOP     "));
      Serial.println(minY);
      Serial.print(F("#define TS_BOTTOM  "));
      Serial.println(maxY);
      Serial.println();
      Serial.println(F("Also copy pin defines:"));
      Serial.print(F("#define YP A"));
      Serial.println(YP - A0);
      Serial.print(F("#define XM A"));
      Serial.println(XM - A0);
      Serial.print(F("#define YM "));
      Serial.println(YM);
      Serial.print(F("#define XP "));
      Serial.println(XP);
      Serial.println(F("=============================="));
      Serial.println();
    }
  }

  delay(100);
}
