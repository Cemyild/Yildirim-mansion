// ================================================================
//  UI Drawing Functions
// ================================================================

// ---- Splash Screen ----
void drawSplash() {
  tft.fillScreen(C_BG);
  tft.setTextColor(C_ACCENT);
  tft.setTextSize(2);
  printCentered(F("YILDIRIM"), SCR_W / 2, 100);
  printCentered(F("MANSION"), SCR_W / 2, 125);
  tft.setTextColor(C_DIM);
  tft.setTextSize(1);
  printCentered(F("Home Lights Control"), SCR_W / 2, 165);
  printCentered(F("Touch to begin..."), SCR_W / 2, 200);
}

// ---- Header Bar ----
void drawHeader(const __FlashStringHelper* title, bool showBack) {
  tft.fillRect(0, 0, SCR_W, HDR_H, C_HEADER);

  if (showBack) {
    // Back arrow button
    tft.fillRoundRect(5, 5, 40, 30, 4, C_DANGER);
    tft.setTextColor(C_TEXT);
    tft.setTextSize(2);
    tft.setCursor(14, 11);
    tft.print(F("<"));
  }

  tft.setTextColor(C_ACCENT);
  tft.setTextSize(2);
  uint8_t xOff = showBack ? 50 : 10;
  tft.setCursor(xOff, 12);
  tft.print(title);
}

void drawHeaderPgm(uint8_t room, bool showBack) {
  tft.fillRect(0, 0, SCR_W, HDR_H, C_HEADER);

  if (showBack) {
    tft.fillRoundRect(5, 5, 40, 30, 4, C_DANGER);
    tft.setTextColor(C_TEXT);
    tft.setTextSize(2);
    tft.setCursor(14, 11);
    tft.print(F("<"));
  }

  readPgm(getRoomName(room), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(C_ACCENT);
  tft.setTextSize(2);
  tft.setCursor(showBack ? 50 : 10, 12);
  tft.print(pgmBuf);
}

// ---- Bottom Bar ----
void drawBottomBar(bool showTimer) {
  tft.fillRect(0, BAR_Y, SCR_W, BAR_H, C_HEADER);

  uint8_t btnW = showTimer ? 73 : 110;
  uint8_t btnH = 35;
  uint8_t y = BAR_Y + 5;

  // ALL ON
  tft.fillRoundRect(8, y, btnW, btnH, 4, C_ON);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(1);
  printCentered(F("ALL ON"), 8 + btnW / 2, y + 13);

  // ALL OFF
  tft.fillRoundRect(8 + btnW + 5, y, btnW, btnH, 4, C_OFF);
  tft.setTextColor(C_TEXT);
  printCentered(F("ALL OFF"), 8 + btnW + 5 + btnW / 2, y + 13);

  if (showTimer) {
    // TIMERS
    tft.fillRoundRect(8 + (btnW + 5) * 2, y, btnW, btnH, 4, C_TIMER);
    tft.setTextColor(C_BG);
    printCentered(F("TIMERS"), 8 + (btnW + 5) * 2 + btnW / 2, y + 13);
  }
}

// ---- Home Screen ----
void drawHomeScreen() {
  currentScreen = SCR_HOME;
  tft.fillScreen(C_BG);
  drawHeader(F("YILDIRIM MANSION"), false);
  drawBottomBar(true);

  // Draw room cards in 2x2 grid
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
  tft.fillRoundRect(x, y, GRID_W, GRID_H, 6, cardColor);
  tft.drawRoundRect(x, y, GRID_W, GRID_H, 6, C_BORDER);

  // Light bulb icon
  uint16_t iconColor = anyOn ? C_TEXT : C_DIM;
  int16_t cx = x + GRID_W / 2;
  int16_t cy = y + 30;
  tft.fillCircle(cx, cy, 12, iconColor);
  tft.fillRect(cx - 5, cy + 10, 10, 8, iconColor);

  // Room name
  readPgm(getRoomName(room), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(anyOn ? C_BG : C_TEXT);
  tft.setTextSize(1);
  printCentered(pgmBuf, cx, y + 62);

  // Status: "2/3 ON"
  char statusBuf[10];
  snprintf(statusBuf, sizeof(statusBuf), "%d/%d %s", onCount, total, anyOn ? "ON" : "OFF");
  tft.setTextColor(anyOn ? C_BG : C_DIM);
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
  bool isOn = lightState[selectedRoom][light];

  // Row background
  tft.fillRoundRect(8, y, SCR_W - 16, LIGHT_ROW_H - 5, 4, C_CARD);

  // Status dot
  tft.fillCircle(25, y + (LIGHT_ROW_H - 5) / 2, 6, isOn ? C_ON : C_OFF);

  // Light name
  readPgm(getLightName(selectedRoom, light), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(C_TEXT);
  tft.setTextSize(1);
  tft.setCursor(38, y + 10);
  tft.print(pgmBuf);

  // Timer indicator (small clock icon area)
  if (hasTimer(selectedRoom, light)) {
    tft.setTextColor(C_TIMER);
    tft.setCursor(38, y + 28);
    tft.print(F("timer"));
  }

  // Toggle button
  drawToggleBtn(TOGGLE_X, y + 8, isOn);
}

void drawToggleBtn(int16_t x, int16_t y, bool isOn) {
  uint16_t bgColor = isOn ? C_ON : C_OFF;
  tft.fillRoundRect(x, y, TOGGLE_W, TOGGLE_H, 4, bgColor);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  if (isOn) {
    tft.setCursor(x + 12, y + 7);
    tft.print(F("ON"));
  } else {
    tft.setCursor(x + 6, y + 7);
    tft.print(F("OFF"));
  }
}

// ---- Timer Set Screen ----
void drawTimerSetScreen() {
  currentScreen = SCR_TIMER_SET;
  tft.fillScreen(C_BG);
  drawHeader(F("Set Timer"), true);

  // Show which light
  readPgm(getLightName(selectedRoom, selectedLight), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(C_DIM);
  tft.setTextSize(1);
  tft.setCursor(15, 55);
  tft.print(F("Light: "));
  tft.setTextColor(C_TEXT);
  tft.print(pgmBuf);

  drawTimerAction();
  drawTimerHours();
  drawTimerMinutes();

  // START button
  tft.fillRoundRect(15, 260, 100, 40, 6, C_ON);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  printCentered(F("START"), 65, 273);

  // CANCEL button
  tft.fillRoundRect(125, 260, 100, 40, 6, C_DANGER);
  tft.setTextColor(C_TEXT);
  printCentered(F("CANCEL"), 175, 273);
}

void drawTimerAction() {
  int16_t y = 90;
  // Clear area
  tft.fillRect(15, y, SCR_W - 30, 35, C_BG);

  tft.setTextColor(C_DIM);
  tft.setTextSize(1);
  tft.setCursor(15, y + 5);
  tft.print(F("Action:"));

  // Action toggle button
  uint16_t color = tmpTimerAction ? C_ON : C_OFF;
  tft.fillRoundRect(90, y, 120, 28, 4, color);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  if (tmpTimerAction) {
    printCentered(F("Turn ON"), 150, y + 6);
  } else {
    printCentered(F("Turn OFF"), 150, y + 6);
  }
}

void drawTimerHours() {
  int16_t y = 140;
  tft.fillRect(15, y, SCR_W - 30, 40, C_BG);

  tft.setTextColor(C_DIM);
  tft.setTextSize(1);
  tft.setCursor(15, y + 12);
  tft.print(F("Hours:"));

  // [-] button
  tft.fillRoundRect(90, y + 2, 35, 35, 4, C_CARD);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  tft.setCursor(102, y + 10);
  tft.print(F("-"));

  // Value
  tft.fillRect(130, y + 2, 40, 35, C_BG);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(3);
  char buf[4];
  snprintf(buf, sizeof(buf), "%02d", tmpTimerHours);
  printCentered(buf, 150, y + 8);

  // [+] button
  tft.fillRoundRect(175, y + 2, 35, 35, 4, C_CARD);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  tft.setCursor(185, y + 10);
  tft.print(F("+"));
}

void drawTimerMinutes() {
  int16_t y = 195;
  tft.fillRect(15, y, SCR_W - 30, 40, C_BG);

  tft.setTextColor(C_DIM);
  tft.setTextSize(1);
  tft.setCursor(15, y + 12);
  tft.print(F("Mins:"));

  // [-] button
  tft.fillRoundRect(90, y + 2, 35, 35, 4, C_CARD);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  tft.setCursor(102, y + 10);
  tft.print(F("-"));

  // Value
  tft.fillRect(130, y + 2, 40, 35, C_BG);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(3);
  char buf[4];
  snprintf(buf, sizeof(buf), "%02d", tmpTimerMins);
  printCentered(buf, 150, y + 8);

  // [+] button
  tft.fillRoundRect(175, y + 2, 35, 35, 4, C_CARD);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  tft.setCursor(185, y + 10);
  tft.print(F("+"));
}

// ---- Timer List Screen ----
void drawTimerListScreen() {
  currentScreen = SCR_TIMER_LIST;
  tft.fillScreen(C_BG);
  drawHeader(F("Active Timers"), true);

  if (timerCount == 0) {
    tft.setTextColor(C_DIM);
    tft.setTextSize(1);
    printCentered(F("No active timers"), SCR_W / 2, 150);
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
  int16_t y = 50 + row * 60;

  tft.fillRoundRect(8, y, SCR_W - 16, 52, 4, C_CARD);

  // Light name
  readPgm(getLightName(timers[timerIdx].room, timers[timerIdx].light), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(C_TEXT);
  tft.setTextSize(1);
  tft.setCursor(15, y + 8);
  tft.print(pgmBuf);

  // Action and remaining time
  unsigned long remaining = 0;
  unsigned long now = millis();
  if (timers[timerIdx].triggerAt > now) {
    remaining = (timers[timerIdx].triggerAt - now) / 1000;
  }
  uint8_t mins = remaining / 60;
  uint8_t secs = remaining % 60;

  tft.setTextColor(C_TIMER);
  tft.setCursor(15, y + 22);
  tft.print(timers[timerIdx].action ? F("ON in ") : F("OFF in "));
  char timeBuf[10];
  snprintf(timeBuf, sizeof(timeBuf), "%dm %ds", mins, secs);
  tft.print(timeBuf);

  // Cancel button [X]
  tft.fillRoundRect(SCR_W - 50, y + 10, 35, 30, 4, C_DANGER);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  tft.setCursor(SCR_W - 42, y + 16);
  tft.print(F("X"));
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
