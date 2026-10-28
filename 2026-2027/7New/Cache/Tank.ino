

#include <Servo.h>

Servo tankServo;

const int trigPin = 7;
const int echoPin = 8;
const int servoPin = 6;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  tankServo.attach(servoPin);
  tankServo.write(0);

  delay(1000);
}

void loop() {
  // Measure distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration =
      pulseIn(echoPin, HIGH, 50000);

  if (duration == 0) {
    Serial.println("No echo detected");
    delay(200);
    return;
  }

  float distance = duration * 0.0343 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Move servo when hand is between 15 and 30 cm
  if (distance >= 15 && distance <= 30) {
    tankServo.write(90);
  } else {
    tankServo.write(0);
  }

  delay(200);
}
