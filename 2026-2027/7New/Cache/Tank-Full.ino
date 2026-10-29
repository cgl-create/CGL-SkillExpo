#include <Servo.h>

// CGL+ Skills Expo — Tank-Full
// Scanner demo: ultrasonic obstacle detection + servo head.
// The original Tank.ino is left unchanged.
// This file does not control a projectile launcher.

Servo tankServo;

const int trigPin = 7;
const int echoPin = 8;
const int servoPin = 6;

const float detectionRadius = 25.0;
const unsigned long maxPauseMs = 25000UL;
const int maxMisses = 10;

int currentAngle = 0;
int sweepDirection = 2;
int missCounter = 0;

bool isPaused = false;
bool ignoreObstacleUntilClear = false;
unsigned long pauseStartedAt = 0;

void resumeScanning(const char* reason, bool timedOut) {
  isPaused = false;
  // If the timer expires while an obstacle is still present, keep scanning
  // until that obstacle leaves range so we do not immediately pause again.
  ignoreObstacleUntilClear = timedOut;
  missCounter = 0;

  Serial.println(reason);
  tankServo.attach(servoPin);
  tankServo.write(currentAngle);
  delay(150);
}

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

  unsigned long duration = pulseIn(echoPin, HIGH, 20000UL);
  float distance = (duration == 0) ? 999.0 : (duration * 0.0343 / 2.0);
  bool obstaclePresent = (distance > 0.0 && distance <= detectionRadius);

  // After timeout, keep scanning rather than immediately pausing again
  // on the same obstacle. Re-arm detection once it leaves the range.
  if (ignoreObstacleUntilClear && !obstaclePresent) {
    ignoreObstacleUntilClear = false;
    Serial.println("Obstacle cleared. Detection re-armed.");
  }

  if (obstaclePresent && !ignoreObstacleUntilClear) {
    missCounter = 0;

    if (!isPaused) {
      isPaused = true;
      pauseStartedAt = millis();
      Serial.print("Obstacle detected at ");
      Serial.print(distance);
      Serial.println(" cm. Pausing scan for up to 25 seconds.");
      tankServo.detach();
    }
  } else if (isPaused) {
    if (missCounter < maxMisses) {
      missCounter++;
    }

    // Resume when 10 consecutive readings report no obstacle.
    if (missCounter >= maxMisses) {
      resumeScanning("Obstacle disappeared. Resuming scan.", false);
    }
  }

  // Resume after 25 seconds even if the obstacle remains present.
  if (isPaused && (unsigned long)(millis() - pauseStartedAt) >= maxPauseMs) {
    resumeScanning("25-second pause elapsed. Resuming scan.", true);
  }

  // Sweep only when the head is not paused.
  if (!isPaused) {
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
