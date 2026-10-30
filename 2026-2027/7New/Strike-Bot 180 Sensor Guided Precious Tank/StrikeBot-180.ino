#include <NewPing.h>
#include <Servo.h>

const int servoPin = 10;
const int trigPin = A0;
const int echoPin = A1;
const int pulseStop = 1500;
const int pulseSlow = 120;
const int seeCm = 30;
const unsigned long targetWaitMs = 25000; // 25 seconds wait after locking onto target
const unsigned long nudgeMs = 160;
const unsigned long settleMs = 250;

NewPing sonar(trigPin, echoPin, seeCm);
Servo turret;

void holdServo() {
turret.writeMicroseconds(pulseStop);
}

// Moves the continuous rotation servo briefly in a direction
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
delay(settleMs);
}

int readCm() {
delay(30);
unsigned int distanceCm = sonar.ping_cm();
if (distanceCm == 0 || distanceCm >= seeCm) {
return 999;
}
return distanceCm;
}

void setup() {
Serial.begin(115200);
turret.attach(servoPin);
holdServo();
Serial.println("System Initialized.");
}

void loop() {
static int currentDirection = 1; // 1 for right, -1 for left
static int sweepSteps = 0;
const int maxSweepRange = 15; // Controls how far it sweeps left/right before turning back

// Step 1: Check if something is in front of the sensor right now
int distanceCm = readCm();
Serial.print("Scanning: ");
Serial.print(distanceCm);
Serial.println(" cm");

// Step 2: Target Detected!
if (distanceCm < seeCm) {
Serial.println("Target Detected! Stopping...");
holdServo(); // Freeze immediately
Serial.println("Holding position for 25 seconds...");
delay(targetWaitMs); // Wait for 25 seconds
Serial.println("Resuming sweep...");
// The loop will automatically continue the sweep on the next cycle
return;
}

// Step 3: If no target, take one step in the current sweep direction
nudge(currentDirection);
sweepSteps++;

// Alternate the sweep direction so it goes back and forth
if (sweepSteps >= maxSweepRange) {
currentDirection = -currentDirection; // Reverse direction (1 to -1, or -1 to 1)
sweepSteps = 0; // Reset step counter for the new direction
}
}


