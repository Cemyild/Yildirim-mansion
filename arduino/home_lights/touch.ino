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

  // Map touch coordinates to screen pixels
  // NOTE: x/y mapping depends on your shield and rotation.
  // If touch seems mirrored or rotated, swap tp.x/tp.y
  // or change the map() ranges below.
  int16_t px = map(tp.y, TS_LEFT, TS_RIGHT, 0, SCR_W);
  int16_t py = map(tp.x, TS_TOP, TS_BOTTOM, 0, SCR_H);

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

  // Bottom bar
  if (py >= BAR_Y) {
    uint8_t btnW = 73;
    int16_t y = BAR_Y + 5;

    if (inRect(px, py, 8, y, btnW, 35)) {
      // ALL ON
      setAllLights(1);
      drawHomeScreen();
    } else if (inRect(px, py, 8 + btnW + 5, y, btnW, 35)) {
      // ALL OFF
      setAllLights(0);
      drawHomeScreen();
    } else if (inRect(px, py, 8 + (btnW + 5) * 2, y, btnW, 35)) {
      // TIMERS
      drawTimerListScreen();
    }
  }
}

// ================================================================
//  ROOM SCREEN TOUCH
// ================================================================
void touchRoom(int16_t px, int16_t py) {
  // Back button
  if (inRect(px, py, 5, 5, 40, 30)) {
    drawHomeScreen();
    return;
  }

  // Light toggle buttons
  uint8_t count = getLightCount(selectedRoom);
  for (uint8_t i = 0; i < count; i++) {
    int16_t y = LIGHT_Y_START + i * LIGHT_ROW_H;

    // Toggle ON/OFF button
    if (inRect(px, py, TOGGLE_X, y + 8, TOGGLE_W, TOGGLE_H)) {
      toggleLight(selectedRoom, i);
      drawLightRow(i);
      return;
    }

    // Tap on the row (not the toggle) to set timer for this light
    if (inRect(px, py, 8, y, TOGGLE_X - 15, LIGHT_ROW_H - 5)) {
      selectedLight = i;
      tmpTimerHours = 0;
      tmpTimerMins = 30;
      tmpTimerAction = lightState[selectedRoom][i] ? 0 : 1; // opposite of current
      drawTimerSetScreen();
      return;
    }
  }

  // Bottom bar
  if (py >= BAR_Y) {
    uint8_t btnW = 73;
    int16_t y = BAR_Y + 5;

    if (inRect(px, py, 8, y, btnW, 35)) {
      // ALL ON in this room
      setRoomLights(selectedRoom, 1);
      drawRoomScreen();
    } else if (inRect(px, py, 8 + btnW + 5, y, btnW, 35)) {
      // ALL OFF in this room
      setRoomLights(selectedRoom, 0);
      drawRoomScreen();
    } else if (inRect(px, py, 8 + (btnW + 5) * 2, y, btnW, 35)) {
      // TIMERS
      drawTimerListScreen();
    }
  }
}

// ================================================================
//  TIMER SET SCREEN TOUCH
// ================================================================
void touchTimerSet(int16_t px, int16_t py) {
  // Back button
  if (inRect(px, py, 5, 5, 40, 30)) {
    drawRoomScreen();
    return;
  }

  // Action toggle
  if (inRect(px, py, 90, 90, 120, 28)) {
    tmpTimerAction = !tmpTimerAction;
    drawTimerAction();
    return;
  }

  // Hours [-]
  if (inRect(px, py, 90, 142, 35, 35)) {
    if (tmpTimerHours > 0) tmpTimerHours--;
    drawTimerHours();
    return;
  }
  // Hours [+]
  if (inRect(px, py, 175, 142, 35, 35)) {
    if (tmpTimerHours < 23) tmpTimerHours++;
    drawTimerHours();
    return;
  }

  // Minutes [-]
  if (inRect(px, py, 90, 197, 35, 35)) {
    if (tmpTimerMins > 0) tmpTimerMins -= 5;
    else if (tmpTimerHours > 0) { tmpTimerHours--; tmpTimerMins = 55; drawTimerHours(); }
    drawTimerMinutes();
    return;
  }
  // Minutes [+]
  if (inRect(px, py, 175, 197, 35, 35)) {
    tmpTimerMins += 5;
    if (tmpTimerMins >= 60) { tmpTimerMins = 0; tmpTimerHours++; drawTimerHours(); }
    drawTimerMinutes();
    return;
  }

  // START button
  if (inRect(px, py, 15, 260, 100, 40)) {
    unsigned long delayMs = ((unsigned long)tmpTimerHours * 3600 + (unsigned long)tmpTimerMins * 60) * 1000;
    if (delayMs > 0) {
      addTimer(selectedRoom, selectedLight, tmpTimerAction, delayMs);
    }
    drawRoomScreen();
    return;
  }

  // CANCEL button
  if (inRect(px, py, 125, 260, 100, 40)) {
    drawRoomScreen();
    return;
  }
}

// ================================================================
//  TIMER LIST SCREEN TOUCH
// ================================================================
void touchTimerList(int16_t px, int16_t py) {
  // Back button
  if (inRect(px, py, 5, 5, 40, 30)) {
    if (currentScreen == SCR_TIMER_LIST) {
      drawHomeScreen();
    }
    return;
  }

  // Cancel buttons [X] on timer rows
  uint8_t row = 0;
  for (uint8_t i = 0; i < MAX_TIMERS && row < 4; i++) {
    if (!timers[i].active) continue;
    int16_t y = 50 + row * 60;

    if (inRect(px, py, SCR_W - 50, y + 10, 35, 30)) {
      cancelTimer(i);
      drawTimerListScreen();
      return;
    }
    row++;
  }
}
