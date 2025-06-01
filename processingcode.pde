// ---------------------------
// HOUSE ALARM SYSTEM BY EBOKOSIA EBUBE JOY
// ---------------------------

import processing.serial.*; // Import the serial library for communication

class Room {
    int x, y, w, h; // Room position and dimensions
    String name; // Room name
    
    boolean alarm = false; // Alarm state
    boolean hovered = false; // Hover state for mouse interaction
    boolean isZone; // Indicates if the room is a zone
    boolean flash = false; // Flash state for zones
    boolean flashInfinite = false; // Indicates if the room should flash infinitely
    
    int flashCount = 0; // Number of flashes
    int flashDuration = 10; // Total flashes (5 times, 2 states each)
    int flashInterval = 500; // Flash interval in milliseconds
    int lastFlashTime = 0; // Last time the flash state was updated

    // Constructor for Room without infinite flashing
    Room(int x, int y, int w, int h, String name) {
        this.x = x;
        this.y = y;
        this.w = w;
        this.h = h;
        this.name = name;
        this.isZone = name.contains("Zone"); // Determine if it's a zone based on name
    }

    // Constructor for Room with infinite flashing option
    Room(int x, int y, int w, int h, String name, boolean flashInfinite) {
        this.x = x;
        this.y = y;
        this.w = w;
        this.h = h;
        this.name = name;
        this.flashInfinite = flashInfinite; // Set infinite flashing option
        this.isZone = name.contains("Zone"); // Determine if it's a zone based on name
    }
    
    // Display the room on the screen
    void display() {
        stroke(0);
        fill(220); // Default color for rooms
        // Change color based on hover state
        if (hovered) {
            fill(isZone ? color(0, 0, 255, 150) : color(0, 255, 0, 150)); // Blue for zones, Green for rooms
        } else if (flash) {
            // Flashing colors for specific zones
            if (name.equals("Entry Zone")) {
                fill((millis() / flashInterval) % 2 == 0 ? color(135, 206, 235) : color(255)); // Flashing sky blue
            } else if (name.equals("Exit Zone")) {
                fill((millis() / flashInterval) % 2 == 0 ? color(255, 223, 0) : color(255)); // Flashing yellow
            } else if (name.equals("Panic Zone") || name.equals("Night Zone") || name.equals("Front Door") || name.equals("Kitchen & Dining Hall") || name.equals("Washroom")) {
                fill((millis() / flashInterval) % 2 == 0 ? color(255, 0, 0) : color(255)); // Flashing red for these zones
            } else if (alarm) {
                fill(color(255, 0, 0, 150)); // Red for alarms
            }
        } else if (alarm) {
            fill(color(255, 0, 0, 150)); // Red for alarms
        }

        rect(x, y, w, h); // Draw the room rectangle
        fill(0);
        textSize(12);
        textAlign(CENTER, CENTER);
        text(name, x + w / 2, y + h / 2); // Display the room name
    }

    // Update the flashing state of the room
    void updateFlashing() {
        if (flash) {
            if (!flashInfinite) { // Check if flashing is not infinite
                if (millis() - lastFlashTime > flashInterval) {
                    flashCount++;
                    lastFlashTime = millis(); // Update the last flash time
                }
                if (flashCount >= flashDuration) { // Stop flashing after specified flashes
                    flash = false;
                    flashCount = 0; // Reset count
                }
            }
        }
    }

    // Check if the room is clicked based on mouse coordinates
    boolean isClicked(int mx, int my) {
        return mx > x && mx < x + w && my > y && my < y + h;
    }

    // Check if mouse is hovering over the room
    void checkHover(int mx, int my) {
        hovered = isClicked(mx, my);
    }
}

Room[] rooms; // Array of room objects
Serial myPort; // Serial port object for communication
String inputString = ""; // For incoming serial data

void setup() {
    size(800, 600); // Set the size of the window
    String portName = "/dev/cu.usbmodem1101"; // Define the port name
    myPort = new Serial(this, portName, 9600); // Initialize the serial port
    myPort.clear();

    // Initialize rooms with specified positions, dimensions, and names
    rooms = new Room[]{
        new Room(50, 100, 150, 100, "Bedroom 2"),
        new Room(200, 100, 150, 100, "Bedroom 3", true),
        new Room(50, 200, 150, 100, "Washroom", true),
        new Room(50, 300, 150, 100, "Toilet"),
        new Room(50, 400, 200, 150, "Bedroom 1"),
        new Room(300, 450, 150, 100, "Front Door", true),
        new Room(350, 100, 250, 150, "Kitchen & Dining Hall", true),
        new Room(350, 250, 250, 150, "Lounge"),
        new Room(600, 300, 150, 100, "Guest Toilet"),
        new Room(600, 400, 150, 150, "Garage"),
        
        // Initialize zones
        new Room(650, 50, 60, 35, "Entry Zone"),
        new Room(720, 50, 60, 35, "Exit Zone"),
        new Room(650, 100, 60, 35, "Panic Zone"),
        new Room(720, 100, 60, 35, "Night Zone"),
    };
}

void draw() {
    background(255); // Clear the background

    fill(0);
    textSize(40);
    textAlign(CENTER, CENTER);
    text("EBUBE'S HOUSE ALARM", width / 2, 40); // Display the title

    for (Room r : rooms) {
        r.display(); // Display each room
        r.updateFlashing(); // Update the flashing state
    }

    // Check for incoming serial data
    while (myPort.available() > 0) {
        inputString = myPort.readStringUntil('\n'); // Read until a newline
        if (inputString != null) {
            inputString = inputString.trim(); // Remove whitespace
            // Handle specific commands for controlling room states
            if (inputString.equals("Entry Granted")) {
                for (Room r : rooms) {
                    if (r.name.equals("Entry Zone")) {
                        r.flash = true; // Set flash to true for Entry Zone
                        r.flashCount = 0; // Reset flash count for new flashing
                    }
                }
            } else if (inputString.equals("Exit Granted")) {
                for (Room r : rooms) {
                    if (r.name.equals("Exit Zone")) {
                        r.flash = true; // Set flash to true for Exit Zone
                        r.flashCount = 0; // Reset flash count for new flashing
                    }
                }
            } else if (inputString.equals("Panic Zone Activated")) {
                for (Room r : rooms) {
                    if (r.name.equals("Panic Zone")) {
                        r.flash = true; // Set flash to true for Panic Zone
                        r.flashCount = 0; // Reset flash count for new flashing
                    }
                }
            } else if (inputString.equals("Night Zone Activated")) {
                for (Room r : rooms) {
                    if (r.name.equals("Night Zone")) {
                        r.flash = true; // Set flash to true for Night Zone
                        r.flashCount = 0; // Reset flash count for new flashing
                    }
                }
            } else if (inputString.equals("Alarm Activated!")) {
                for (Room r : rooms) {
                    if (r.name.equals("Front Door")) {
                        r.flash = true; // Set flash to true for Front Door
                    }
                }
            } else if (inputString.equals("Alarm Stopped")) {
                for (Room r : rooms) {
                    if (r.name.equals("Front Door")) {
                        r.flash = false; // Stop flashing for Front Door
                    }
                }
            } else if (inputString.equals("Sound Detected in Night Zone!")) {
                for (Room r : rooms) {
                    if (r.name.equals("Kitchen & Dining Hall")) {
                        r.flash = true; // Set flash to true for Kitchen & Dining Hall
                    }
                }
            } else if (inputString.equals("Night Zone Deactivated")) {
                for (Room r : rooms) {
                    if (r.name.equals("Kitchen & Dining Hall")) {
                        r.flash = false; // Stop flashing for Kitchen & Dining Hall
                    }
                }
            } else if (inputString.equals("IR Intruder!")) {
                for (Room r : rooms) {
                    if (r.name.equals("Washroom")) {
                        r.flash = true; // Set flash to true for Washroom
                    }
                }
            } else if (inputString.equals("IR Stopped")) {
                for (Room r : rooms) {
                    if (r.name.equals("Washroom")) {
                        r.flash = false; // Stop flashing for Washroom
                    }
                }
            }
        }
    }
}

// Handle mouse press events to toggle the alarm state of rooms
void mousePressed() {
    for (Room r : rooms) {
        if (r.isClicked(mouseX, mouseY)) {
            r.alarm = !r.alarm; // Toggle the alarm state
        }
    }
}

// Handle mouse movement events to check hover state
void mouseMoved() {
    for (Room r : rooms) {
        r.checkHover(mouseX, mouseY); // Update hover state
    }
}
