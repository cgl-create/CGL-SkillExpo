// Project tank. HW-130 shield on an Arduino Uno.
// The servo holds still on a 1500 us stop pulse.
// On a ping below 30 cm it turns one short step at a time,
// then the stop pulse is sent again so it cannot keep spinning.
// Servo plugs into SER1. Ultrasonic trig A0, echo A1, VCC 5V, GND GND.
// After it lines up: wait 2 seconds, run M3 and M4 for 3 seconds,
// turn back to the start heading, wait 15 seconds.

#include <AFMotor.h>
#include <NewPing.h>
#include <Servo.h>

const int servoPin = 10;
const int trigPin = A0;
const int echoPin = A1;
const int pulseStop = 1500;
const int pulseSlow = 120;
const int seeCm = 30;
const int closerCm = 2;
const int maxSteps = 20;
const int motorSpeed = 255;
const unsigned long motorRunMs = 3000;
const unsigned long reloadMs = 15000;
const unsigned long seenWaitMs = 2000;
const unsigned long nudgeMs = 160;
const unsigned long settleMs = 250;

int netSteps = 0;

AF_DCMotor motorA(3);
AF_DCMotor motorB(4);
NewPing sonar(trigPin, echoPin, seeCm);
Servo turret;

void stopShootingMotors() {
  motorA.run(RELEASE);
  motorB.run(RELEASE);
}

void holdServo() {
  turret.writeMicroseconds(pulseStop);
}

void nudge(int direction) {
  if (direction > 0) {
    turret.writeMicroseconds(pulseStop + pulseSlow);
  } else if (direction < 0) {
    turret.writeMicroseconds(pulseStop - pulseSlow);
  } else {
    holdServo();
    return;
  }
  delay(nudgeMs);
  holdServo();
  netSteps += direction;
  delay(settleMs);
}

void shootMotors() {
  Serial.println("On target. Both motors.");
  holdServo();
  motorA.setSpeed(motorSpeed);
  motorB.setSpeed(motorSpeed);
  motorA.run(FORWARD);
  motorB.run(FORWARD);
  delay(motorRunMs);
  stopShootingMotors();
  holdServo();
}

int readCm() {
  delay(30);
  unsigned int distanceCm = sonar.ping_cm();
  if (distanceCm == 0 || distanceCm >= seeCm) {
    return 999;
  }
  return distanceCm;
}

void waitForPing() {
  while (true) {
    holdServo();
    int distanceCm = readCm();
    Serial.print("STILL ");
    Serial.print(distanceCm);
    Serial.println(" cm");
    if (distanceCm < seeCm) {
      Serial.println("Ping.");
      return;
    }
  }
}

void alignSlowly() {
  holdServo();
  int bestDist = readCm();
  Serial.print(bestDist);
  Serial.println(" cm");

  nudge(1);
  int rightDist = readCm();
  Serial.print(rightDist);
  Serial.println(" cm");

  nudge(-1);
  nudge(-1);
  int leftDist = readCm();
  Serial.print(leftDist);
  Serial.println(" cm");

  nudge(1);

  int step = 0;
  if (rightDist + closerCm < bestDist && rightDist <= leftDist) {
    step = 1;
    bestDist = rightDist;
    Serial.println("Turn right.");
  } else if (leftDist + closerCm < bestDist) {
    step = -1;
    bestDist = leftDist;
    Serial.println("Turn left.");
  } else {
    holdServo();
    Serial.println("Aligned.");
    return;
  }

  nudge(step);
  for (int n = 1; n < maxSteps; n++) {
    nudge(step);
    int distanceCm = readCm();
    Serial.print(distanceCm);
    Serial.println(" cm");
    if (distanceCm > bestDist - closerCm) {
      nudge(-step);
      break;
    }
    bestDist = distanceCm;
  }

  holdServo();
  Serial.println("Aligned.");
}

void returnStraight() {
  int back = 0;
  if (netSteps > 0) {
    back = -1;
  } else if (netSteps < 0) {
    back = 1;
  }
  while (netSteps != 0) {
    nudge(back);
  }
  holdServo();
  Serial.println("Straight.");
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  stopShootingMotors();
  turret.writeMicroseconds(pulseStop);
  turret.attach(servoPin, 500, 2500);
  holdServo();
}

void loop() {
  waitForPing();
  alignSlowly();

  Serial.println("Wait 2 seconds.");
  holdServo();
  delay(seenWaitMs);

  shootMotors();
  returnStraight();

  Serial.println("Wait 15 seconds.");
  holdServo();
  delay(reloadMs);
}

