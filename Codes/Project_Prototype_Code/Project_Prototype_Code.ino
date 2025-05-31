#include <Arduino.h>

// --- Actuator 1 (Left Actuator, BTS7960 Driver 1) ---
#define RPWM1 3   // PWM for retraction (to MIN)
#define LPWM1 4   // PWM for extension (to MAX)
#define R_EN1 7   // Enable pin for actuator 1
#define L_EN1 8   // Enable pin for actuator 1

// --- Actuator 2 (Right Actuator, BTS7960 Driver 2) ---
#define RPWM2 5   // PWM for retraction (to MIN)
#define LPWM2 6   // PWM for extension (to MAX)
#define R_EN2 9   // Enable pin for actuator 2
#define L_EN2 10  // Enable pin for actuator 2

const int fullSpeed = 255;  // PWM value for full speed
const int MIN_POS = 1460;      // Fully retracted position
const int MAX_POS = 2060;   // Updated fully extended position
const int MID_POS = 1760;   // Neutral (midstroke) position

// Structure to hold actuator state
struct ActuatorState {
  int currentPos;           // Current position (position units)
  int targetPos;            // Desired target position
  bool moving;              // True if currently moving
  unsigned long lastUpdate; // Last update time (millis)
  int pinRetract;           // Pin for retraction (toward MIN)
  int pinExtend;            // Pin for extension (toward MAX)
};

ActuatorState actuator1;  // Left actuator
ActuatorState actuator2;  // Right actuator

// Start a movement by setting a new target and recording the time
void startMovement(ActuatorState &act, int target) {
  act.targetPos = target;
  act.moving = true;
  act.lastUpdate = millis();
}

// Update an actuator's position based on elapsed time (non-blocking)
void updateActuator(ActuatorState &act) {
  if (!act.moving) return;
  
  unsigned long now = millis();
  unsigned long elapsed = now - act.lastUpdate;
  act.lastUpdate = now;
  
  // Extend (move toward MAX) if target is greater than current position
  if (act.targetPos > act.currentPos) {
    int step = elapsed;  // movement rate: 1 unit per ms
    act.currentPos += step;
    if (act.currentPos >= act.targetPos) {
      act.currentPos = act.targetPos;
      act.moving = false;
      analogWrite(act.pinRetract, 0);
      analogWrite(act.pinExtend, 0);
      Serial.print("Actuator reached target: ");
      Serial.println(act.currentPos);
    } else {
      // Continue extending
      analogWrite(act.pinRetract, 0);
      analogWrite(act.pinExtend, fullSpeed);
    }
  }
  // Retract (move toward MIN) if target is less than current position
  else if (act.targetPos < act.currentPos) {
    int step = elapsed;
    act.currentPos -= step;
    if (act.currentPos <= act.targetPos) {
      act.currentPos = act.targetPos;
      act.moving = false;
      analogWrite(act.pinRetract, 0);
      analogWrite(act.pinExtend, 0);
      Serial.print("Actuator reached target: ");
      Serial.println(act.currentPos);
    } else {
      // Continue retracting
      analogWrite(act.pinRetract, fullSpeed);
      analogWrite(act.pinExtend, 0);
    }
  }
}

// Process incoming commands (LEFT, RIGHT, NEUTRAL)
// An override (e.g. NEUTRAL) will immediately set new targets.
void processCommand(String cmd) {
  cmd.trim();
  Serial.print("Received command: ");
  Serial.println(cmd);
  
  if (cmd.equalsIgnoreCase("LEFT")) {
    // LEFT: Left actuator to MIN and Right actuator to MAX
    startMovement(actuator1, MIN_POS);
    startMovement(actuator2, MAX_POS);
  }
  else if (cmd.equalsIgnoreCase("RIGHT")) {
    // RIGHT: Left actuator to MAX and Right actuator to MIN
    startMovement(actuator1, MAX_POS);
    startMovement(actuator2, MIN_POS);
  }
  else if (cmd.equalsIgnoreCase("NEUTRAL")) {
    // NEUTRAL: Immediately override any movement; both actuators to MID
    startMovement(actuator1, MID_POS);
    startMovement(actuator2, MID_POS);
  }
  else {
    Serial.print("Unknown command: ");
    Serial.println(cmd);
  }
}

void setup() {
  Serial.begin(9600);
  
  // Set pin modes for Actuator 1 pins
  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM1, OUTPUT);
  pinMode(R_EN1, OUTPUT);
  pinMode(L_EN1, OUTPUT);
  
  // Set pin modes for Actuator 2 pins
  pinMode(RPWM2, OUTPUT);
  pinMode(LPWM2, OUTPUT);
  pinMode(R_EN2, OUTPUT);
  pinMode(L_EN2, OUTPUT);
  
  // Enable both drivers
  digitalWrite(R_EN1, HIGH);
  digitalWrite(L_EN1, HIGH);
  digitalWrite(R_EN2, HIGH);
  digitalWrite(L_EN2, HIGH);
  
  // Initialize actuator states to the neutral position
  actuator1.currentPos = MID_POS;
  actuator1.targetPos = MID_POS;
  actuator1.moving = false;
  actuator1.lastUpdate = millis();
  actuator1.pinRetract = RPWM1;
  actuator1.pinExtend = LPWM1;
  
  actuator2.currentPos = MID_POS;
  actuator2.targetPos = MID_POS;
  actuator2.moving = false;
  actuator2.lastUpdate = millis();
  actuator2.pinRetract = RPWM2;
  actuator2.pinExtend = LPWM2;
  
  // At startup, send NEUTRAL to ensure both actuators are at MID (2000)
  Serial.println("Startup: Setting actuators to NEUTRAL (MID) position...");
  processCommand("NEUTRAL");
}

void loop() {
  // Check for new serial commands and process them
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    processCommand(cmd);
  }
  
  // Update both actuators simultaneously
  updateActuator(actuator1);
  updateActuator(actuator2);
  
  // Small delay to help with loop stability
  delay(10);
}
