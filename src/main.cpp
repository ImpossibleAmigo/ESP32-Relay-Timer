#include <Arduino.h>

const int RELAY_CTRL_PIN = 4;
const int RELAY_SENSE_PIN = 5;

volatile unsigned long startTime = 0;
volatile unsigned long measuredDelay = 0;
volatile bool resultReady = false;

const int NUM_MEASUREMENTS = 10;
unsigned long onDelays[NUM_MEASUREMENTS];
unsigned long offDelays[NUM_MEASUREMENTS];
bool testCompleted = false;

void IRAM_ATTR senseISR() {
  if (!resultReady) {
    measuredDelay = micros() - startTime;
    resultReady = true;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_CTRL_PIN, OUTPUT);
  digitalWrite(RELAY_CTRL_PIN, LOW);
  pinMode(RELAY_SENSE_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RELAY_SENSE_PIN), senseISR, CHANGE);
  
  Serial.println("\n--- Початок вимірювання затримки реле ---");
  delay(1000);
}

void loop() {
  if (!testCompleted) {
    for (int i = 0; i < NUM_MEASUREMENTS; i++) {
      // 1. Вимірювання увімкнення
      resultReady = false;
      startTime = micros();
      digitalWrite(RELAY_CTRL_PIN, HIGH);
      
      while (!resultReady && (micros() - startTime < 1000000)) { yield(); }
      onDelays[i] = measuredDelay;
      delay(500); // Чекаємо завершення брязкоту

      // 2. Вимірювання вимкнення
      resultReady = false;
      startTime = micros();
      digitalWrite(RELAY_CTRL_PIN, LOW);
      
      while (!resultReady && (micros() - startTime < 1000000)) { yield(); }
      offDelays[i] = measuredDelay;
      delay(500);
      
      Serial.print("Вимір "); Serial.print(i + 1);
      Serial.print(" -> Увімкнення: "); Serial.print(onDelays[i]);
      Serial.print(" мкс | Вимкнення: "); Serial.print(offDelays[i]);
      Serial.println(" мкс");
    }
    testCompleted = true; // Тимчасово зупиняємо тут
  }
}