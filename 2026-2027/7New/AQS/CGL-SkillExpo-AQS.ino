/*
  CGL+ LabZ — MQ-2 Air Quality Sensor
  Board: Arduino UNO

  MQ-2 DO  -> D2
  MQ-2 AO  -> A0
  Red LED  -> D3 (through resistor)
  Green LED -> D5 (through resistor)
  Buzzer   -> D8
  Common GND

  Serial output: 9600 baud
  No startup delay, yellow LED, or fan.
*/

const byte MQ2_DO = 2;
const byte MQ2_AO = A0;
const byte RED_LED = 3;
const byte GREEN_LED = 5;
const byte BUZZER = 8;

const unsigned long REPORT_INTERVAL = 1000;
const unsigned long BUZZER_INTERVAL = 300;

unsigned long lastReport = 0;
unsigned long lastBuzzerToggle = 0;

bool alarmActive = false;
bool buzzerState = false;

void setup() {
  pinMode(MQ2_DO, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BUZZER, LOW);

  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();

  int digitalReading = digitalRead(MQ2_DO);
  int analogReading = analogRead(MQ2_AO);

  // Most MQ-2 comparator modules pull DO LOW on detection.
  alarmActive = (digitalReading == LOW);

  if (alarmActive) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);

    if (now - lastBuzzerToggle >= BUZZER_INTERVAL) {
      lastBuzzerToggle = now;
      buzzerState = !buzzerState;
      digitalWrite(BUZZER, buzzerState ? HIGH : LOW);
    }
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(BUZZER, LOW);

    buzzerState = false;
    lastBuzzerToggle = now;
  }

  // Send telemetry once per second without blocking the alarm.
  if (now - lastReport >= REPORT_INTERVAL) {
    lastReport = now;

    Serial.print("{\"sensor\":\"MQ-2\",\"analog\":");
    Serial.print(analogReading);
    Serial.print(",\"digital\":");
    Serial.print(digitalReading);
    Serial.print(",\"alert\":");
    Serial.print(alarmActive ? "true" : "false");
    Serial.println("}");
  }
}