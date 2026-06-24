// === Motor Control Pins ===
#define ENA 5
#define IN1 6
#define IN2 7
#define IN3 8
#define IN4 9
#define ENB 10
// === Flame Sensors ===
#define FLAME_LEFT A2
#define FLAME_CENTER A1
#define FLAME_RIGHT A0
#define RELAY_PIN 11
unsigned long pumpStartTime = 0;
bool pumpRunning = false;
// === Flame Detection Parameters ===
int threshold = 700; // Flame detection threshold for pump
int requiredConsecutiveFire = 3; // Readings to confirm fire
int requiredNoFire = 3; // Readings to confirm no fire
int fireCounter = 0;
int noFireCounter = 0;
bool pumpState = false;
// === Motor Control Parameters ===
int flameThreshold = 300; // Flame close
int farFlameMin = 300; // Flame just visible
int farFlameMax = 400; // Flame a bit far
void setup() {
 // Motor control pins
 pinMode(ENA, OUTPUT);
 pinMode(IN1, OUTPUT);
 pinMode(IN2, OUTPUT);
 pinMode(IN3, OUTPUT);
 pinMode(IN4, OUTPUT);
 pinMode(ENB, OUTPUT);
 // Flame sensor pins
 pinMode(FLAME_LEFT, INPUT);
 pinMode(FLAME_CENTER, INPUT);
 pinMode(FLAME_RIGHT, INPUT);
 pinMode(RELAY_PIN, OUTPUT);
 // Start with pump OFF and motors stopped
 digitalWrite(RELAY_PIN, LOW);
 stopMotors();
 Serial.begin(9600);
}
void loop() {
 // Read flame sensor values
 int leftValue = analogRead(FLAME_LEFT);
 int centerValue = analogRead(FLAME_CENTER);
 int rightValue = analogRead(FLAME_RIGHT);
 // Print sensor values for monitoring
 Serial.print("Left: "); Serial.print(leftValue);
 Serial.print(" Center: "); Serial.print(centerValue);
 Serial.print(" Right: "); Serial.print(rightValue);
 // === PUMP CONTROL LOGIC ===
 bool flameDetected = (leftValue < threshold || centerValue <
threshold || rightValue < threshold);
 if (flameDetected) {
 fireCounter++;
 noFireCounter = 0;
 if (!pumpState && fireCounter >= requiredConsecutiveFire) {
 digitalWrite(RELAY_PIN, LOW); // Turn pump ON
 pumpState = true;
 pumpRunning = true;
 pumpStartTime = millis();
 Serial.println(" - FIRE CONFIRMED. Pump ON");
 } else {
 Serial.println(" - Flame detected, confirming...");
 }
 } else {
 fireCounter = 0;
 noFireCounter++;
 if (pumpState && noFireCounter >= requiredNoFire) {
 digitalWrite(RELAY_PIN, HIGH); // Turn pump OFF
 pumpState = false;
 pumpRunning = false;
 Serial.println(" - FIRE GONE. Pump OFF");
 } else {
 Serial.println(" - No flame, checking again...");
 }
 }
 if (pumpRunning && (millis() - pumpStartTime >= 3000)) {
 digitalWrite(RELAY_PIN, HIGH); // পাম্প বন্ধ
 pumpRunning = false;
 pumpState = false;
 Serial.println(" - Pump stopped after 3 seconds");
}
 // === MOTOR CONTROL LOGIC (Updated with distance logic) ===
 bool fireDetectedForMotors = (leftValue < farFlameMax) ||
(centerValue < farFlameMax) || (rightValue < farFlameMax);
 if (fireDetectedForMotors) {
 int minValue = min(leftValue, min(centerValue, rightValue));
 if (minValue < flameThreshold) {
 if (centerValue <= leftValue && centerValue <= rightValue) {
 moveForward();
 } else if (leftValue < rightValue) {
 softTurnLeft();
 } else {
 softTurnRight();
 }
 } else if (minValue >= farFlameMin && minValue <= farFlameMax) {
 stopMotors();
 }
} else {
 stopMotors();
}
 delay(200);
}
// === Motor control functions ===
void moveForward() {
 digitalWrite(IN1, LOW);
 digitalWrite(IN2, HIGH);
 analogWrite(ENA, 180);
 digitalWrite(IN3, LOW);
 digitalWrite(IN4, HIGH);
 analogWrite(ENB, 180);
}
void stopMotors() {
 digitalWrite(IN1, LOW);
 digitalWrite(IN2, LOW);
 analogWrite(ENA, 0);
 digitalWrite(IN3, LOW);
 digitalWrite(IN4, LOW);
 analogWrite(ENB, 0);
}
void softTurnLeft() {
 digitalWrite(IN1, LOW);
 digitalWrite(IN2, HIGH);
 analogWrite(ENA, 180); // বাম মোটর পূর্ণ স্পিড
 digitalWrite(IN3, HIGH);
 digitalWrite(IN4, LOW);
 analogWrite(ENB, 80); // ডান মোটর কম স্পিড
}
void softTurnRight() {
 digitalWrite(IN1, HIGH);
 digitalWrite(IN2, LOW);
 analogWrite(ENA, 80); // write motor small speed
 digitalWrite(IN3, LOW);
 digitalWrite(IN4, HIGH);
 analogWrite(ENB, 180); // write motor full speed
}