# P2Button
The Pbutton project is designed to create a Bluetooth Low Energy (BLE) controlled video recording system specifically for Android platforms, primarily for indoor environments like gyms. This innovative system enables users to operate video recording functions through a robust, mounted button system, making it highly accessible and user-friendly.


## Overview
This project is a BLE (Bluetooth Low Energy) device setup for the P2CAM application, featuring a combination of functionalities including:
- BLE services and characteristics for communication.
- An OLED display for visual feedback.
- A buzzer and LEDs for status indication.
- Button interactions for triggering specific actions.

The code is designed to manage device states, interact with a BLE client, and provide visual/audio feedback for user interactions.

---

## Features
1. **BLE Communication**:
   - Implements multiple services and characteristics.
   - Supports read, write, indicate, and notify operations.
   - Handles commands like "Unlock", "Record", "Upload", "Login", and more.

2. **OLED Display**:
   - Displays the P2CAM logo, messages, and countdown timers.
   - Provides visual feedback during various states.

3. **Device States**:
   - The device transitions through states such as `NOT_PAIRED`, `LOGGED_LOCKED`, `CAMERA_UNLOCKED`, `RECORDING`, and more.

4. **LED Indications**:
   - Different LED colors indicate different device states.

5. **Buzzer Notifications**:
   - Provides audio feedback for specific actions.

6. **Button Interaction**:
   - Short and long press interactions trigger state changes and actions.

---

## Hardware Requirements
- **Microcontroller**: ESP32 or similar with BLE support.
- **OLED Display**: 128x64 resolution (I2C interface).
- **LEDs**: Blue, Green, and Red LEDs.
- **Buzzer**: For audio notifications.
- **Push Buttons**: For user interactions.
- **Battery Monitoring Pin**: For battery level feedback.

---

## Software Requirements
- **Arduino IDE** with the following libraries:
  - [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
  - [Adafruit SSD1306 Library](https://github.com/adafruit/Adafruit_SSD1306)
  - [BLEDevice Library](https://github.com/nkolban/esp32-snippets)
  - Wire library (default in Arduino IDE)

---

## Pin Configuration
| Component       | Pin   |
|------------------|-------|
| Blue LED         | 9     |
| Green LED        | 10    |
| Red LED          | 20    |
| Buzzer           | 5     |
| Button           | 21    |
| Unpair Button    | 3     |
| Battery Monitor  | 34    |
| OLED SDA         | 6     |
| OLED SCL         | 7     |

---

## BLE Setup
### Services
1. **Service 1 (UUID: 19b10000-e8f2-537e-4f6c-d104768a1214)**
   - Unlock Button
   - Unlock
   - Record Button
   - Record
   - Upload Button

2. **Service 2 (UUID: 19b20000-e8f2-537e-4f6c-d104768a1214)**
   - Upload
   - Setup
   - Login
   - Logout
   - Sleep

### Commands
The following commands are supported via BLE:
- `Login`: Log into the device.
- `Unlock`: Transition to the camera unlocked state.
- `Record`: Start recording.
- `Upload`: Start uploading data.
- `Logout`: Log out from the device.
- `Sleep`: Transition to sleep mode.

---

## Setup and Usage
1. **Hardware Setup**:
   - Connect the components as per the pin configuration table.
   - Ensure the OLED display is properly connected via I2C.

2. **Software Setup**:
   - Install the required libraries in Arduino IDE.
   - Compile and upload the code to your ESP32.

3. **Operation**:
   - Power on the device.
   - Pair with the BLE client (e.g., smartphone app).
   - Use the buttons for various actions.
   - Observe the OLED, LED, and buzzer for feedback.

4. **Resetting**:
   - Use the unpair button to reset the BLE pairing.

---

## Additional Information
- **OLED Bitmap**: Displays the P2CAM logo at startup.
- **Timers**:
  - Button holds initiate a countdown before actions.
- **Error Handling**:
  - Logs errors like failed BLE characteristic creation to the serial monitor.

---

## Troubleshooting
- **OLED Not Working**:
  - Verify the I2C address (default is `0x3C`).
  - Check SDA and SCL pin connections.

- **BLE Issues**:
  - Ensure BLE is enabled on the client device.
  - Restart advertising if connection drops.

- **No Buzzer Sound**:
  - Check buzzer pin connection and voltage.

---

## License
...
