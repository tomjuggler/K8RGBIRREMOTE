# Bluetooth K8 IR RGB Remote

A wireless remote control for K8 Juggling Equipment using an ESP32C3 microcontroller. Features dual remote support (K8 and Chinese remotes) with OTA update capability.

## Features

- Web-based control interface accessible via WiFi with dual remote tabs
- IR signal transmission for controlling IR-enabled K8 equipment
- Built-in RGB LED for local feedback (optional)
- Captive portal support for Android, Apple, Firefox, Chrome/Windows devices
- Over-the-air (OTA) updates via ElegantOTA
- Pattern strobe effects with adjustable speed control
- Multi-color strobe patterns (red/blue, red/green, etc.)

## Hardware Requirements

- DFRobot Beetle ESP32C3 board (compatible with ESP32 C3 SuperMini)
- IR LED (for transmitting signals)
- Common cathode analog RGB LED (for local feedback)
- Resistors (appropriate for your LEDs)
- Power supply (USB or external)

## Wiring

### IR LED
- Connect the anode of the IR LED to pin 4 (kIrLedPin) 
- Connect the cathode to ground

### Analog RGB LED (Common Cathode)
- Connect the common cathode to ground
- Connect the red anode to pin 0 (rPin) 
- Connect the green anode to pin 1 (gPin) 
- Connect the blue anode to pin 2 (bPin) 

## Installation

1. Install PlatformIO (VSCode extension or standalone)
2. Clone this repository
3. Connect your DFRobot Beetle ESP32C3 (or C3 SuperMini) via USB
4. Build and upload the project

### PlatformIO Commands
```bash
# Build the project
pio run

# Upload to the board
pio run --target upload

# Build Filesystem
pio run --target buildfs

# Upload Filesystem
pio run --target uploadfs

# Monitor serial output
pio device monitor
```

## Usage

1. Power on the device
2. Connect to the "K8_RGB_IR_REMOTE" WiFi network from your device
3. Open a web browser and navigate to any page (captive portal will redirect to control interface) or directly visit http://192.168.4.1
4. Use the dual-tab interface:
   - **K8 Remote Tab**: Standard K8 remote commands (colors, effects, demo modes)
   - **Chinese Remote Tab**: Alternative remote protocol with additional features:
     * Color commands (Red, Green, Blue, White)
     * Brightness control (BRT Up/Down)
     * Power control (ON/OFF)
     * Effect modes (FLASH, STROBE, FADE, SMOOTH)
     * Pattern strobe effects (RB Strobe, RG Strobe, etc.) with adjustable speed control
5. Adjust pattern speed using the slider in the Chinese Remote tab (100-5000ms)

## OTA Updates

The device supports over-the-air updates via ElegantOTA:
- Access the OTA interface at http://192.168.4.1/update
- Upload new firmware files directly from your browser
- Progress is shown in the serial monitor

## Project Structure

- `src/main.cpp` - Main firmware code (IR commands, LED control, pattern handling)
- `src/tasks.cpp` - Web server, OTA updates, captive portal endpoints
- `src/tasks.h` - Header file with function declarations
- `platformio.ini` - PlatformIO configuration with library dependencies
- `data/index.html` - Web interface with dual remote tabs
- `data/script.js` - JavaScript for button interactions and speed control
- `data/style.css` - Styling for the web interface

## Serial Output

The device outputs status information to the serial port at 115200 baud:
- Startup messages
- Network information (AP IP, client count)
- Command acknowledgments for each IR signal sent
- OTA update progress and status
- Captive portal detection events

## Troubleshooting

- **Hardware Issues**:
  - Ensure all connections are secure and correct
  - Verify resistor values are appropriate for your LEDs
  - Check IR LED polarity and orientation (point at target device)

- **Network/Web Interface**:
  - If captive portal doesn't redirect, manually visit http://192.168.4.1
  - Some devices may require disabling mobile data for captive portal detection
  - Clear browser cache if interface appears broken
  - Ensure you're connected to "K8_RGB_IR_REMOTE" WiFi network

- **OTA Updates**:
  - Check serial monitor for OTA progress and error messages
  - Ensure firmware file is compatible with ESP32C3
  - Upload may fail if filesystem is full - check LittleFS usage
  - Device will reboot automatically after successful OTA update

- **IR Control**:
  - Check serial output for command acknowledgments
  - Ensure IR LED is working (test with phone camera)
  - Distance should be within 1-3 meters for reliable transmission

- **General Debugging**:
  - Monitor serial output at 115200 baud for detailed status
  - Reset device if web interface becomes unresponsive
  - Verify LittleFS filesystem is properly uploaded (data/ folder contents)

## License

This project is open source and available under the MIT License.
