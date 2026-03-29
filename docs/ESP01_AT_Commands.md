# ESP-01 AT Commands - Quick Reference

The ESP-01 (ESP8266) communicates via serial using AT commands.
Connect to Arduino via SoftwareSerial (pins 10/11).

## Wiring to Arduino UNO

| ESP-01 Pin | Connect To | Notes |
|------------|------------|-------|
| VCC | 3.3V regulator | NOT Arduino 3.3V pin (not enough current) |
| GND | GND | |
| TX | Arduino D10 | ESP sends data to Arduino |
| RX | Voltage divider → Arduino D11 | 5V→3.3V! Use 1K + 2K resistor divider |
| CH_PD (EN) | 3.3V via 10K pullup | Must be HIGH to enable chip |
| GPIO0 | 3.3V via 10K pullup | Must be HIGH for normal operation |
| GPIO2 | Leave floating or HIGH | |
| RST | 3.3V via 10K pullup | Or connect to Arduino pin for reset control |

### Voltage Divider for RX (5V → 3.3V)
```
Arduino D11 ---[1K ohm]---+---[2K ohm]--- GND
                           |
                       ESP-01 RX
```

## Basic AT Commands

| Command | Response | Description |
|---------|----------|-------------|
| `AT` | `OK` | Test communication |
| `AT+RST` | `ready` | Reset module |
| `AT+GMR` | Version info | Firmware version |
| `AT+CWMODE=1` | `OK` | Set to Station mode (client) |
| `AT+CWMODE=2` | `OK` | Set to AP mode (access point) |
| `AT+CWMODE=3` | `OK` | Set to both Station + AP |

## WiFi Connection

```
AT+CWJAP="SSID","PASSWORD"      → OK (connected) or FAIL
AT+CWQAP                        → Disconnect from WiFi
AT+CIFSR                        → Show IP address
AT+CWLAP                        → List available networks
```

## TCP Connection & HTTP Request

```
AT+CIPMUX=0                     → Single connection mode
AT+CIPSTART="TCP","192.168.1.100",5000  → Connect to server
AT+CIPSEND=<length>             → Prepare to send <length> bytes
> (send your HTTP request here)
AT+CIPCLOSE                     → Close connection
```

### Example: HTTP GET

```
AT+CIPSTART="TCP","192.168.1.100",5000
AT+CIPSEND=62
GET /api/status HTTP/1.1\r\nHost: 192.168.1.100\r\nConnection: close\r\n\r\n
```

### Example: HTTP POST

```
AT+CIPSTART="TCP","192.168.1.100",5000
AT+CIPSEND=<total_length>
POST /api/toggle HTTP/1.1\r\nHost: 192.168.1.100\r\nContent-Type: application/json\r\nContent-Length: <body_length>\r\nConnection: close\r\n\r\n{"device_id":"dev_living_ceiling","state":true}
```

## Arduino SoftwareSerial Example

```cpp
#include <SoftwareSerial.h>
SoftwareSerial esp(10, 11); // RX=10, TX=11

void setup() {
  Serial.begin(9600);
  esp.begin(9600);  // Default ESP-01 baud rate is 115200
                     // Flash with 9600 firmware or use AT+UART_DEF=9600,8,1,0,0
}

void loop() {
  // Forward ESP responses to Serial Monitor
  if (esp.available()) Serial.write(esp.read());
  // Forward Serial Monitor input to ESP
  if (Serial.available()) esp.write(Serial.read());
}
```

## Changing Baud Rate

Default ESP-01 baud rate is 115200, which is too fast for SoftwareSerial.
Change to 9600:

```
AT+UART_DEF=9600,8,1,0,0
```

Or flash the ESP-01 with 9600 baud firmware.

## Troubleshooting

| Problem | Solution |
|---------|----------|
| No response to AT | Check wiring, baud rate (try 115200), CH_PD must be HIGH |
| Garbage characters | Wrong baud rate - try 115200, 9600, 57600 |
| `busy p...` | Previous command still running, wait or reset |
| `FAIL` on CWJAP | Wrong SSID/password, or out of WiFi range |
| Resets randomly | Power issue - ESP needs ~300mA at peak, use external 3.3V regulator |
| `SEND FAIL` | Connection dropped, reconnect with CIPSTART |

## Power Requirements

The ESP-01 draws up to **300mA** during WiFi transmission. The Arduino 3.3V pin can only provide ~50mA. **You MUST use an external 3.3V regulator** (like AMS1117-3.3).

### Simple Power Circuit
```
Arduino 5V → AMS1117-3.3V → ESP-01 VCC
              |
             GND → ESP-01 GND
Add 10uF capacitor across 3.3V and GND
```
