# HouseAlarm

    

PROJECT DESCRIPTION
-------------------
This is a **complete Arduino-based Home Alarm System** with a custom 
**Processing GUI interface** for real-time monitoring and room-level 
visual alerts. It supports **entry/exit control**, **panic mode**, 
**night surveillance**, and multiple types of **intrusion detection**.

Two main components:
  - **Arduino Alarm Logic** (Embedded C++)
  - **Processing GUI** for Room Visualization

------------------------------------------------------------
FEATURE HIGHLIGHTS
------------------------------------------------------------

- **Keypad Access Control** with secure entry/exit codes
- **LCD Status Display** (16x2) for live feedback
- **IR Motion Sensor**, **Ultrasonic Distance**, **Sound Sensor**
- **MLX90614 Temperature Sensor** for human heat detection
- **Panic & Night Mode** buttons with buzzer alerts
- **Tamper Detection** for cut wires or intrusion attempts
- **Processing GUI** with animated zone alerts and serial control

------------------------------------------------------------
SECURITY CODES
------------------------------------------------------------

* Entry Code:        **1234**
* Exit Code:         **4321**
* Stop Alarm Code:   **CCCC**
* Clear Input:       **\*** (Asterisk key)

------------------------------------------------------------
HARDWARE REQUIREMENTS
------------------------------------------------------------

- **Arduino Uno** or compatible board
- **16x2 LCD Display** (parallel)
- **4x4 Matrix Keypad**
- **MLX90614 IR Temperature Sensor**
- **IR Sensor**, **Ultrasonic Sensor (HC-SR04)**
- **Analog Sound Sensor**
- **Push Buttons** for Panic/Night modes
- **Buzzers** for alerts
- Jumper wires, breadboard or custom PCB

------------------------------------------------------------
SOFTWARE & LIBRARIES
------------------------------------------------------------

*Arduino IDE Libraries:*
  - LiquidCrystal
  - Keypad
  - Wire
  - Adafruit_MLX90614

*Processing IDE:*
  - processing.serial.*

------------------------------------------------------------
SETUP INSTRUCTIONS
------------------------------------------------------------

1. Upload the Arduino sketch using Arduino IDE.
2. Open the Processing sketch (.pde) and set your **serial port name**.
3. Run the Processing GUI.
4. Use the **keypad** to enter codes, and physical buttons to control modes.
5. The Processing GUI will **flash zones** based on alerts received via serial.

------------------------------------------------------------
SERIAL COMMUNICATION EVENTS
------------------------------------------------------------

* "Entry Granted"             → Flash Entry Zone
* "Exit Granted"              → Flash Exit Zone
* "Panic Zone Activated"      → Flash Panic Zone
* "Night Zone Activated"      → Flash Night Zone
* "Alarm Activated!"          → Flash Front Door
* "Alarm Stopped"             → Stop Front Door Flash
* "Sound Detected in Night Zone!" → Flash Kitchen/Dining
* "Night Zone Deactivated"    → Stop Kitchen Flash
* "IR Intruder!"              → Flash Washroom
* "IR Stopped"                → Stop Washroom Flash

------------------------------------------------------------
FILE STRUCTURE
------------------------------------------------------------

House-Alarm-System/
├── **Arduino_Code.ino**        ← Arduino logic (C++)
├── **RoomVisualizer.pde**      ← Processing GUI sketch
├── **README.txt**              ← This file

------------------------------------------------------------
TROUBLESHOOTING
------------------------------------------------------------

- Ensure correct **COM port** is used in Processing sketch.
- Use **INPUT_PULLUP** on tamper and button pins (no external pull-downs).
- Check **sensor thresholds** if alerts are too sensitive or unresponsive.
- Use **Serial Monitor** to debug Arduino side events.


============================================================
     For questions or improvements, please contribute!
============================================================
