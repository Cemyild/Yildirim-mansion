// ================================================================
//  Touch Input Handling
// ================================================================

void handleTouch() {
  TSPoint tp = ts.getPoint();

  // CRITICAL: restore pin modes after reading touch
  // The touchscreen shares analog pins with the LCD
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (tp.z < TS_PRESSURE_MIN || tp.z > TS_PRESSURE_MAX) return;

  // Debounce
  unsigned long now = millis();
  if (now - lastTouchTime < TOUCH_DEBOUNCE) return;
  lastTouchTime = now;

  // Map raw touch to screen pixels
  // tp.x → screen X (left/right), tp.y → screen Y (top/bottom)
  int16_t px = map(tp.x, TS_LEFT, TS_RIGHT, 0, SCR_W);
  int16_t py = map(tp.y, TS_TOP, TS_BOTTOM, 0, SCR_H);

  // Clamp to screen bounds
  px = constrain(px, 0, SCR_W - 1);
  py = constrain(py, 0, SCR_H - 1);

  // Dispatch to current screen handler
  switch (currentScreen) {
    case SCR_HOME:       touchHome(px, py);      break;
    case SCR_ROOM:       touchRoom(px, py);      break;
    case SCR_TIMER_SET:  touchTimerSet(px, py);  break;
    case SCR_TIMER_LIST: touchTimerList(px, py); break;
  }
}

// ---- Hit test helper ----
bool inRect(int16_t px, int16_t py, int16_t x, int16_t y, int16_t w, int16_t h) {
  return (px >= x && px < x + w && py >= y && py < y + h);
}

// ================================================================
//  HOME SCREEN TOUCH
// ================================================================
void touchHome(int16_t px, int16_t py) {
  // Check room cards
  for (uint8_t i = 0; i < NUM_ROOMS; i++) {
    uint8_t col = i % 2;
    uint8_t row = i / 2;
    int16_t x = (col == 0) ? GRID_X1 : GRID_X2;
    int16_t y = (row == 0) ? GRID_Y1 : GRID_Y2;

    if (inRect(px, py, x, y, GRID_W, GRID_H)) {
      selectedRoom = i;
      drawRoomScreen();
      return;
    }
  }

  // Bottom bar (3 buttons: 72px wide, 6px gap, starting at x=6)
  if (py >= BAR_Y) {
    int16_t y = BAR_Y + 5;
    if (inRect(px, py, 6, y, 72, 38)) {
      setAllLights(1);
      drawHomeScreen();
    } else if (inRect(px, py, 84, y, 72, 38)) {
      setAllLights(0);
      drawHomeScreen();
    } else if (inRect(px, py, 162, y, 72, 38)) {
      drawTimerListScreen();
    }
  }
}

// ================================================================
//  ROOM SCREEN TOUCH
// ================================================================
void touchRoom(int16_t px, int16_t py) {
  // Back button
  if (inRect(px, py, 4, 4, 52, 38)) {
    drawHomeScreen();
    return;
  }

  // Light toggle buttons and rows
  uint8_t count = getLightCount(selectedRoom);
  for (uint8_t i = 0; i < count; i++) {
    int16_t y = LIGHT_Y_START + i * LIGHT_ROW_H;
    int16_t rowH = LIGHT_ROW_H - 6;
    int16_t toggleY = y + (rowH - TOGGLE_H) / 2;

    // Toggle ON/OFF button
    if (inRect(px, py, TOGGLE_X, toggleY, TOGGLE_W, TOGGLE_H)) {
      toggleLight(selectedRoom, i);
      drawLightRow(i);
      return;
    }

    // Tap on the row (not the toggle) to set timer
    if (inRect(px, py, 6, y, TOGGLE_X - 10, rowH)) {
      selectedLight = i;
      tmpTimerHours = 0;
      tmpTimerMins = 30;
      tmpTimerAction = lightState[selectedRoom][i] ? 0 : 1;
      drawTimerSetScreen();
      return;
    }
  }

  // Bottom bar
  if (py >= BAR_Y) {
    int16_t y = BAR_Y + 5;
    if (inRect(px, py, 6, y, 72, 38)) {
      setRoomLights(selectedRoom, 1);
      drawRoomScreen();
    } else if (inRect(px, py, 84, y, 72, 38)) {
      setRoomLights(selectedRoom, 0);
      drawRoomScreen();
    } else if (inRect(px, py, 162, y, 72, 38)) {
      drawTimerListScreen();
    }
  }
}

// ================================================================
//  TIMER SET SCREEN TOUCH
// ================================================================
void touchTimerSet(int16_t px, int16_t py) {
  // Back button
  if (inRect(px, py, 4, 4, 52, 38)) {
    drawRoomScreen();
    return;
  }

  // Action toggle (y=100, h=34)
  if (inRect(px, py, 90, 102, 125, 34)) {
    tmpTimerAction = !tmpTimerAction;
    drawTimerAction();
    return;
  }

  // Hours [-] (y=150)
  if (inRect(px, py, 90, 152, 38, 38)) {
    if (tmpTimerHours > 0) tmpTimerHours--;
    drawTimerHours();
    return;
  }
  // Hours [+]
  if (inRect(px, py, 182, 152, 38, 38)) {
    if (tmpTimerHours < 23) tmpTimerHours++;
    drawTimerHours();
    return;
  }

  // Minutes [-] (y=202)
  if (inRect(px, py, 90, 204, 38, 38)) {
    if (tmpTimerMins > 0) tmpTimerMins -= 5;
    else if (tmpTimerHours > 0) { tmpTimerHours--; tmpTimerMins = 55; drawTimerHours(); }
    drawTimerMinutes();
    return;
  }
  // Minutes [+]
  if (inRect(px, py, 182, 204, 38, 38)) {
    tmpTimerMins += 5;
    if (tmpTimerMins >= 60) { tmpTimerMins = 0; tmpTimerHours++; drawTimerHours(); }
    drawTimerMinutes();
    return;
  }

  // START button (y=260, h=44)
  if (inRect(px, py, 15, 260, 100, 44)) {
    unsigned long delayMs = ((unsigned long)tmpTimerHours * 3600 + (unsigned long)tmpTimerMins * 60) * 1000;
    if (delayMs > 0) {
      addTimer(selectedRoom, selectedLight, tmpTimerAction, delayMs);
    }
    drawRoomScreen();
    return;
  }

  // CANCEL button
  if (inRect(px, py, 125, 260, 100, 44)) {
    drawRoomScreen();
    return;
  }
}

// ================================================================
//  TIMER LIST SCREEN TOUCH
// ================================================================
void touchTimerList(int16_t px, int16_t py) {
  // Back button
  if (inRect(px, py, 4, 4, 52, 38)) {
    if (currentScreen == SCR_TIMER_LIST) {
      drawHomeScreen();
    }
    return;
  }

  // Cancel buttons [X] on timer rows
  uint8_t row = 0;
  for (uint8_t i = 0; i < MAX_TIMERS && row < 4; i++) {
    if (!timers[i].active) continue;
    int16_t y = 55 + row * 55;

    if (inRect(px, py, SCR_W - 48, y + 8, 38, 32)) {
      cancelTimer(i);
      drawTimerListScreen();
      return;
    }
    row++;
  }
}
