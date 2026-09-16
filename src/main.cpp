#include <Arduino.h>

// Визначаємо піни для ESP32-S3
const int RELAY_CTRL_PIN = 4;  // Керування котушкою
const int RELAY_SENSE_PIN = 5; // Зчитування сухого контакту

void setup() {
  Serial.begin(115200);
  
  // Налаштовуємо піни
  pinMode(RELAY_CTRL_PIN, OUTPUT);
  digitalWrite(RELAY_CTRL_PIN, LOW); // Реле вимкнено за замовчуванням
  
  pinMode(RELAY_SENSE_PIN, INPUT_PULLUP); // Внутрішня підтяжка
  
  Serial.println("\n--- Система ініціалізована ---");
  delay(1000);
}

void loop() {
  // Поки що порожньо
}