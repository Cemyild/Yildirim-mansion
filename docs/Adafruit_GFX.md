# Adafruit GFX Library - Quick Reference

Core graphics library used by MCUFRIEND_kbv and all Adafruit displays.
GitHub: https://github.com/adafruit/Adafruit-GFX-Library

## Drawing Primitives

### Pixels
```cpp
tft.drawPixel(x, y, color);
```

### Lines
```cpp
tft.drawLine(x0, y0, x1, y1, color);
tft.drawFastHLine(x, y, width, color);   // horizontal (fast)
tft.drawFastVLine(x, y, height, color);  // vertical (fast)
```

### Rectangles
```cpp
tft.drawRect(x, y, w, h, color);         // outline
tft.fillRect(x, y, w, h, color);         // filled
tft.drawRoundRect(x, y, w, h, r, color); // rounded outline
tft.fillRoundRect(x, y, w, h, r, color); // rounded filled
```

### Circles
```cpp
tft.drawCircle(cx, cy, r, color);        // outline
tft.fillCircle(cx, cy, r, color);        // filled
```

### Triangles
```cpp
tft.drawTriangle(x0, y0, x1, y1, x2, y2, color);
tft.fillTriangle(x0, y0, x1, y1, x2, y2, color);
```

### Screen
```cpp
tft.fillScreen(color);                   // clear entire screen
tft.setRotation(r);                      // 0-3
int16_t w = tft.width();
int16_t h = tft.height();
```

## Text

```cpp
tft.setCursor(x, y);                     // set text position
tft.setTextColor(color);                 // text color (transparent bg)
tft.setTextColor(color, bgColor);        // text color + background
tft.setTextSize(size);                   // 1=6x8, 2=12x16, 3=18x24 ...
tft.setTextWrap(true);                   // wrap at screen edge

tft.print("Hello");                      // print string
tft.println("World");                    // print + newline
tft.print(123);                          // print number
tft.print(3.14, 2);                      // print float (2 decimals)
tft.print(F("Flash string"));            // print from flash (saves RAM)
```

### Text Bounds (for centering)
```cpp
int16_t x1, y1;
uint16_t w, h;
tft.getTextBounds("Hello", 0, 0, &x1, &y1, &w, &h);
// w = text width in pixels, h = text height
// Use to center: tft.setCursor(screenW/2 - w/2, y);
```

### Text Size Reference

| Size | Char Width | Char Height | Good For |
|------|-----------|------------|----------|
| 1 | 6px | 8px | Small labels, status text |
| 2 | 12px | 16px | Normal UI text, buttons |
| 3 | 18px | 24px | Headers, titles |
| 4 | 24px | 32px | Large numbers |

## RGB565 Color Format

16-bit color: 5 bits red, 6 bits green, 5 bits blue.

### Common Colors
```cpp
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define ORANGE  0xFD20
#define GRAY    0x8410
```

### Convert RGB to RGB565
```cpp
uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
```

Or calculate manually: `(R/8 << 11) | (G/4 << 5) | (B/8)`

### Online Color Picker
Search "RGB565 color picker" - many web tools convert hex colors to RGB565.

## Memory Tips for Arduino UNO

- Use `F("string")` macro for string literals → stored in flash, not RAM
- Use `tft.setTextColor(fg, bg)` to auto-clear text (avoids `fillRect` call)
- Minimize `fillScreen()` calls - they're slow. Redraw only changed areas.
- `fillRect` is faster than drawing individual pixels
- `drawFastHLine` / `drawFastVLine` are faster than `drawLine`
