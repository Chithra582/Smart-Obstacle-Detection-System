
/*
  OBSTACLE DETECTION SYSTEM FOR THE VISUALLY IMPAIRED
  Board: Arduino Uno R3
*/

#define TRIG_PIN 9
#define ECHO_PIN 10
#define PIR_PIN 2

#define BUZZER_PIN 8
#define MOTOR_PIN 6

#define LED_RED 3
#define LED_GREEN 4
#define LED_BLUE 5

const float DANGER_CM = 50.0;
const float CAUTION_CM = 150.0;

const unsigned long LOG_INTERVAL_MS = 500;

float distanceCm = 0.0;
bool motionDetected = false;
unsigned long lastLogTime = 0;

float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);

  if (duration == 0) {
    return -1.0;  // No echo received
  }

  return duration * 0.0343 / 2.0;
}

void setIndicators(bool red, bool green, bool blue) {
  digitalWrite(LED_RED, red ? HIGH : LOW);
  digitalWrite(LED_GREEN, green ? HIGH : LOW);
  digitalWrite(LED_BLUE, blue ? HIGH : LOW);
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  digitalWrite(TRIG_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(MOTOR_PIN, LOW);

  setIndicators(false, true, false);

  Serial.println("time_ms,distance_cm,motion,status");
}

void loop() {
  distanceCm = readDistanceCm();
  motionDetected = digitalRead(PIR_PIN) == HIGH;

  String status;

  if (distanceCm < 0) {
    status = "NO_ECHO";
    setIndicators(false, false, true);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(MOTOR_PIN, LOW);
  } else if (distanceCm <= DANGER_CM) {
    status = "DANGER";
    setIndicators(true, false, false);
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(MOTOR_PIN, HIGH);
  } else if (distanceCm <= CAUTION_CM) {
    status = "CAUTION";
    setIndicators(false, false, true);
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(MOTOR_PIN, LOW);
  } else {
    status = "SAFE";
    setIndicators(false, true, false);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(MOTOR_PIN, LOW);
  }

  if (millis() - lastLogTime >= LOG_INTERVAL_MS) {
    lastLogTime = millis();

    Serial.print(millis());
    Serial.print(",");
    Serial.print(distanceCm, 2);
    Serial.print(",");
    Serial.print(motionDetected ? 1 : 0);
    Serial.print(",");
    Serial.println(status);
  }

  delay(50);
}
