// ================================================================
//  Countdown Timer Management
// ================================================================

// Add a new countdown timer
bool addTimer(uint8_t room, uint8_t light, uint8_t action, unsigned long delayMs) {
  // Check if there's already a timer for this light - replace it
  for (uint8_t i = 0; i < MAX_TIMERS; i++) {
    if (timers[i].active && timers[i].room == room && timers[i].light == light) {
      timers[i].action = action;
      timers[i].triggerAt = millis() + delayMs;
      return true;
    }
  }

  // Find empty slot
  for (uint8_t i = 0; i < MAX_TIMERS; i++) {
    if (!timers[i].active) {
      timers[i].room = room;
      timers[i].light = light;
      timers[i].action = action;
      timers[i].triggerAt = millis() + delayMs;
      timers[i].active = true;
      timerCount++;
      return true;
    }
  }

  return false; // no slots available
}

// Cancel a timer by index
void cancelTimer(uint8_t idx) {
  if (idx < MAX_TIMERS && timers[idx].active) {
    timers[idx].active = false;
    if (timerCount > 0) timerCount--;
  }
}

// Check if a specific light has an active timer
bool hasTimer(uint8_t room, uint8_t light) {
  for (uint8_t i = 0; i < MAX_TIMERS; i++) {
    if (timers[i].active && timers[i].room == room && timers[i].light == light) {
      return true;
    }
  }
  return false;
}

// Called every loop() - check and fire expired timers
void checkTimers() {
  unsigned long now = millis();

  for (uint8_t i = 0; i < MAX_TIMERS; i++) {
    if (!timers[i].active) continue;

    if (now >= timers[i].triggerAt) {
      // Timer fired!
      uint8_t room = timers[i].room;
      uint8_t light = timers[i].light;
      uint8_t action = timers[i].action;

      // Set the light state
      lightState[room][light] = action;

#if NETWORK_ENABLED
      sendToggle(room, light, action);
#endif

      // Deactivate timer
      timers[i].active = false;
      if (timerCount > 0) timerCount--;

      // Refresh screen if we're looking at the affected room
      if (currentScreen == SCR_ROOM && selectedRoom == room) {
        drawLightRow(light);
      } else if (currentScreen == SCR_HOME) {
        drawRoomCard(room);
      } else if (currentScreen == SCR_TIMER_LIST) {
        drawTimerListScreen();
      }

      Serial.print(F("Timer fired: room="));
      Serial.print(room);
      Serial.print(F(" light="));
      Serial.print(light);
      Serial.print(F(" action="));
      Serial.println(action);
    }
  }
}
