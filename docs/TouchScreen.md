# Adafruit TouchScreen - Quick Reference

4-wire resistive touchscreen library.
GitHub: https://github.com/adafruit/Adafruit_TouchScreen

**Install**: Arduino Library Manager → search "Adafruit TouchScreen"

## Setup

```cpp
#include <TouchScreen.h>

// Pin definitions - these vary by shield!
#define YP A3   // must be analog pin
#define XM A2   // must be analog pin
#define YM 9    // can be digital pin
#define XP 8    // can be digital pin

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
// Last parameter (300) = resistance between X+ and X- pins (ohms)
// Measure with multimeter, or use 300 as default
```

## Reading Touch

```cpp
TSPoint tp = ts.getPoint();

// CRITICAL: Restore pin modes after reading!
// Touch reading changes analog pins to INPUT.
// LCD needs them as OUTPUT to work.
pinMode(XM, OUTPUT);
pinMode(YP, OUTPUT);

// Check if touch is valid
if (tp.z > 10 && tp.z < 1000) {
  // tp.x = raw X (0-1023 range typically)
  // tp.y = raw Y (0-1023 range typically)
  // tp.z = pressure (higher = harder press)
}
```

## Mapping to Screen Coordinates

Raw touch values need to be mapped to pixel coordinates:

```cpp
// Map raw touch → screen pixels
int16_t px = map(tp.y, TS_LEFT, TS_RIGHT, 0, tft.width());
int16_t py = map(tp.x, TS_TOP, TS_BOTTOM, 0, tft.height());

// NOTE: tp.y maps to screen X, tp.x maps to screen Y
// This is because touch and display axes are often swapped.
// If touch is wrong, try swapping tp.x/tp.y in the map() calls.
```

## Common Pin Configurations

Different shield manufacturers use different pins:

| Shield | YP | XM | YM | XP |
|--------|----|----|----|----|
| Config A | A3 | A2 | 9 | 8 |
| Config B | A1 | A2 | 7 | 6 |
| Config C | A2 | A3 | 8 | 9 |

**How to find yours**: Use the calibration sketch (`arduino/calibration/calibration.ino`).
If no touch is detected, try each pin configuration.

## Calibration

Each shield has different raw value ranges. You need 4 values:

| Value | Meaning | How to find |
|-------|---------|-------------|
| TS_LEFT | Raw value at left screen edge | Touch left side, note X |
| TS_RIGHT | Raw value at right screen edge | Touch right side, note X |
| TS_TOP | Raw value at top screen edge | Touch top, note Y |
| TS_BOTTOM | Raw value at bottom screen edge | Touch bottom, note Y |

Typical ranges: 100-900 (varies per unit).

## Troubleshooting

| Problem | Solution |
|---------|----------|
| No touch detected | Try different pin configs (A/B/C above) |
| Touch works but position is wrong | Swap tp.x/tp.y in map() calls |
| Touch is mirrored horizontally | Swap TS_LEFT and TS_RIGHT values |
| Touch is mirrored vertically | Swap TS_TOP and TS_BOTTOM values |
| Touch is offset/inaccurate | Re-run calibration, touch corners carefully |
| Display goes white when touching | Missing `pinMode(XM, OUTPUT); pinMode(YP, OUTPUT);` after `getPoint()` |
| Phantom touches | Increase TS_PRESSURE_MIN (try 20-50) |
| Requires hard press | Decrease TS_PRESSURE_MIN or increase the resistance parameter |

## Important Notes

1. **Always restore pin modes** after `getPoint()` - the LCD will stop working otherwise
2. **Debounce touches** - add 200-300ms delay between touch reads to prevent double-taps
3. **Pressure threshold** - use `tp.z` to filter out noise (typical: z > 10 && z < 1000)
4. The touchscreen shares analog pins with the LCD controller - they cannot be used simultaneously
