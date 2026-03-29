// ================================================================
//  Touch Input Handling
// ================================================================

void handleTouch() {
  TSPoint tp = ts.getPoint();

  // CRITICAL: restore pin modes after reading touch
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (tp.z < TS_PRESSURE_MIN || tp.z > TS_PRESSURE_MAX) return;

  // Debounce
  unsigned long now = millis();
  if (now - lastTouchTime < TOUCH_DEBOUNCE) return;
  lastTouchTime = now;

  // Map raw touch to screen pixels
  // tp.y = horizontal (high=left, low=right) → screen X
  // tp.x = vertical (high=top, low=bottom) → screen Y
  int16_t px = map(tp.y, TS_LEFT, TS_RIGHT, 0, SCR_W);
  int16_t py = map(tp.x, TS_TOP, TS_BOTTOM, 0, SCR_H);

  px = constrain(px, 0, SCR_W - 1);
  py = constrain(py, 0, SCR_H - 1);

  switch (currentScreen) {
    case SCR_HOME:       touchHome(px, py);      break;
    case SCR_ROOM:       touchRoom(px, py);      break;
    case SCR_TIMER_SET:  touchTimerSet(px, py);  break;
    case SCR_TIMER_LIST: touchTimerList(px, py); break;
  }
}

bool inRect(int16_t px, int16_t py, int16_t x, int16_t y, int16_t w, int16_t h) {
  return (px >= x && px < x + w && py >= y && py < y + h);
}

// ================================================================
//  HOME SCREEN TOUCH
// ================================================================
void touchHome(int16_t px, int16_t py) {
  // Room cards
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

  // Bottom bar (3 buttons: 72px wide, 5px gap, starting at x=6)
  if (py >= BAR_Y) {
    int16_t by = BAR_Y + 8;
    if (inRect(px, py, 6, by, 72, 28)) {
      setAllLights(1);
      drawHomeScreen();
    } else if (inRect(px, py, 83, by, 72, 28)) {
      setAllLights(0);
      drawHomeScreen();
    } else if (inRect(px, py, 160, by, 72, 28)) {
      drawTimerListScreen();
    }
  }
}

// ================================================================
//  ROOM SCREEN TOUCH
// ================================================================
void touchRoom(int16_t px, int16_t py) {
  // Back button (< arrow area at top-left)
  if (inRect(px, py, 0, 0, 38, HDR_H)) {
    drawHomeScreen();
    return;
  }

  // Light rows
  uint8_t count = getLightCount(selectedRoom);
  for (uint8_t i = 0; i < count; i++) {
    int16_t y = LIGHT_Y_START + i * LIGHT_ROW_H;
    int16_t rowH = LIGHT_ROW_H - 6;
    int16_t toggleY = y + (rowH - TOGGLE_H) / 2;

    // Toggle button
    if (inRect(px, py, TOGGLE_X, toggleY, TOGGLE_W, TOGGLE_H)) {
      toggleLight(selectedRoom, i);
      drawLightRow(i);
      return;
    }

    // Tap row name area → set timer
    if (inRect(px, py, 8, y, TOGGLE_X - 12, rowH)) {
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
    int16_t by = BAR_Y + 8;
    if (inRect(px, py, 6, by, 72, 28)) {
      setRoomLights(selectedRoom, 1);
      drawRoomScreen();
    } else if (inRect(px, py, 83, by, 72, 28)) {
      setRoomLights(selectedRoom, 0);
      drawRoomScreen();
    } else if (inRect(px, py, 160, by, 72, 28)) {
      drawTimerListScreen();
    }
  }
}

// ================================================================
//  TIMER SET SCREEN TOUCH
// ================================================================
void touchTimerSet(int16_t px, int16_t py) {
  // Back
  if (inRect(px, py, 0, 0, 38, HDR_H)) {
    drawRoomScreen();
    return;
  }

  // Action toggle (y=100)
  if (inRect(px, py, 90, 104, 125, 32)) {
    tmpTimerAction = !tmpTimerAction;
    drawTimerAction();
    return;
  }

  // Hours [-] (y=150)
  if (inRect(px, py, 88, 154, 38, 38)) {
    if (tmpTimerHours > 0) tmpTimerHours--;
    drawTimerHours();
    return;
  }
  // Hours [+]
  if (inRect(px, py, 178, 154, 38, 38)) {
    if (tmpTimerHours < 23) tmpTimerHours++;
    drawTimerHours();
    return;
  }

  // Minutes [-] (y=204)
  if (inRect(px, py, 88, 208, 38, 38)) {
    if (tmpTimerMins > 0) tmpTimerMins -= 5;
    else if (tmpTimerHours > 0) { tmpTimerHours--; tmpTimerMins = 55; drawTimerHours(); }
    drawTimerMinutes();
    return;
  }
  // Minutes [+]
  if (inRect(px, py, 178, 208, 38, 38)) {
    tmpTimerMins += 5;
    if (tmpTimerMins >= 60) { tmpTimerMins = 0; tmpTimerHours++; drawTimerHours(); }
    drawTimerMinutes();
    return;
  }

  // START (y=262)
  if (inRect(px, py, 15, 262, 100, 42)) {
    unsigned long delayMs = ((unsigned long)tmpTimerHours * 3600 + (unsigned long)tmpTimerMins * 60) * 1000;
    if (delayMs > 0) {
      addTimer(selectedRoom, selectedLight, tmpTimerAction, delayMs);
    }
    drawRoomScreen();
    return;
  }

  // CANCEL
  if (inRect(px, py, 125, 262, 100, 42)) {
    drawRoomScreen();
    return;
  }
}

// ================================================================
//  TIMER LIST SCREEN TOUCH
// ================================================================
void touchTimerList(int16_t px, int16_t py) {
  // Back
  if (inRect(px, py, 0, 0, 38, HDR_H)) {
    drawHomeScreen();
    return;
  }

  // Cancel [X] buttons
  uint8_t row = 0;
  for (uint8_t i = 0; i < MAX_TIMERS && row < 4; i++) {
    if (!timers[i].active) continue;
    int16_t y = 52 + row * 56;

    if (inRect(px, py, SCR_W - 46, y + 12, 32, 28)) {
      cancelTimer(i);
      drawTimerListScreen();
      return;
    }
    row++;
  }
}
