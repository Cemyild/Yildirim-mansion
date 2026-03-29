// ================================================================
//  ESP-01 WiFi Communication (AT Commands)
//  Only compiled when NETWORK_ENABLED is set to 1 in config.h
// ================================================================

#if NETWORK_ENABLED

// Send AT command and wait for expected response
bool espSendCmd(const __FlashStringHelper* cmd, const char* expected, unsigned long timeout) {
  espSerial.println(cmd);

  unsigned long start = millis();
  String response = "";

  while (millis() - start < timeout) {
    if (espSerial.available()) {
      char c = espSerial.read();
      response += c;
      if (response.indexOf(expected) >= 0) return true;
    }
  }

  Serial.print(F("ESP timeout for: "));
  Serial.println(cmd);
  return false;
}

// Initialize WiFi connection
void initNetwork() {
  Serial.println(F("Connecting WiFi..."));

  // Reset ESP
  espSendCmd(F("AT+RST"), "ready", 5000);
  delay(1000);

  // Station mode
  espSendCmd(F("AT+CWMODE=1"), "OK", 3000);

  // Connect to WiFi
  espSerial.print(F("AT+CWJAP=\""));
  espSerial.print(F(WIFI_SSID));
  espSerial.print(F("\",\""));
  espSerial.print(F(WIFI_PASS));
  espSerial.println(F("\""));

  if (espSendCmd(F(""), "OK", 15000)) {
    wifiConnected = true;
    Serial.println(F("WiFi connected!"));
  } else {
    wifiConnected = false;
    Serial.println(F("WiFi FAILED"));
  }

  // Single connection mode
  espSendCmd(F("AT+CIPMUX=0"), "OK", 3000);
}

// Make HTTP GET request to bridge server
bool httpGet(const char* path, char* responseBuf, uint8_t bufLen) {
  if (!wifiConnected) return false;

  // Connect to server
  espSerial.print(F("AT+CIPSTART=\"TCP\",\""));
  espSerial.print(F(SERVER_IP));
  espSerial.print(F("\","));
  espSerial.println(SERVER_PORT);

  if (!espSendCmd(F(""), "OK", 5000)) return false;

  // Build request
  char request[80];
  snprintf(request, sizeof(request), "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
           path, SERVER_IP);

  uint8_t len = strlen(request);
  espSerial.print(F("AT+CIPSEND="));
  espSerial.println(len);

  if (!espSendCmd(F(""), ">", 3000)) return false;

  espSerial.print(request);

  // Read response (simplified - look for JSON body)
  unsigned long start = millis();
  bool bodyStarted = false;
  uint8_t idx = 0;
  String fullResp = "";

  while (millis() - start < 5000) {
    if (espSerial.available()) {
      char c = espSerial.read();
      fullResp += c;

      if (!bodyStarted && fullResp.endsWith("\r\n\r\n")) {
        bodyStarted = true;
        continue;
      }

      if (bodyStarted && idx < bufLen - 1) {
        responseBuf[idx++] = c;
      }
    }
  }
  responseBuf[idx] = '\0';
  return idx > 0;
}

// Make HTTP POST request to toggle a light
bool httpPostToggle(const char* deviceId, uint8_t state) {
  if (!wifiConnected) return false;

  // Connect
  espSerial.print(F("AT+CIPSTART=\"TCP\",\""));
  espSerial.print(F(SERVER_IP));
  espSerial.print(F("\","));
  espSerial.println(SERVER_PORT);

  if (!espSendCmd(F(""), "OK", 5000)) return false;

  // Build JSON body
  char body[60];
  snprintf(body, sizeof(body), "{\"device_id\":\"%s\",\"state\":%s}", deviceId, state ? "true" : "false");

  uint8_t bodyLen = strlen(body);

  // Build full request
  char request[160];
  snprintf(request, sizeof(request),
           "POST /api/toggle HTTP/1.1\r\nHost: %s\r\nContent-Type: application/json\r\nContent-Length: %d\r\nConnection: close\r\n\r\n%s",
           SERVER_IP, bodyLen, body);

  uint8_t len = strlen(request);
  espSerial.print(F("AT+CIPSEND="));
  espSerial.println(len);

  if (!espSendCmd(F(""), ">", 3000)) return false;

  espSerial.print(request);

  return espSendCmd(F(""), "200 OK", 5000);
}

// Send toggle command for a specific light
void sendToggle(uint8_t room, uint8_t light, uint8_t state) {
  readPgm(getDeviceId(room, light), pgmBuf, sizeof(pgmBuf));
  httpPostToggle(pgmBuf, state);
}

// Refresh all light states from bridge server
void refreshStates() {
  char resp[200];
  if (!httpGet("/api/status", resp, sizeof(resp))) return;

  // Simple JSON parsing - expects format:
  // {"devices":[{"id":"dev_xxx","state":true},{"id":"dev_yyy","state":false}]}
  // This is a minimal parser for Arduino's limited resources

  char* pos = resp;
  while ((pos = strstr(pos, "\"id\":\"")) != NULL) {
    pos += 6;
    char* idEnd = strchr(pos, '"');
    if (!idEnd) break;

    *idEnd = '\0';
    char foundId[25];
    strncpy(foundId, pos, sizeof(foundId) - 1);
    foundId[sizeof(foundId) - 1] = '\0';
    *idEnd = '"';
    pos = idEnd + 1;

    // Find state
    char* statePos = strstr(pos, "\"state\":");
    if (!statePos) break;
    statePos += 8;
    bool stateVal = (*statePos == 't');

    // Match device ID to room/light
    for (uint8_t r = 0; r < NUM_ROOMS; r++) {
      uint8_t count = getLightCount(r);
      for (uint8_t l = 0; l < count; l++) {
        readPgm(getDeviceId(r, l), pgmBuf, sizeof(pgmBuf));
        if (strcmp(pgmBuf, foundId) == 0) {
          lightState[r][l] = stateVal ? 1 : 0;
        }
      }
    }
  }

  // Redraw current screen
  if (currentScreen == SCR_HOME) drawHomeScreen();
  else if (currentScreen == SCR_ROOM) drawRoomScreen();
}

#endif // NETWORK_ENABLED
