// ================================================================
//  UI Drawing Functions
// ================================================================

// ---- Splash Screen ----
void drawSplash() {
  tft.fillScreen(C_BG);
  tft.setTextColor(C_ACCENT);
  tft.setTextSize(3);
  printCentered(F("YILDIRIM"), SCR_W / 2, 90);
  printCentered(F("MANSION"), SCR_W / 2, 125);
  tft.setTextColor(C_DIM);
  tft.setTextSize(1);
  printCentered(F("Home Lights Control"), SCR_W / 2, 175);
  printCentered(F("Touch to begin..."), SCR_W / 2, 210);
}

// ---- Header Bar ----
void drawHeader(const __FlashStringHelper* title, bool showBack) {
  tft.fillRect(0, 0, SCR_W, HDR_H, C_HEADER);

  if (showBack) {
    // Back button - big touch target
    tft.fillRoundRect(4, 4, 52, 38, 5, C_DANGER);
    tft.setTextColor(C_TEXT);
    tft.setTextSize(3);
    tft.setCursor(16, 10);
    tft.print(F("<"));
  }

  tft.setTextColor(C_ACCENT);
  tft.setTextSize(2);
  uint8_t xOff = showBack ? 62 : 10;
  tft.setCursor(xOff, 14);
  tft.print(title);
}

void drawHeaderPgm(uint8_t room, bool showBack) {
  tft.fillRect(0, 0, SCR_W, HDR_H, C_HEADER);

  if (showBack) {
    tft.fillRoundRect(4, 4, 52, 38, 5, C_DANGER);
    tft.setTextColor(C_TEXT);
    tft.setTextSize(3);
    tft.setCursor(16, 10);
    tft.print(F("<"));
  }

  readPgm(getRoomName(room), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(C_ACCENT);
  tft.setTextSize(2);
  tft.setCursor(showBack ? 62 : 10, 14);
  tft.print(pgmBuf);
}

// ---- Bottom Bar ----
void drawBottomBar(bool showTimer) {
  tft.fillRect(0, BAR_Y, SCR_W, BAR_H, C_HEADER);

  uint8_t btnH = 38;
  uint8_t y = BAR_Y + 5;

  if (showTimer) {
    // 3 buttons: ALL ON (72px) + ALL OFF (72px) + TIMERS (72px) + gaps
    uint8_t btnW = 72;
    uint8_t gap = 6;
    uint8_t x0 = 6;

    // ALL ON
    tft.fillRoundRect(x0, y, btnW, btnH, 5, C_ON);
    tft.setTextColor(C_TEXT);
    tft.setTextSize(1);
    printCentered(F("ALL ON"), x0 + btnW / 2, y + 15);

    // ALL OFF
    tft.fillRoundRect(x0 + btnW + gap, y, btnW, btnH, 5, C_OFF);
    tft.setTextColor(C_TEXT);
    printCentered(F("ALL OFF"), x0 + btnW + gap + btnW / 2, y + 15);

    // TIMERS
    tft.fillRoundRect(x0 + (btnW + gap) * 2, y, btnW, btnH, 5, C_TIMER);
    tft.setTextColor(C_BG);
    printCentered(F("TIMERS"), x0 + (btnW + gap) * 2 + btnW / 2, y + 15);
  } else {
    // 2 buttons only
    uint8_t btnW = 110;
    uint8_t gap = 8;
    uint8_t x0 = 6;

    tft.fillRoundRect(x0, y, btnW, btnH, 5, C_ON);
    tft.setTextColor(C_TEXT);
    tft.setTextSize(1);
    printCentered(F("ALL ON"), x0 + btnW / 2, y + 15);

    tft.fillRoundRect(x0 + btnW + gap, y, btnW, btnH, 5, C_OFF);
    tft.setTextColor(C_TEXT);
    printCentered(F("ALL OFF"), x0 + btnW + gap + btnW / 2, y + 15);
  }
}

// ---- Home Screen ----
void drawHomeScreen() {
  currentScreen = SCR_HOME;
  tft.fillScreen(C_BG);
  drawHeader(F("YILDIRIM"), false);
  drawBottomBar(true);

  for (uint8_t i = 0; i < NUM_ROOMS; i++) {
    drawRoomCard(i);
  }
}

void drawRoomCard(uint8_t room) {
  uint8_t col = room % 2;
  uint8_t row = room / 2;
  int16_t x = (col == 0) ? GRID_X1 : GRID_X2;
  int16_t y = (row == 0) ? GRID_Y1 : GRID_Y2;

  uint8_t onCount = countOnLights(room);
  uint8_t total = getLightCount(room);
  bool anyOn = (onCount > 0);

  // Card background
  uint16_t cardColor = anyOn ? C_ON : C_CARD;
  tft.fillRoundRect(x, y, GRID_W, GRID_H, 8, cardColor);
  tft.drawRoundRect(x, y, GRID_W, GRID_H, 8, C_BORDER);

  int16_t cx = x + GRID_W / 2;

  // Light bulb icon (centered, upper area)
  uint16_t iconColor = anyOn ? C_TEXT : C_DIM;
  tft.fillCircle(cx, y + 28, 14, iconColor);
  tft.fillRect(cx - 6, y + 40, 12, 8, iconColor);
  // Bulb base lines
  uint16_t lineColor = anyOn ? cardColor : C_BG;
  tft.drawFastHLine(cx - 4, y + 42, 8, lineColor);
  tft.drawFastHLine(cx - 4, y + 44, 8, lineColor);

  // Room name (below icon with gap)
  readPgm(getRoomName(room), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(anyOn ? C_BG : C_TEXT);
  tft.setTextSize(1);
  printCentered(pgmBuf, cx, y + 62);

  // Status: "2/3 ON" or "OFF"
  char statusBuf[10];
  if (anyOn) {
    snprintf(statusBuf, sizeof(statusBuf), "%d/%d ON", onCount, total);
  } else {
    snprintf(statusBuf, sizeof(statusBuf), "%d OFF", total);
  }
  tft.setTextColor(anyOn ? C_BG : C_DIM);
  tft.setTextSize(1);
  printCentered(statusBuf, cx, y + 82);
}

// ---- Room Screen ----
void drawRoomScreen() {
  currentScreen = SCR_ROOM;
  tft.fillScreen(C_BG);
  drawHeaderPgm(selectedRoom, true);
  drawBottomBar(true);

  uint8_t count = getLightCount(selectedRoom);
  for (uint8_t i = 0; i < count; i++) {
    drawLightRow(i);
  }
}

void drawLightRow(uint8_t light) {
  int16_t y = LIGHT_Y_START + light * LIGHT_ROW_H;
  int16_t rowH = LIGHT_ROW_H - 6;  // gap between rows
  bool isOn = lightState[selectedRoom][light];

  // Row background
  tft.fillRoundRect(6, y, SCR_W - 12, rowH, 6, C_CARD);

  // Status dot (vertically centered)
  int16_t dotY = y + rowH / 2;
  tft.fillCircle(24, dotY, 7, isOn ? C_ON : C_OFF);

  // Light name (vertically centered)
  readPgm(getLightName(selectedRoom, light), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  tft.setCursor(38, y + rowH / 2 - 8);
  tft.print(pgmBuf);

  // Timer indicator
  if (hasTimer(selectedRoom, light)) {
    tft.setTextColor(C_TIMER);
    tft.setTextSize(1);
    tft.setCursor(38, y + rowH / 2 + 10);
    tft.print(F("timer active"));
  }

  // Toggle button (vertically centered)
  int16_t toggleY = y + (rowH - TOGGLE_H) / 2;
  drawToggleBtn(TOGGLE_X, toggleY, isOn);
}

void drawToggleBtn(int16_t x, int16_t y, bool isOn) {
  uint16_t bgColor = isOn ? C_ON : C_OFF;
  tft.fillRoundRect(x, y, TOGGLE_W, TOGGLE_H, 5, bgColor);
  tft.drawRoundRect(x, y, TOGGLE_W, TOGGLE_H, 5, C_BORDER);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  if (isOn) {
    printCentered(F("ON"), x + TOGGLE_W / 2, y + 9);
  } else {
    printCentered(F("OFF"), x + TOGGLE_W / 2, y + 9);
  }
}

// ---- Timer Set Screen ----
void drawTimerSetScreen() {
  currentScreen = SCR_TIMER_SET;
  tft.fillScreen(C_BG);
  drawHeader(F("Set Timer"), true);

  // Show which light
  readPgm(getRoomName(selectedRoom), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(C_DIM);
  tft.setTextSize(1);
  tft.setCursor(15, 55);
  tft.print(pgmBuf);
  tft.print(F(" > "));
  readPgm(getLightName(selectedRoom, selectedLight), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  tft.setCursor(15, 70);
  tft.print(pgmBuf);

  drawTimerAction();
  drawTimerHours();
  drawTimerMinutes();

  // START button
  tft.fillRoundRect(15, 260, 100, 44, 6, C_ON);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  printCentered(F("START"), 65, 274);

  // CANCEL button
  tft.fillRoundRect(125, 260, 100, 44, 6, C_DANGER);
  tft.setTextColor(C_TEXT);
  printCentered(F("CANCEL"), 175, 274);
}

void drawTimerAction() {
  int16_t y = 100;
  tft.fillRect(15, y, SCR_W - 30, 38, C_BG);

  tft.setTextColor(C_DIM);
  tft.setTextSize(1);
  tft.setCursor(15, y + 10);
  tft.print(F("Action:"));

  // Action toggle button
  uint16_t color = tmpTimerAction ? C_ON : C_OFF;
  tft.fillRoundRect(90, y + 2, 125, 34, 5, color);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  if (tmpTimerAction) {
    printCentered(F("Turn ON"), 152, y + 10);
  } else {
    printCentered(F("Turn OFF"), 152, y + 10);
  }
}

void drawTimerHours() {
  int16_t y = 150;
  tft.fillRect(15, y, SCR_W - 30, 42, C_BG);

  tft.setTextColor(C_DIM);
  tft.setTextSize(1);
  tft.setCursor(15, y + 14);
  tft.print(F("Hours:"));

  // [-] button
  tft.fillRoundRect(90, y + 2, 38, 38, 5, C_CARD);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(3);
  tft.setCursor(100, y + 8);
  tft.print(F("-"));

  // Value
  tft.fillRect(133, y + 2, 44, 38, C_BG);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(3);
  char buf[4];
  snprintf(buf, sizeof(buf), "%02d", tmpTimerHours);
  printCentered(buf, 155, y + 9);

  // [+] button
  tft.fillRoundRect(182, y + 2, 38, 38, 5, C_CARD);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(3);
  tft.setCursor(190, y + 8);
  tft.print(F("+"));
}

void drawTimerMinutes() {
  int16_t y = 202;
  tft.fillRect(15, y, SCR_W - 30, 42, C_BG);

  tft.setTextColor(C_DIM);
  tft.setTextSize(1);
  tft.setCursor(15, y + 14);
  tft.print(F("Mins:"));

  // [-] button
  tft.fillRoundRect(90, y + 2, 38, 38, 5, C_CARD);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(3);
  tft.setCursor(100, y + 8);
  tft.print(F("-"));

  // Value
  tft.fillRect(133, y + 2, 44, 38, C_BG);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(3);
  char buf[4];
  snprintf(buf, sizeof(buf), "%02d", tmpTimerMins);
  printCentered(buf, 155, y + 9);

  // [+] button
  tft.fillRoundRect(182, y + 2, 38, 38, 5, C_CARD);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(3);
  tft.setCursor(190, y + 8);
  tft.print(F("+"));
}

// ---- Timer List Screen ----
void drawTimerListScreen() {
  currentScreen = SCR_TIMER_LIST;
  tft.fillScreen(C_BG);
  drawHeader(F("Active Timers"), true);

  if (timerCount == 0) {
    tft.setTextColor(C_DIM);
    tft.setTextSize(2);
    printCentered(F("No active"), SCR_W / 2, 130);
    printCentered(F("timers"), SCR_W / 2, 155);
    return;
  }

  uint8_t row = 0;
  for (uint8_t i = 0; i < MAX_TIMERS && row < 4; i++) {
    if (!timers[i].active) continue;
    drawTimerListRow(i, row);
    row++;
  }
}

void drawTimerListRow(uint8_t timerIdx, uint8_t row) {
  int16_t y = 55 + row * 55;

  tft.fillRoundRect(6, y, SCR_W - 12, 48, 5, C_CARD);

  // Light name
  readPgm(getLightName(timers[timerIdx].room, timers[timerIdx].light), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(C_TEXT);
  tft.setTextSize(1);
  tft.setCursor(14, y + 8);
  tft.print(pgmBuf);

  // Remaining time
  unsigned long remaining = 0;
  unsigned long now = millis();
  if (timers[timerIdx].triggerAt > now) {
    remaining = (timers[timerIdx].triggerAt - now) / 1000;
  }
  uint8_t mins = remaining / 60;
  uint8_t secs = remaining % 60;

  tft.setTextColor(C_TIMER);
  tft.setTextSize(2);
  tft.setCursor(14, y + 22);
  tft.print(timers[timerIdx].action ? F("ON ") : F("OFF "));
  char timeBuf[10];
  snprintf(timeBuf, sizeof(timeBuf), "%dm%ds", mins, secs);
  tft.print(timeBuf);

  // Cancel [X] button
  tft.fillRoundRect(SCR_W - 48, y + 8, 38, 32, 5, C_DANGER);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  printCentered(F("X"), SCR_W - 29, y + 16);
}

// ---- Helper: centered text ----
void printCentered(const __FlashStringHelper* text, int16_t cx, int16_t y) {
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor(cx - w / 2, y);
  tft.print(text);
}

void printCentered(const char* text, int16_t cx, int16_t y) {
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor(cx - w / 2, y);
  tft.print(text);
}
