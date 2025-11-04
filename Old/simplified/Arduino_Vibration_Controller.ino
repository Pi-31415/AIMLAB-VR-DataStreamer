/**
 * @file Arduino_Vibration_Controller.ino
 * @brief Vibration Motor Controller for AIMLAB VR Data Streamer
 * @author Pi Ko (pi.ko@nyu.edu)
 * @date 04 November 2025
 * @version v1.0
 * 
 * Arduino sketch for controlling a vibration motor via serial commands.
 * Compatible with AIMLAB VR Data Streamer Console application.
 * 
 * Hardware Setup:
 * - Vibration motor connected to digital pin 9 (PWM)
 * - Motor connected through transistor/MOSFET for proper current handling
 * - Optional LED on pin 13 for status indication
 * 
 * Serial Protocol:
 * - Baud Rate: 9600
 * - Handshake: Receives "HELLO" → Sends "Vibration Motor Controller Ready"
 * - Command '1': Activate vibration (500ms pulse)
 * - Command '0': Stop vibration
 * 
 * Changelog:
 * v1.0 - 04 November 2025 - Initial implementation
 *   - Serial handshake protocol
 *   - Single vibration pulse command
 *   - Status LED indication
 */

// Pin definitions
const int MOTOR_PIN = 9;       // PWM pin for vibration motor
const int STATUS_LED = 13;     // Built-in LED for status

// Motor control parameters
const int VIBRATION_DURATION = 500;  // Duration in milliseconds
const int MOTOR_INTENSITY = 255;     // PWM value (0-255)

// State variables
bool motorActive = false;
unsigned long motorStartTime = 0;

/**
 * @brief Initialize hardware and serial communication
 */
void setup() {
  // Initialize pins
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  
  // Ensure motor is off at startup
  digitalWrite(MOTOR_PIN, LOW);
  digitalWrite(STATUS_LED, LOW);
  
  // Initialize serial communication
  Serial.begin(9600);
  
  // Wait for serial to be ready
  while (!Serial) {
    ; // Wait for serial port to connect (needed for native USB)
  }
  
  // Blink LED to indicate ready
  for (int i = 0; i < 3; i++) {
    digitalWrite(STATUS_LED, HIGH);
    delay(100);
    digitalWrite(STATUS_LED, LOW);
    delay(100);
  }
  
  // Send ready message
  Serial.println("Vibration Motor Controller Ready");
  Serial.flush();
}

/**
 * @brief Main loop - process serial commands and manage motor state
 */
void loop() {
  // Check for incoming serial data
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();  // Remove whitespace
    
    // Handle handshake
    if (command == "HELLO") {
      Serial.println("Vibration Motor Controller Ready");
      Serial.flush();
      
      // Blink LED to acknowledge
      digitalWrite(STATUS_LED, HIGH);
      delay(200);
      digitalWrite(STATUS_LED, LOW);
    }
    // Handle vibration command
    else if (command == "1") {
      activateMotor();
      Serial.println("Vibration activated");
      Serial.flush();
    }
    // Handle stop command
    else if (command == "0") {
      deactivateMotor();
      Serial.println("Vibration stopped");
      Serial.flush();
    }
    // Unknown command
    else if (command.length() > 0) {
      Serial.print("Unknown command: ");
      Serial.println(command);
      Serial.flush();
    }
  }
  
  // Check if motor should be turned off (after duration)
  if (motorActive && (millis() - motorStartTime >= VIBRATION_DURATION)) {
    deactivateMotor();
  }
  
  // Small delay to prevent overwhelming the serial buffer
  delay(10);
}

/**
 * @brief Activate the vibration motor
 */
void activateMotor() {
  analogWrite(MOTOR_PIN, MOTOR_INTENSITY);
  digitalWrite(STATUS_LED, HIGH);
  motorActive = true;
  motorStartTime = millis();
}

/**
 * @brief Deactivate the vibration motor
 */
void deactivateMotor() {
  analogWrite(MOTOR_PIN, 0);
  digitalWrite(STATUS_LED, LOW);
  motorActive = false;
}

