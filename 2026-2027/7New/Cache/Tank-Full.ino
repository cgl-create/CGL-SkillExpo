#include <Servo.h>

// CGL+ Skills Expo — Tank-Full
// Scanner demo: ultrasonic obstacle detection + servo head,
// with a stock-power Nerf dart flywheel launcher.
//
// Firing only happens when ALL of these are true:
//   - the arm switch is closed (pin 4 to GND)
//   - an obstacle has been confirmed and the warning countdown has finished
//   - the flywheels have had time to spin up
//   - the obstacle is still there at the moment of firing
// One dart per detection. The warning LED blinks during the countdown.

Servo tankServo;
Servo triggerServo;   // dart pusher

// ---------- Pins ----------
const int trigPin     = 7;
const int echoPin     = 8;
const int servoPin    = 6;

const int triggerPin  = 5;    // pusher servo signal (own 5 V supply, shared GND)
const int flywheelPin = 3;    // PWM -> 100 ohm -> logic-level MOSFET gate (10k to GND)
const int armSwitchPin = 4;   // switch to GND = armed
const int warnLedPin  = 13;

// ---------- Scanner settings ----------
const float detectionRadius = 25.0;
const unsigned long maxPauseMs = 25000UL;
const int maxMisses = 10;     // consecutive "no obstacle" readings to resume / re-arm
const int minHits   = 3;      // consecutive "obstacle" readings to confirm a detection

// ---------- Launcher settings (tune these, start low) ----------
const int triggerRest = 0;                 // pusher servo angle: retracted
const int triggerFire = 60;                // pusher servo angle: pushes one dart
const int flywheelSpeed = 120;             // 0-255 PWM; raise in steps of 20 only until the dart flies reliably
const unsigned long spinUpMs    = 1000UL;  // time for wheels to reach speed
const unsigned long fireDelayMs = 3000UL;  // warning countdown after detection
const unsigned long cooldownMs  = 5000UL;  // minimum time between shots

// ---------- State ----------
int currentAngle = 0;
int sweepDirection = 2;
int missCounter = 0;
int hitCounter = 0;
int clearCounter = 0;

bool isPaused = false;
bool ignoreObstacleUntilClear = false;
unsigned long pauseStartedAt = 0;

bool shotFiredThisEvent = false;
bool hasFiredBefore = false;
unsigned long lastShotAt = 0;

bool flywheelOn = false;
unsigned long flywheelStartedAt = 0;

// ---------- Launcher helpers ----------
void flywheelStart() {
  if (!flywheelOn) {
    analogWrite(flywheelPin, flywheelSpeed);
    flywheelOn = true;
    flywheelStartedAt = millis();
  }
}

void flywheelStop() {
  analogWrite(flywheelPin, 0);
  flywheelOn = false;
}

void pushDart() {
  triggerServo.write(triggerFire);
  delay(350);
  triggerServo.write(triggerRest);
  delay(300);
}

void handleFiring(bool obstaclePresent) {
  bool armed = (digitalRead(armSwitchPin) == LOW);

  // Disarmed or already fired this event: everything off.
  if (!armed || shotFiredThisEvent) {
    flywheelStop();
    digitalWrite(warnLedPin, LOW);
    return;
  }

  // Respect the cooldown: stay off until it has passed.
  if (hasFiredBefore && (unsigned long)(millis() - lastShotAt) < cooldownMs) {
    flywheelStop();
    digitalWrite(warnLedPin, LOW);
    return;
  }

  flywheelStart();

  // Warning countdown: fast blink, no firing yet.
  if ((unsigned long)(millis() - pauseStartedAt) < fireDelayMs) {
    digitalWrite(warnLedPin, (millis() / 150) % 2);
    return;
  }

  // Countdown over: wait for full wheel speed and a target that is still there.
  digitalWrite(warnLedPin, HIGH);
  if ((unsigned long)(millis() - flywheelStartedAt) < spinUpMs) return;
  if (!obstaclePresent) return;

  Serial.print("FIRE at ");
  Serial.print(currentAngle);
  Serial.println(" deg");

  pushDart();
  flywheelStop();
  digitalWrite(warnLedPin, LOW);

  shotFiredThisEvent = true;
  hasFiredBefore = true;
  lastShotAt = millis();
}

// ---------- Scanner helpers ----------
void resumeScanning(const char* reason, bool timedOut) {
  isPaused = false;
  // If the timer expires while an obstacle is still present, keep scanning
  // until that obstacle leaves range so we do not immediately pause again.
  ignoreObstacleUntilClear = timedOut;
  missCounter = 0;
  hitCounter = 0;
  clearCounter = 0;

  shotFiredThisEvent = false;
  flywheelStop();
  digitalWrite(warnLedPin, LOW);

  Serial.println(reason);
  tankServo.attach(servoPin);
  tankServo.write(currentAngle);
  delay(150);
}

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(flywheelPin, OUTPUT);
  analogWrite(flywheelPin, 0);          // flywheels off at boot
  pinMode(armSwitchPin, INPUT_PULLUP);
  pinMode(warnLedPin, OUTPUT);
  digitalWrite(warnLedPin, LOW);

  triggerServo.attach(triggerPin);
  triggerServo.write(triggerRest);

  tankServo.attach(servoPin);
  tankServo.write(currentAngle);

  delay(1000);
  Serial.println("System Ready. Scanning...");
  Serial.println(digitalRead(armSwitchPin) == LOW ? "Launcher ARMED" : "Launcher safe (switch open)");
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

  // Debounce: confirm a detection after minHits readings, and re-arm after
  // maxMisses clear readings following a timeout.
  if (ignoreObstacleUntilClear) {
    hitCounter = 0;
    if (!obstaclePresent) {
      if (++clearCounter >= maxMisses) {
        ignoreObstacleUntilClear = false;
        clearCounter = 0;
        Serial.println("Obstacle cleared. Detection re-armed.");
      }
    } else {
      clearCounter = 0;
    }
  } else if (obstaclePresent) {
    if (hitCounter < minHits) hitCounter++;
  } else {
    hitCounter = 0;
  }

  bool confirmedObstacle = !ignoreObstacleUntilClear && (hitCounter >= minHits);

  if (!isPaused && confirmedObstacle) {
    isPaused = true;
    pauseStartedAt = millis();
    missCounter = 0;
    Serial.print("Obstacle detected at ");
    Serial.print(distance);
    Serial.print(" cm, ");
    Serial.print(currentAngle);
    Serial.print(" deg. Pausing scan for up to ");
    Serial.print(maxPauseMs / 1000UL);
    Serial.println(" seconds.");
    tankServo.detach();
  } else if (isPaused) {
    if (obstaclePresent) {
      missCounter = 0;
    } else if (missCounter < maxMisses) {
      missCounter++;
    }

    // Resume when 10 consecutive readings report no obstacle.
    if (missCounter >= maxMisses) {
      resumeScanning("Obstacle disappeared. Resuming scan.", false);
    }
  }

  // Launcher logic only runs while paused on a target.
  if (isPaused) {
    handleFiring(obstaclePresent);
  }

  // Resume after the pause limit even if the obstacle remains present.
  if (isPaused && (unsigned long)(millis() - pauseStartedAt) >= maxPauseMs) {
    resumeScanning("Pause time elapsed. Resuming scan.", true);
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
