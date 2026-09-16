#include <Arduino.h>

const int RELAY_CTRL_PIN = 4;
const int RELAY_SENSE_PIN = 5;

// Змінні для вимірювання часу (volatile для переривань)
volatile unsigned long startTime = 0;
volatile unsigned long measuredDelay = 0;
volatile bool resultReady = false;

// Обробник переривання (ISR)
void IRAM_ATTR senseISR() {
  // Фіксуємо час лише першого спрацювання (ігноруємо брязкіт)
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
  
  // Підключаємо переривання
  attachInterrupt(digitalPinToInterrupt(RELAY_SENSE_PIN), senseISR, CHANGE);
  
  Serial.println("\n--- Система ініціалізована. Переривання підключено ---");
  delay(1000);
}

void loop() {
  // Поки що порожньо
}