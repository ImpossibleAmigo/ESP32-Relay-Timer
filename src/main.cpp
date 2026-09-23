#include <Arduino.h>

// Піни підключення
const int RELAY_CTRL_PIN = 4; // Пін керування обмоткою реле (до IN)
const int SENSOR_PIN = 5;     // Пін зчитування сухого контакту (до NO)

// Змінні для вимірювання часу (volatile, бо використовуються в перериванні)
volatile unsigned long triggerTime = 0;
volatile unsigned long actuationTime = 0;
volatile bool measurementReady = false;

// Захист від брязкоту (Debounce)
volatile unsigned long lastInterruptTime = 0;
const unsigned long DEBOUNCE_DELAY_US = 20000; // 20 мс ігнорування брязкоту

// Змінні для статистики
const int MAX_MEASUREMENTS = 10;
int currentCycle = 0;

unsigned long sumOnTime = 0;
unsigned long sumOffTime = 0;
int onCount = 0;
int offCount = 0;

bool isTurningOn = false;

// Обробник переривання
void IRAM_ATTR handleInterrupt() {
  unsigned long currentTime = micros();
  
  // Ігноруємо механічний брязкіт контактів
  if (currentTime - lastInterruptTime > DEBOUNCE_DELAY_US) {
    if (triggerTime > 0) {
      actuationTime = currentTime - triggerTime;
      measurementReady = true;
      triggerTime = 0; // Скидаємо таймер
    }
  }
  lastInterruptTime = currentTime;
}

void setup() {
  Serial.begin(115200);
  
  // Налаштування піна керування (Low-Level Trigger)
  pinMode(RELAY_CTRL_PIN, OUTPUT);
  digitalWrite(RELAY_CTRL_PIN, HIGH); // Спочатку вимкнено (HIGH = вимкнено)
  
  // Налаштування піна зчитування з внутрішньою підтяжкою
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  
  // Налаштовуємо переривання на будь-яку зміну стану (замикання або розмикання)
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), handleInterrupt, CHANGE);
  
  delay(2000);
  Serial.println("=== Початок вимірювання затримки реле ===");
}

void loop() {
  if (currentCycle < MAX_MEASUREMENTS) {
    delay(1500); // Пауза перед наступною дією
    
    isTurningOn = !isTurningOn; 
    measurementReady = false;
    
    // Запам'ятовуємо час рівно перед відправкою сигналу
    triggerTime = micros(); 
    
    if (isTurningOn) {
      digitalWrite(RELAY_CTRL_PIN, LOW); // Вмикаємо реле (LOW = увімкнено)
    } else {
      digitalWrite(RELAY_CTRL_PIN, HIGH); // Вимикаємо реле
    }
    
    // Чекаємо на відповідь від переривання (максимум 1 секунду)
    unsigned long waitStart = millis();
    while (!measurementReady && (millis() - waitStart < 1000)) {}
    
    if (measurementReady) {
      if (isTurningOn) {
        sumOnTime += actuationTime;
        onCount++;
        Serial.printf("Цикл %d | УВІМКНЕННЯ: %lu мкс\n", onCount, actuationTime);
      } else {
        sumOffTime += actuationTime;
        offCount++;
        Serial.printf("Цикл %d | ВИМКНЕННЯ:  %lu мкс\n", offCount, actuationTime);
        currentCycle++; // Плюсуємо цикл тільки після повного увімкнення-вимкнення
      }
    } else {
      Serial.println("Помилка: Переривання не спрацювало! Перевір контакти NO та COM.");
      triggerTime = 0;
    }
    
  } else if (currentCycle == MAX_MEASUREMENTS) {
    Serial.println("\n=== ФІНАЛЬНІ РЕЗУЛЬТАТИ (Середнє значення) ===");
    Serial.printf("Середній час УВІМКНЕННЯ: %lu мкс\n", sumOnTime / onCount);
    Serial.printf("Середній час ВИМКНЕННЯ:  %lu мкс\n", sumOffTime / offCount);
    Serial.println("================================================");
    
    currentCycle++; // Зупиняємо виконання
  }
}