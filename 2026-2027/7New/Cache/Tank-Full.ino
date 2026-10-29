#include <Servo.h>

// CGL+ Skills Expo — Tank-Full
// Safe scanner build: ultrasonic obstacle detection + servo head.
// The existing Tank.ino is left unchanged.
// No projectile-launcher control is implemented in this file.

Servo tankServo;

const int trigPin = 7;
const int echoPin = 8;
const int servoPin = 6;

const float detectionRadius = 25.0;

int currentAngle = 0;
int sweepDirection = 2;

bool isLocked = false;
int missCounter = 0;
const int maxMisses = 10;

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
  // Measure distance using the ultrasonic sensor.
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH, 20000);
  float distance = (duration == 0) ? 999.0 : (duration * 0.0343 / 2.0);

  // Pause the scanning head when an obstacle is within 25 cm.
  if (distance > 0.0 && distance <= detectionRadius) {
    missCounter = 0;

    if (!isLocked) {
      isLocked = true;
      Serial.print("Obstacle detected at ");
      Serial.print(distance);
      Serial.println(" cm. Pausing scan.");
      tankServo.detach();
    }
  } else {
    if (missCounter < maxMisses) {
      missCounter++;
    }

    if (missCounter >= maxMisses && isLocked) {
      isLocked = false;
      Serial.println("Obstacle no longer detected. Resuming scan.");
      tankServo.attach(servoPin);
      tankServo.write(currentAngle);
      delay(150);
    }
  }

  // Sweep only while no obstacle is detected.
  if (!isLocked) {
    currentAngle += sweepDirection;

    if (currentAngle >= 180) {
      currentAngle = 180;
      sweepDirection = -2;
    } else if (currentAngle <= 0) {
      currentAngle = 0;
      sweepDirection = 2;
    }

    tankServo.write(currentAngle);
    delay(33);
  } else {
    delay(50);
  }
}
