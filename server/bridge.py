"""
Yildirim Mansion - Bridge Server
Receives HTTP requests from Arduino/ESP-01 and controls
Tuya/eWeLink smart home devices on the local network.

Run: python bridge.py
Requires: pip install flask tinytuya
"""

import json
import os
from flask import Flask, jsonify, request

app = Flask(__name__)

# Load device configuration
CONFIG_PATH = os.path.join(os.path.dirname(__file__), "config.json")


def load_config():
    with open(CONFIG_PATH, "r") as f:
        return json.load(f)


def get_tuya_device(dev_config):
    """Create a tinytuya device connection."""
    try:
        import tinytuya

        d = tinytuya.OutletDevice(
            dev_id=dev_config["tuya_id"],
            address=dev_config["ip"],
            local_key=dev_config["local_key"],
        )
        d.set_version(dev_config.get("version", 3.3))
        return d
    except ImportError:
        print("tinytuya not installed. Install with: pip install tinytuya")
        return None


@app.route("/api/status", methods=["GET"])
def get_status():
    """Return current state of all devices."""
    config = load_config()
    devices = []

    for dev_id, dev_config in config["devices"].items():
        state = False

        if dev_config.get("type") == "tuya":
            device = get_tuya_device(dev_config)
            if device:
                try:
                    data = device.status()
                    # DPS 1 is typically the power switch for Tuya devices
                    dps_key = dev_config.get("dps_key", "1")
                    state = data.get("dps", {}).get(dps_key, False)
                except Exception as e:
                    print(f"Error reading {dev_id}: {e}")

        elif dev_config.get("type") == "virtual":
            # Virtual device for testing without real hardware
            state = dev_config.get("state", False)

        devices.append({"id": dev_id, "state": state})

    return jsonify({"devices": devices})


@app.route("/api/toggle", methods=["POST"])
def toggle_device():
    """Toggle a device on or off."""
    data = request.get_json()
    if not data or "device_id" not in data:
        return jsonify({"error": "device_id required"}), 400

    dev_id = data["device_id"]
    new_state = data.get("state", True)

    config = load_config()

    if dev_id not in config["devices"]:
        return jsonify({"error": "device not found"}), 404

    dev_config = config["devices"][dev_id]
    success = False

    if dev_config.get("type") == "tuya":
        device = get_tuya_device(dev_config)
        if device:
            try:
                dps_key = dev_config.get("dps_key", "1")
                device.set_status(new_state, switch=int(dps_key))
                success = True
            except Exception as e:
                print(f"Error toggling {dev_id}: {e}")
                return jsonify({"error": str(e)}), 500

    elif dev_config.get("type") == "virtual":
        # Update virtual device state in memory
        config["devices"][dev_id]["state"] = new_state
        with open(CONFIG_PATH, "w") as f:
            json.dump(config, f, indent=2)
        success = True

    return jsonify({"device_id": dev_id, "state": new_state, "success": success})


@app.route("/", methods=["GET"])
def index():
    """Health check and info."""
    return jsonify(
        {
            "name": "Yildirim Mansion Bridge",
            "status": "running",
            "endpoints": [
                "GET  /api/status  - Get all device states",
                "POST /api/toggle  - Toggle device {device_id, state}",
            ],
        }
    )


if __name__ == "__main__":
    if not os.path.exists(CONFIG_PATH):
        print(f"ERROR: {CONFIG_PATH} not found!")
        print("Copy config_example.json to config.json and edit it with your device details.")
        exit(1)

    print("Yildirim Mansion Bridge Server")
    print("Listening on http://0.0.0.0:5000")
    app.run(host="0.0.0.0", port=5000, debug=False)
