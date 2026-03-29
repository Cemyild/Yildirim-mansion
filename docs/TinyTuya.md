# TinyTuya - Quick Reference

Python library for LOCAL control of Tuya/Smart Life devices (no cloud needed).
GitHub: https://github.com/jasonacox/tinytuya
PyPI: `pip install tinytuya`

## Getting Device Keys

Before you can control devices locally, you need each device's **ID**, **IP**, and **local key**.

### Method 1: TinyTuya Wizard (Recommended)

```bash
pip install tinytuya
python -m tinytuya wizard
```

The wizard will:
1. Ask for your Tuya IoT Platform credentials
2. Scan your network for Tuya devices
3. Output a `devices.json` with all IDs, IPs, and keys

### Method 2: Tuya IoT Platform

1. Go to https://iot.tuya.com and create an account
2. Create a Cloud Project (select your region)
3. Link your Smart Life / Tuya app account
4. Go to Devices → find your device → copy Device ID
5. Go to API Explorer → run `Get Device Information` → find `local_key`
6. Find device IP from your router's DHCP table

## Basic Usage

```python
import tinytuya

# Connect to device
d = tinytuya.OutletDevice(
    dev_id='DEVICE_ID_HERE',
    address='192.168.1.50',
    local_key='LOCAL_KEY_HERE'
)
d.set_version(3.3)  # Most devices use 3.3, some older ones use 3.1

# Get status
data = d.status()
print(data)
# {'dps': {'1': True, '2': 0, '3': 255}}

# Turn ON
d.turn_on()        # equivalent to d.set_status(True)

# Turn OFF
d.turn_off()       # equivalent to d.set_status(False)

# Set specific DPS value
d.set_value(1, True)   # DPS 1 = power switch
d.set_value(2, 128)    # DPS 2 = brightness (device dependent)
```

## Device Types

```python
# Smart plug / switch
d = tinytuya.OutletDevice(dev_id, address, local_key)

# Light bulb
d = tinytuya.BulbDevice(dev_id, address, local_key)
d.turn_on()
d.set_brightness(50)      # 0-100
d.set_white(255, 100)     # brightness, color temp
d.set_colour(255, 0, 0)   # RGB

# Generic device
d = tinytuya.Device(dev_id, address, local_key)
```

## Common DPS Keys

DPS (Data Point Set) keys vary by device, but common ones:

| DPS | Typical Function |
|-----|-----------------|
| 1 | Power On/Off (bool) |
| 2 | Mode / Brightness |
| 3 | Brightness (0-255 or 0-1000) |
| 4 | Color Temperature |
| 5 | Color (HSV hex string) |

Check your device's DPS with `d.status()`.

## Network Scanner

Find Tuya devices on your network:

```python
import tinytuya
devices = tinytuya.deviceScan()
for ip, info in devices.items():
    print(f"IP: {ip}, ID: {info['gwId']}, Version: {info['version']}")
```

## Protocol Versions

| Version | Notes |
|---------|-------|
| 3.1 | Older devices, unencrypted |
| 3.3 | Most common, encrypted |
| 3.4 | Newer devices, enhanced encryption |
| 3.5 | Latest, used by some new devices |

Always try 3.3 first. If it doesn't work, try 3.4 or 3.1.

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Connection timeout | Device IP may have changed - rescan network |
| `No response` | Wrong local_key, or device protocol version mismatch |
| `Network unreachable` | Device and server must be on same LAN |
| DPS values don't match | Run `d.status()` to discover actual DPS keys for your device |
| Key expired after OTA update | Some firmware updates change the local_key - re-run wizard |

## Keeping IPs Stable

Tuya devices get IP addresses via DHCP and they can change. Solutions:
- Set static IP/DHCP reservation in your router for each device
- Use `tinytuya.deviceScan()` to re-discover IPs periodically
