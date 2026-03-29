// ================================================================
//  UI Drawing Functions — Modern Dark Theme
// ================================================================

// ---- Splash Screen ----
void drawSplash() {
  tft.fillScreen(C_BG);
  tft.setTextColor(C_ACCENT);
  tft.setTextSize(3);
  printCentered(F("YILDIRIM"), SCR_W / 2, 100);
  tft.setTextColor(C_DIM);
  tft.setTextSize(2);
  printCentered(F("MANSION"), SCR_W / 2, 135);
  tft.setTextSize(1);
  tft.setTextColor(C_DIM);
  printCentered(F("Home Lights"), SCR_W / 2, 180);
  // Accent line
  tft.drawFastHLine(80, 170, 80, C_ACCENT);
}

// ---- Header Bar ----
void drawHeader(const __FlashStringHelper* title, bool showBack) {
  tft.fillRect(0, 0, SCR_W, HDR_H, C_BG);

  if (showBack) {
    // Simple arrow, no red box
    tft.setTextColor(C_ACCENT);
    tft.setTextSize(3);
    tft.setCursor(8, 10);
    tft.print(F("<"));
  }

  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  int16_t xOff = showBack ? 38 : 10;
  tft.setCursor(xOff, 14);
  tft.print(title);

  // Thin accent line under header
  tft.drawFastHLine(0, HDR_H - 1, SCR_W, C_BORDER);
}

void drawHeaderPgm(uint8_t room, bool showBack) {
  tft.fillRect(0, 0, SCR_W, HDR_H, C_BG);

  if (showBack) {
    tft.setTextColor(C_ACCENT);
    tft.setTextSize(3);
    tft.setCursor(8, 10);
    tft.print(F("<"));
  }

  readPgm(getRoomName(room), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  tft.setCursor(showBack ? 38 : 10, 14);
  tft.print(pgmBuf);

  tft.drawFastHLine(0, HDR_H - 1, SCR_W, C_BORDER);
}

// ---- Bottom Bar ----
void drawBottomBar(bool showTimer) {
  // Divider line above bar
  tft.drawFastHLine(0, BAR_Y - 1, SCR_W, C_BORDER);
  tft.fillRect(0, BAR_Y, SCR_W, BAR_H, C_BG);

  // IMPORTANT: use int16_t — BAR_Y+offset can exceed 255
  int16_t y = BAR_Y + 8;
  int16_t btnH = 28;

  if (showTimer) {
    int16_t btnW = 72;
    int16_t gap = 5;
    int16_t x0 = 6;

    // ALL ON — outlined style with accent
    tft.drawRoundRect(x0, y, btnW, btnH, 6, C_ON);
    tft.setTextColor(C_ON);
    tft.setTextSize(1);
    printCentered(F("ALL ON"), x0 + btnW / 2, y + 10);

    // ALL OFF — outlined style
    tft.drawRoundRect(x0 + btnW + gap, y, btnW, btnH, 6, C_DIM);
    tft.setTextColor(C_DIM);
    printCentered(F("ALL OFF"), x0 + btnW + gap + btnW / 2, y + 10);

    // TIMERS — outlined accent
    tft.drawRoundRect(x0 + (btnW + gap) * 2, y, btnW, btnH, 6, C_ACCENT);
    tft.setTextColor(C_ACCENT);
    printCentered(F("TIMERS"), x0 + (btnW + gap) * 2 + btnW / 2, y + 10);
  } else {
    int16_t btnW = 110;
    int16_t x0 = 8;

    tft.drawRoundRect(x0, y, btnW, btnH, 6, C_ON);
    tft.setTextColor(C_ON);
    tft.setTextSize(1);
    printCentered(F("ALL ON"), x0 + btnW / 2, y + 10);

    tft.drawRoundRect(x0 + btnW + 8, y, btnW, btnH, 6, C_DIM);
    tft.setTextColor(C_DIM);
    printCentered(F("ALL OFF"), x0 + btnW + 8 + btnW / 2, y + 10);
  }
}

// ---- Home Screen ----
void drawHomeScreen() {
  currentScreen = SCR_HOME;
  tft.fillScreen(C_BG);

  // Centered title
  tft.setTextColor(C_ACCENT);
  tft.setTextSize(2);
  printCentered(F("YILDIRIM"), SCR_W / 2, 14);
  tft.drawFastHLine(0, HDR_H - 1, SCR_W, C_BORDER);

  for (uint8_t i = 0; i < NUM_ROOMS; i++) {
    drawRoomCard(i);
  }

  drawBottomBar(true);
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
  tft.fillRoundRect(x, y, GRID_W, GRID_H, 10, C_CARD);

  // Left accent bar (4px wide, inside card)
  uint16_t accentColor = anyOn ? C_ON : C_OFF;
  tft.fillRoundRect(x, y, 6, GRID_H, 10, accentColor);
  tft.fillRect(x + 3, y + 3, 3, GRID_H - 6, accentColor);

  int16_t cx = x + GRID_W / 2 + 2;  // shift right slightly due to accent bar

  // Room name
  readPgm(getRoomName(room), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(C_TEXT);
  tft.setTextSize(1);
  printCentered(pgmBuf, cx, y + 35);

  // Status count
  char statusBuf[10];
  if (anyOn) {
    snprintf(statusBuf, sizeof(statusBuf), "%d/%d ON", onCount, total);
    tft.setTextColor(C_ON);
  } else {
    snprintf(statusBuf, sizeof(statusBuf), "%d lights", total);
    tft.setTextColor(C_DIM);
  }
  tft.setTextSize(1);
  printCentered(statusBuf, cx, y + 55);

  // Power icon: filled circle when ON, ring when OFF
  if (anyOn) {
    tft.fillCircle(cx, y + 82, 8, C_ON);
  } else {
    tft.drawCircle(cx, y + 82, 8, C_DIM);
  }
  // Power symbol vertical line
  uint16_t lineC = anyOn ? C_BG : C_DIM;
  tft.drawFastVLine(cx, y + 74, 8, lineC);
}

// ---- Room Screen ----
void drawRoomScreen() {
  currentScreen = SCR_ROOM;
  tft.fillScreen(C_BG);
  drawHeaderPgm(selectedRoom, true);

  uint8_t count = getLightCount(selectedRoom);
  for (uint8_t i = 0; i < count; i++) {
    drawLightRow(i);
  }

  drawBottomBar(true);
}

void drawLightRow(uint8_t light) {
  int16_t y = LIGHT_Y_START + light * LIGHT_ROW_H;
  int16_t rowH = LIGHT_ROW_H - 6;
  bool isOn = lightState[selectedRoom][light];

  // Card background
  tft.fillRoundRect(8, y, SCR_W - 16, rowH, 8, C_CARD);

  // Status indicator — thin vertical bar on left
  uint16_t barColor = isOn ? C_ON : C_OFF;
  tft.fillRect(12, y + 8, 3, rowH - 16, barColor);

  // Light name
  readPgm(getLightName(selectedRoom, light), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(isOn ? C_TEXT : C_DIM);
  tft.setTextSize(2);
  tft.setCursor(22, y + rowH / 2 - 8);
  tft.print(pgmBuf);

  // Timer indicator
  if (hasTimer(selectedRoom, light)) {
    tft.setTextColor(C_TIMER);
    tft.setTextSize(1);
    tft.setCursor(22, y + rowH / 2 + 10);
    tft.print(F("timer"));
  }

  // Toggle pill
  drawToggleBtn(TOGGLE_X, y + (rowH - TOGGLE_H) / 2, isOn);
}

void drawToggleBtn(int16_t x, int16_t y, bool isOn) {
  if (isOn) {
    // Filled amber pill
    tft.fillRoundRect(x, y, TOGGLE_W, TOGGLE_H, TOGGLE_H / 2, C_ON);
    tft.setTextColor(C_BG);
  } else {
    // Outlined gray pill
    tft.fillRoundRect(x, y, TOGGLE_W, TOGGLE_H, TOGGLE_H / 2, C_OFF);
    tft.drawRoundRect(x, y, TOGGLE_W, TOGGLE_H, TOGGLE_H / 2, C_DIM);
    tft.setTextColor(C_DIM);
  }
  tft.setTextSize(1);
  printCentered(isOn ? F("ON") : F("OFF"), x + TOGGLE_W / 2, y + 10);
}

// ---- Timer Set Screen ----
void drawTimerSetScreen() {
  currentScreen = SCR_TIMER_SET;
  tft.fillScreen(C_BG);
  drawHeader(F("Set Timer"), true);

  // Light info
  readPgm(getRoomName(selectedRoom), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(C_DIM);
  tft.setTextSize(1);
  tft.setCursor(15, 52);
  tft.print(pgmBuf);

  readPgm(getLightName(selectedRoom, selectedLight), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(C_TEXT);
  tft.setTextSize(2);
  tft.setCursor(15, 68);
  tft.print(pgmBuf);

  // Divider
  tft.drawFastHLine(15, 92, SCR_W - 30, C_BORDER);

  drawTimerAction();
  drawTimerHours();
  drawTimerMinutes();

  // START button — filled accent
  int16_t btnY = 262;
  tft.fillRoundRect(15, btnY, 100, 42, 10, C_ON);
  tft.setTextColor(C_BG);
  tft.setTextSize(2);
  printCentered(F("START"), 65, btnY + 13);

  // CANCEL button — outlined
  tft.drawRoundRect(125, btnY, 100, 42, 10, C_DIM);
  tft.setTextColor(C_DIM);
  printCentered(F("CANCEL"), 175, btnY + 13);
}

void drawTimerAction() {
  int16_t y = 100;
  tft.fillRect(15, y, SCR_W - 30, 40, C_BG);

  tft.setTextColor(C_DIM);
  tft.setTextSize(1);
  tft.setCursor(15, y + 12);
  tft.print(F("Action:"));

  // Pill toggle
  uint16_t color = tmpTimerAction ? C_ON : C_OFF;
  tft.fillRoundRect(90, y + 4, 125, 32, 16, color);
  tft.setTextColor(tmpTimerAction ? C_BG : C_DIM);
  tft.setTextSize(2);
  printCentered(tmpTimerAction ? F("Turn ON") : F("Turn OFF"), 152, y + 10);
}

void drawTimerHours() {
  int16_t y = 150;
  tft.fillRect(15, y, SCR_W - 30, 44, C_BG);

  tft.setTextColor(C_DIM);
  tft.setTextSize(1);
  tft.setCursor(15, y + 16);
  tft.print(F("Hours:"));

  // [-]
  tft.fillRoundRect(88, y + 4, 38, 38, 8, C_CARD);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(3);
  printCentered(F("-"), 107, y + 10);

  // Value
  char buf[4];
  snprintf(buf, sizeof(buf), "%02d", tmpTimerHours);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(3);
  printCentered(buf, 152, y + 10);

  // [+]
  tft.fillRoundRect(178, y + 4, 38, 38, 8, C_CARD);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(3);
  printCentered(F("+"), 197, y + 10);
}

void drawTimerMinutes() {
  int16_t y = 204;
  tft.fillRect(15, y, SCR_W - 30, 44, C_BG);

  tft.setTextColor(C_DIM);
  tft.setTextSize(1);
  tft.setCursor(15, y + 16);
  tft.print(F("Mins:"));

  // [-]
  tft.fillRoundRect(88, y + 4, 38, 38, 8, C_CARD);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(3);
  printCentered(F("-"), 107, y + 10);

  // Value
  char buf[4];
  snprintf(buf, sizeof(buf), "%02d", tmpTimerMins);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(3);
  printCentered(buf, 152, y + 10);

  // [+]
  tft.fillRoundRect(178, y + 4, 38, 38, 8, C_CARD);
  tft.setTextColor(C_TEXT);
  tft.setTextSize(3);
  printCentered(F("+"), 197, y + 10);
}

// ---- Timer List Screen ----
void drawTimerListScreen() {
  currentScreen = SCR_TIMER_LIST;
  tft.fillScreen(C_BG);
  drawHeader(F("Timers"), true);

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
  int16_t y = 52 + row * 56;

  tft.fillRoundRect(8, y, SCR_W - 16, 50, 8, C_CARD);

  // Accent bar
  uint16_t accentC = timers[timerIdx].action ? C_ON : C_OFF;
  tft.fillRect(12, y + 10, 3, 30, accentC);

  // Light name
  readPgm(getLightName(timers[timerIdx].room, timers[timerIdx].light), pgmBuf, sizeof(pgmBuf));
  tft.setTextColor(C_TEXT);
  tft.setTextSize(1);
  tft.setCursor(22, y + 10);
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
  tft.setCursor(22, y + 24);
  tft.print(timers[timerIdx].action ? F("ON ") : F("OFF "));
  char timeBuf[10];
  snprintf(timeBuf, sizeof(timeBuf), "%dm%ds", mins, secs);
  tft.print(timeBuf);

  // Cancel [X]
  tft.drawRoundRect(SCR_W - 46, y + 12, 32, 28, 6, C_DANGER);
  tft.setTextColor(C_DANGER);
  tft.setTextSize(2);
  printCentered(F("X"), SCR_W - 30, y + 18);
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
