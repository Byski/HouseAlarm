// ------------------------------------------
// HOUSE ALARM SYSTEM BY EBOKOSIA EBUBE JOY|
// ------------------------------------------

// ---------------------------
// Include Libraries
// ---------------------------
#include <LiquidCrystal.h> // Library for controlling LCD
#include <Keypad.h>       // Library for keypad input
#include <Wire.h>         // Library for I2C communication
#include <Adafruit_MLX90614.h> // Library for temperature sensor

// ---------------------------
// LCD Setup
// ---------------------------
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12; // Pin configuration for LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Create an LCD object

// ---------------------------
// Keypad Setup
// ---------------------------
const byte ROWS = 4; // Number of rows in the keypad
const byte COLS = 4; // Number of columns in the keypad
char keys[ROWS][COLS] = { // Key mapping for the keypad
    { '1', '2', '3', 'A' },
    { '4', '5', '6', 'B' },
    { '7', '8', '9', 'C' },
    { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 23, 25, 27, 29 }; // Row pin connections
byte colPins[COLS] = { 31, 33, 35, 37 }; // Column pin connections
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // Create a keypad object

// ---------------------------
// Temperature Sensor Setup
// ---------------------------
Adafruit_MLX90614 mlx = Adafruit_MLX90614(); // Create a temperature sensor object

// --------------------------------------------------
// Ultrasonic Sensor, Tamper Pins, and Buzzer Pins
// --------------------------------------------------
const int tamperPins[3] = { 43, 45, 47 }; // Pins for tamper detection
const int trigPin = 5; // Trigger pin for ultrasonic sensor
const int echoPin = 6; // Echo pin for ultrasonic sensor
const int entryExitBuzzer = 48; // Buzzer pin for entry/exit alert
const int panicBuzzer = 46; // Buzzer pin for panic alert
const int tamperBuzzer = 42; // Buzzer pin for tamper alert
const int nightBuzzer = 44; // Buzzer pin for night zone alert

// ---------------------------
// Panic & Night Zone Buttons
// ---------------------------
const int panicActivateBtn = 40; // Panic button activation pin
const int panicDeactivateBtn = 38; // Panic button deactivation pin
const int nightActivateBtn = 34; // Night zone activation button pin
const int nightDeactivateBtn = 36; // Night zone deactivation button pin

// ---------------------------
// Night Zone LED
// ---------------------------
const int nightLED = 39; // Pin for night zone LED

// ---------------------------
// IR Sensor Pin for Motion Detection
// ---------------------------
const int irSensorPin = 3; // Pin for IR motion sensor

// ---------------------------
// Sound Sensor Pin
// ---------------------------
const int soundSensorPin = A0; // Analog pin for sound sensor

// ---------------------------
// Global Variables
// ---------------------------
String inputCode = ""; // Stores entered code
bool alarmActive = false; // Tracks if alarm is active
bool panicActive = false; // Tracks if panic mode is active
bool nightZoneActive = false; // Tracks if night zone is active
bool soundDetected = false; // Tracks if sound is detected
bool isAlarmActive = false; // Tracks if the alarm is currently active
bool keepBuzzerActive = false; // Flag to keep buzzer active

// ---------------------------
// Timers
// ---------------------------
unsigned long lastLCDUpdate = 0; // Last time the LCD was updated
const unsigned long lcdTimeout = 15000; // Time to clear LCD message (15 seconds)
unsigned long irIntruderTime = 0; // Timer for IR intruder detection
unsigned long lastSoundPrintTime = 0; // Timer for last sound print

// ---------------------------
// State Variables for Message Display
// ---------------------------
bool messageDisplayed = false; // Tracks if a message is currently displayed
bool tamperAlerted = false; // Tracks if tamper alert has been activated

// ---------------------------
// Threshold Values
// ---------------------------
const float tempThreshold = 38.0; // Temperature threshold for alerts
const int minDistance = 10; // Minimum distance for detection
const int maxDistance = 100; // Maximum distance for detection
const int soundThreshold = 550; // Sound threshold for detection

// ---------------------------
// Password Attempt Counter
// ---------------------------
int incorrectAttempts = 0; // Counter for incorrect code attempts

// ---------------------------
// Helper Functions
// ---------------------------

// Display a message on the LCD and log to Serial
void displayMessage(String message) {
    Serial.println(message); // Print message to Serial Monitor
    lcd.clear(); // Clear the LCD
    lcd.print(message); // Display message on LCD
    lastLCDUpdate = millis(); // Update last LCD update time
    messageDisplayed = true; // Set message displayed state
}

// Reset message display state after timeout
void resetMessageState() {
    if (millis() - lastLCDUpdate >= lcdTimeout) { // Check if timeout has occurred
        messageDisplayed = false; // Reset message displayed state
    }
}

// Activate buzzer with a frequency
void activateBuzzer(int buzzerPin, int frequency) {
    tone(buzzerPin, frequency); // Emit sound at the given frequency
}

// Deactivate buzzer
void deactivateBuzzer(int buzzerPin) {
    noTone(buzzerPin); // Stop sound on the buzzer
}

// Get distance from the ultrasonic sensor
float getDistance() {
    digitalWrite(trigPin, LOW); // Ensure trigger pin is low
    delayMicroseconds(2); // Wait for 2 microseconds
    digitalWrite(trigPin, HIGH); // Trigger ultrasonic pulse
    delayMicroseconds(10); // Keep trigger high for 10 microseconds
    digitalWrite(trigPin, LOW); // Turn off trigger

    long duration = pulseIn(echoPin, HIGH); // Read the echo time
    return duration * 0.75 / 2; // Convert duration to distance (in cm)
}

// ---------------------------
// Setup Function
// ---------------------------
void setup() {
    Serial.begin(9600); // Start Serial communication at 9600 baud
    lcd.begin(16, 2); // Initialize the LCD with 16 columns and 2 rows

    // Initialize Buzzer Pins
    pinMode(entryExitBuzzer, OUTPUT); // Set entry/exit buzzer pin as output
    pinMode(panicBuzzer, OUTPUT); // Set panic buzzer pin as output
    pinMode(tamperBuzzer, OUTPUT); // Set tamper buzzer pin as output
    pinMode(nightBuzzer, OUTPUT); // Set night buzzer pin as output
    deactivateBuzzer(entryExitBuzzer); // Ensure buzzer is off
    deactivateBuzzer(panicBuzzer);
    deactivateBuzzer(tamperBuzzer);

    // Initialize Button Pins
    pinMode(panicActivateBtn, INPUT_PULLUP); // Set panic activate button as input with pull-up resistor
    pinMode(panicDeactivateBtn, INPUT_PULLUP); // Set panic deactivate button as input with pull-up resistor
    pinMode(nightActivateBtn, INPUT_PULLUP); // Set night activate button as input with pull-up resistor
    pinMode(nightDeactivateBtn, INPUT_PULLUP); // Set night deactivate button as input with pull-up resistor

    // Initialize Night Zone LED
    pinMode(nightLED, OUTPUT); // Set night LED pin as output
    digitalWrite(nightLED, LOW); // Ensure night LED is off

    // Initialize Sensor Pins
    pinMode(irSensorPin, INPUT); // Set IR sensor pin as input
    pinMode(soundSensorPin, INPUT); // Set sound sensor pin as input
    pinMode(trigPin, OUTPUT); // Set trigger pin for ultrasonic sensor as output
    pinMode(echoPin, INPUT); // Set echo pin for ultrasonic sensor as input

    // Initialize Tamper Pins
    for (int i = 0; i < 3; i++) {
        pinMode(tamperPins[i], INPUT_PULLUP); // Set each tamper pin as input with pull-up resistor
    }

    mlx.begin(); // Initialize the MLX90614 temperature sensor
    displayMessage("Ebube's System Initialized"); // Display initialization message

    // Delay to avoid immediate tamper alert
    delay(5000); // Wait for 5 seconds before checking tamper state
}

// ---------------------------
// Main Loop
// ---------------------------
void loop() {
    resetMessageState(); // Reset message state for the LCD

    // Check distance for approaching objects
    float distance = getDistance(); // Get distance from ultrasonic sensor
    if (distance > minDistance && distance < maxDistance) { // If someone is approaching
        displayMessage("Someone Approaching."); // Display message
        activateBuzzer(entryExitBuzzer, 1000); // Activate entry/exit buzzer
    }

    // Keypad input handling
    char key = keypad.getKey(); // Check for any key press
    if (key) { // If a key has been pressed
        Serial.print("Key Pressed: "); // Print key press to Serial Monitor
        Serial.println(key);

        if (key == '*') { // If '*' is pressed
            inputCode = ""; // Clear input code
            displayMessage("Code Cleared"); // Display cleared message
        } else {
            inputCode += key; // Append pressed key to input code
            displayMessage("Code: " + inputCode); // Display current code
        }

        // Handle code inputs
        if (inputCode == "1234") { // Correct entry code
            displayMessage("Entry Granted"); // Grant entry
            deactivateBuzzer(entryExitBuzzer); // Turn off entry buzzer
            incorrectAttempts = 0; // Reset incorrect attempts
            inputCode = ""; // Clear input code
        } else if (inputCode == "4321") { // Correct exit code
            displayMessage("Exit Granted"); // Grant exit
            deactivateBuzzer(entryExitBuzzer); // Turn off entry buzzer
            incorrectAttempts = 0; // Reset incorrect attempts
            inputCode = ""; // Clear input code
        } else if (inputCode == "CCCC") { // Code to stop alarm
            displayMessage("Alarm Stopped"); // Stop the alarm
            deactivateBuzzer(entryExitBuzzer); // Turn off entry buzzer
            inputCode = ""; // Clear input code
            isAlarmActive = false; // Deactivate alarm
            tamperAlerted = false; // Reset tamper alert state
        } else if (inputCode.length() >= 4) { // If 4 or more digits are entered
            incorrectAttempts++; // Increment incorrect attempt counter
            displayMessage("Wrong Code!"); // Display wrong code message

            if (incorrectAttempts >= 2) { // If 2 or more incorrect attempts
                displayMessage("Alarm Activated!"); // Activate alarm
                isAlarmActive = true; // Set alarm state to active
                incorrectAttempts = 0; // Reset incorrect attempts
            }

            inputCode = ""; // Clear input code
        }
    }

    // Activate alarm if necessary
    if (isAlarmActive) {
        activateBuzzer(entryExitBuzzer, 2000); // Activate alarm buzzer
    } else {
        deactivateBuzzer(entryExitBuzzer); // Turn off alarm buzzer
    }

    // Check for IR intruders
    if (digitalRead(irSensorPin) == LOW) { // If motion is detected
        displayMessage("IR Intruder!"); // Display intruder message
        activateBuzzer(tamperBuzzer, 1500); // Activate tamper buzzer
        irIntruderTime = millis(); // Record intruder detection time
    } else {
        deactivateBuzzer(tamperBuzzer); // Turn off tamper buzzer
        if (millis() - irIntruderTime >= 10000 && irIntruderTime > 0) { // If 10 seconds have passed since detection
            displayMessage("IR Stopped"); // Display stopped message
            irIntruderTime = 0; // Reset timer
        }
    }

    // Panic Zone Activation
    if (digitalRead(panicActivateBtn) == LOW) { // If panic button is pressed
        panicActive = true; // Set panic mode to active
        displayMessage("Panic Zone Activated"); // Display activation message
        activateBuzzer(panicBuzzer, 2000); // Activate panic buzzer
        while (digitalRead(panicActivateBtn) == LOW); // Wait until button is released
    }

    // Panic Zone Deactivation
    if (digitalRead(panicDeactivateBtn) == LOW) { // If panic deactivate button is pressed
        panicActive = false; // Set panic mode to inactive
        displayMessage("Panic Zone Deactivated"); // Display deactivation message
        deactivateBuzzer(panicBuzzer); // Turn off panic buzzer
        while (digitalRead(panicDeactivateBtn) == LOW); // Wait until button is released
    }

    // Night Zone Activation
    if (digitalRead(nightActivateBtn) == LOW) { // If night activate button is pressed
        nightZoneActive = true; // Set night zone to active
        digitalWrite(nightLED, LOW); // Turn on night LED
        displayMessage("Night Zone Activated"); // Display activation message
        Serial.println("Night Zone Activated"); // Print to Serial Monitor
        while (digitalRead(nightActivateBtn) == LOW); // Wait until button is released
    }

    // Night Zone Deactivation
    if (digitalRead(nightDeactivateBtn) == LOW) { // If night deactivate button is pressed
        nightZoneActive = false; // Set night zone to inactive
        digitalWrite(nightLED, HIGH); // Turn off night LED
        displayMessage("Night Zone Deactivated"); // Display deactivation message
        Serial.println("Night Zone Deactivated"); // Print to Serial Monitor
        deactivateBuzzer(entryExitBuzzer); // Turn off entry buzzer
        keepBuzzerActive = false; // Reset keep buzzer state
        while (digitalRead(nightDeactivateBtn) == LOW); // Wait until button is released
    }

    // Night Zone Temperature and Sound Detection
    if (nightZoneActive) { // If night zone is active
        float tempC = mlx.readObjectTempC(); // Read temperature
        displayMessage("Temp: " + String(tempC, 1) + " C"); // Display temperature
        if (tempC > tempThreshold) { // If temperature exceeds threshold
            displayMessage("Temp Intruder!"); // Display temperature alert
            activateBuzzer(tamperBuzzer, 1500); // Activate tamper buzzer
        }

        // Check for sound detection
        if (analogRead(soundSensorPin) > soundThreshold) { // If sound detected
            // Check if 10 seconds have passed since last print
            if (millis() - lastSoundPrintTime >= 10000) { // If 10 seconds have passed
                Serial.println("Sound Detected in Night Zone!"); // Print sound detection to Serial Monitor
                lastSoundPrintTime = millis(); // Update last print time
            }
            keepBuzzerActive = true; // Keep buzzer active
            activateBuzzer(entryExitBuzzer, 2000); // Activate entry buzzer
        }
    } else {
        deactivateBuzzer(entryExitBuzzer); // Turn off entry buzzer if night zone is inactive
    }

    // Keep the buzzer active if sound was detected
    if (keepBuzzerActive) {
        activateBuzzer(entryExitBuzzer, 2000); // Keep buzzing if sound detected
    }

    // Tamper Detection Logic with Debounce
 static unsigned long lastTamperTime = 0; // To store the last tamper detection time
    for (int i = 0; i < 3; i++) { // Check each tamper pin
        if (digitalRead(tamperPins[i]) == LOW) { // If tamper pin is triggered
            if (!tamperAlerted && (millis() - lastTamperTime > 10)) { // Debounce for 10 ms
                // displayMessage("Tampered Wire!"); // Optionally display tamper message
                activateBuzzer(tamperBuzzer, 1500); // Activate tamper buzzer
                tamperAlerted = true; // Set alert state to true
                lastTamperTime = millis(); // Update last tamper time
                messageDisplayed = true; // Optionally set message printed state
            }
            break; // Exit loop if any tamper switch is triggered
        }
    }

    // Reset tamper alert if tamper wires are restored
    if (tamperAlerted) {
        for (int i = 0; i < 3; i++) { // Check each tamper pin again
            if (digitalRead(tamperPins[i]) == HIGH) { // If tamper pin is restored
                tamperAlerted = false; // Reset alert state
                messageDisplayed = false; // Reset message printed state
                break; // Exit loop if any tamper switch is restored
            }
        }
    }
 
    // Clear the LCD if no message is displayed
    if (millis() - lastLCDUpdate >= lcdTimeout) { // If timeout has occurred
        lcd.clear(); // Clear the LCD
    }

    delay(200); // Loop delay to prevent rapid cycling
}
