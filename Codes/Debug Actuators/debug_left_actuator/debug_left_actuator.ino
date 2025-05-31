#include <Arduino.h>


// --- Actuator 1 (Left Actuator, BTS7960 Driver 1) ---
#define RPWM1 3   // PWM for retraction (to MIN)
#define LPWM1 4   // PWM for extension (to MAX)
#define R_EN1 7   // Enable pin for actuator 1
#define L_EN1 8   // Enable pin for actuator 1
/**
// --- Actuator 2 (Right Actuator, BTS7960 Driver 2) ---
#define RPWM2 5   // PWM for retraction (to MIN)
#define LPWM2 6   // PWM for extension (to MAX)
#define R_EN2 9   // Enable pin for actuator 2
#define L_EN2 10  // Enable pin for actuator 2**/

const int fullSpeed = 255;  // PWM value for full speed
const int MIN_POS = 0;      // Fully retracted (MIN) position
const int MAX_POS = 5000;   // Fully extended (MAX) position

// Structure to hold actuator state
struct ActuatorState {
  int currentPos;           // Current position (position units)
  int targetPos;            // Desired target position
  bool moving;              // True if currently moving
  unsigned long lastUpdate; // Last update time (millis)
  int pinRetract;           // PWM pin used for retraction
  int pinExtend;            // PWM pin used for extension
};

ActuatorState actuator2;

// Function to start movement by setting target and marking as moving
void startMovement(ActuatorState &act, int target) {
  act.targetPos = target;
  act.moving = true;
  act.lastUpdate = millis();
  Serial.print("Starting movement to ");
  Serial.println(target);
}

// Non-blocking update function to simulate actuator movement
void updateActuator(ActuatorState &act) {
  if (!act.moving) return;
  
  unsigned long now = millis();
  unsigned long elapsed = now - act.lastUpdate;
  act.lastUpdate = now;
  
  // If target is less than current, retract (move toward MIN)
  if (act.targetPos < act.currentPos) {
    int step = elapsed;  // movement rate: 1 unit per ms (adjust if needed)
    act.currentPos -= step;
    if (act.currentPos <= act.targetPos) {
      act.currentPos = act.targetPos;
      act.moving = false;
      analogWrite(act.pinRetract, 0);
      analogWrite(act.pinExtend, 0);
      Serial.print("Reached MIN position: ");
      Serial.println(act.currentPos);
    } else {
      analogWrite(act.pinRetract, fullSpeed);
      analogWrite(act.pinExtend, 0);
    }
  }
  // If target is greater than current, extend (move toward MAX)
  else if (act.targetPos > act.currentPos) {
    int step = elapsed;
    act.currentPos += step;
    if (act.currentPos >= act.targetPos) {
      act.currentPos = act.targetPos;
      act.moving = false;
      analogWrite(act.pinRetract, 0);
      analogWrite(act.pinExtend, 0);
      Serial.print("Reached target position: ");
      Serial.println(act.currentPos);
    } else {
      analogWrite(act.pinRetract, 0);
      analogWrite(act.pinExtend, fullSpeed);
    }
  }
}

void setup() {
  Serial.begin(9600);
  
  // Initialize Actuator 2 pins
  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM1, OUTPUT);
  pinMode(R_EN1, OUTPUT);
  pinMode(L_EN1, OUTPUT);
  
  // Enable the driver
  digitalWrite(R_EN1, HIGH);
  digitalWrite(L_EN1, HIGH);
  
  // Assume actuator starts fully extended, then fully retract.
  actuator2.currentPos = MAX_POS;
  actuator2.targetPos = MAX_POS;
  actuator2.moving = false;
  actuator2.lastUpdate = millis();
  actuator2.pinRetract = RPWM1;
  actuator2.pinExtend = LPWM1;
  
  Serial.println("Actuator2 Debugging Initialized.");
  Serial.println("Fully retracting Actuator 2 (moving to MIN = 0)...");
  startMovement(actuator2, MIN_POS);
  
  Serial.println("After retraction, you can change the position using the command:");
  Serial.println("SET <value>   (0 to 5000)");
}

void loop() {
  // Listen for Serial commands once retraction is complete.
  if (!actuator2.moving && Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.startsWith("SET ")) {
      String valueStr = input.substring(4);
      int target = valueStr.toInt();
      // Constrain the target value to valid limits.
      if (target < MIN_POS) target = MIN_POS;
      if (target > MAX_POS) target = MAX_POS;
      startMovement(actuator2, target);
    } else {
      Serial.println("Invalid command. Please use: SET <value>");
    }
  }
  
  // Continuously update actuator movement.
  updateActuator(actuator2);
  
  // Small delay for loop stability.
  delay(10);
}
