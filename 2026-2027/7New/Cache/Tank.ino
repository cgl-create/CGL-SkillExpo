#include <Servo.h>

Servo tankServo;

const int trigPin = 7;
const int echoPin = 8;
const int servoPin = 6;

const float detectionRadius = 25.0;

int currentAngle = 0;
int sweepDirection = 2;

bool isLocked = false;
int missCounter = 0;
const int maxMisses = 10; // High debounce threshold to guarantee stability

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  tankServo.attach(servoPin);
  tankServo.write(currentAngle);

  delay(1000);
  Serial.println("System Ready. Scanning...");
}

void loop() {
  // 1. Measure distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH, 20000);
  float distance = (duration == 0) ? 999.0 : (duration * 0.0343 / 2);

  // 2. Check for object within 25cm
  if (distance > 0.0 && distance <= detectionRadius) {
    missCounter = 0;
   
    if (!isLocked) {
      isLocked = true;
      Serial.println("--- TARGET LOCK! CUTTING MOTOR POWER DEAD ---");
      tankServo.detach(); // physically cuts the PWM signal so it CANNOT crawl
    }
  }
  else {
    missCounter++;
   
    if (missCounter >= maxMisses && isLocked) {
      isLocked = false;
      Serial.println("--- TARGET GONE. RE-ATTACHING MOTOR AND RESUMING SCAN ---");
      tankServo.attach(servoPin); // Re-power the motor to move again
      tankServo.write(currentAngle); // Return to last known spot smoothly
      delay(150); // Give the motor a split second to re-engage
    }
  }

  // 3. Move loop (only runs when unlocked and attached)
  if (!isLocked) {
    currentAngle += sweepDirection;
   
    if (currentAngle >= 180) {
      currentAngle = 180;
      sweepDirection = -2;
    }
    else if (currentAngle <= 0) {
      currentAngle = 0;
      sweepDirection = 2;
    }
   
    tankServo.write(currentAngle);
    delay(33);
  }
  else {
    delay(50); // Stabilization pause while power is cut
  }
}


