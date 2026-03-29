# MCUFRIEND_kbv - Quick Reference

TFT LCD driver for Arduino UNO shields (2.4", 2.8", 3.5" etc.)
GitHub: https://github.com/prenticedavid/MCUFRIEND_kbv

## Setup

```cpp
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

void setup() {
  uint16_t id = tft.readID();
  if (id == 0xD3D3) id = 0x9481; // fallback for unrecognized
  tft.begin(id);
  tft.setRotation(0); // 0=portrait, 1=landscape, 2=portrait flipped, 3=landscape flipped
}
```

## Common Controller IDs

| ID | Controller |
|----|-----------|
| 0x9341 | ILI9341 (most common 2.4") |
| 0x9481 | ILI9481 |
| 0x9486 | ILI9486 (3.5") |
| 0x7789 | ST7789 |

## Pin Usage (Arduino UNO Shield)

| Pins | Function |
|------|----------|
| D2-D9 | 8-bit data bus |
| A0 | CD (Command/Data) |
| A1 | CS (Chip Select) |
| A2 | WR (Write strobe) |
| A3 | RD (Read strobe) |
| **Free**: D0, D1, D10-D13, A4, A5 |

## Key Methods

All drawing methods are inherited from Adafruit_GFX. MCUFRIEND_kbv adds:

```cpp
uint16_t readID();              // Read controller ID
void begin(uint16_t id);        // Initialize with controller ID
void setRotation(uint8_t r);    // 0-3
int16_t width();                // Current width (depends on rotation)
int16_t height();               // Current height
void vertScroll(int16_t top, int16_t scrollines, int16_t offset);
uint16_t readPixel(int16_t x, int16_t y);
void pushColors(uint16_t *block, int16_t n, bool first);
```

## Supported Shields

Works with most cheap TFT shields from AliExpress/eBay that plug directly into Arduino UNO. Auto-detects the controller chip.

## Troubleshooting

- **White screen**: Wrong ID. Try `tft.begin(0x9341)` explicitly
- **Garbled display**: Check `setRotation()` value
- **Touch not working**: Touch pins are separate from display pins (see TouchScreen docs)
