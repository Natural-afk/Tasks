#include "DHT.h"

// === Pin Assignments ===
const int buttonPin = 2;
const int pirPin = 3;
const int dhtPin = 12;
const int led1Pin = 10; // Red
const int led2Pin = 5;  // Blue
const int led3Pin = 8;  // Yellow

// === DHT Setup ===
#define DHTTYPE DHT22
DHT dht(dhtPin, DHTTYPE);
unsigned long lastDHTRead = 0;
const unsigned long dhtInterval = 5000;

// === PIR Setup ===
volatile bool motionDetected = false;
unsigned long lastMotionTime = 0;
const unsigned long pirCooldown = 5000;

// === Button Setup ===
volatile bool led1State = false;
unsigned long lastButtonInterrupt = 0;
const unsigned long debounceDelay = 200;

// === Timer for LED3 ===
bool led3State = false;
unsigned long lastLED3Toggle = 0;
const unsigned long led3Interval = 1000;

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(pirPin, INPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(pirPin), handleMotionInterrupt, RISING);

  Serial.println("System Ready: DHT22 + PIR + Button + Timer");
}

void loop() {
  unsigned long now = millis();

  // === DHT22 Read ===
  if (now - lastDHTRead >= dhtInterval) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (!isnan(temp) && !isnan(hum)) {
      Serial.print("Temp: "); Serial.print(temp); Serial.print(" °C | ");
      Serial.print("Humidity: "); Serial.print(hum); Serial.println(" %");
    } else {
      Serial.println("Failed to read from DHT22");
    }

    lastDHTRead = now;
  }

  // === PIR Motion → LED2 briefly
  if (motionDetected) {
    Serial.println("Motion interrupt → LED2 ON");
    activateOnlyLED(led2Pin);
    delay(1000);
    digitalWrite(led2Pin, LOW);
    motionDetected = false;
  }

  // === Button → Toggle LED1
  digitalWrite(led1Pin, led1State ? HIGH : LOW);

  // === LED3 Timer Toggle
  if (now - lastLED3Toggle >= led3Interval) {
    led3State = !led3State;
    Serial.println("LED3 toggled (timer)");
    activateOnlyLED(led3State ? led3Pin : -1);  // Turn on if true, else off
    lastLED3Toggle = now;
  }
}

// === Interrupt Handlers ===
void handleButtonInterrupt() {
  unsigned long now = millis();
  if (now - lastButtonInterrupt > debounceDelay) {
    led1State = !led1State;
    Serial.print("Button interrupt → LED1: ");
    Serial.println(led1State ? "ON" : "OFF");
    lastButtonInterrupt = now;
  }
}

void handleMotionInterrupt() {
  unsigned long now = millis();
  if (now - lastMotionTime > pirCooldown) {
    motionDetected = true;
    lastMotionTime = now;
  }
}

// === Utility: Only one LED ON at a time
void activateOnlyLED(int activePin) {
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(led3Pin, LOW);

  if (activePin >= 0) {
    digitalWrite(activePin, HIGH);
  }
}

