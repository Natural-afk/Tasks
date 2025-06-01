#include <DHT.h>
#include "SAMDTimerInterrupt.h"

// === Pin Assignments ===
const int buttonPin = 7;
const int led1Pin = 2;  // Red LED (toggle with button)
const int led2Pin = 3;  // Orange LED (PIR motion)
const int led3Pin = 4;  // Green LED (timer interrupt)
const int pirPin = 11;  // PIR OUT
const int dhtPin = 10;  // DHT22 DATA

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

// === Timer Interrupt Setup ===
#define TIMER_INTERVAL_MS 1000
SAMDTimer ITimer(TIMER_TC3);  // Use TC3 (you can try TC4 or TC5 if needed)
volatile bool led3State = false;

// === Timer ISR ===
void toggleLED3() {
  led3State = !led3State;
  digitalWrite(led3Pin, led3State);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);  // Wait for Serial Monitor to open 
  delay(1000);

  dht.begin();

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(pirPin, INPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(pirPin), handleMotionInterrupt, RISING);

  // Start hardware timer interrupt using SAMDTimerInterrupt
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, toggleLED3)) {
    Serial.println("✅ Timer interrupt started successfully");
  } else {
    Serial.println("❌ Failed to start timer interrupt");
  }

  Serial.println("✅ System Ready: DHT22 + PIR + Button + Hardware Timer Interrupt");
}

void loop() {
  unsigned long now = millis();

  // === DHT22 Read Every 5s ===
  if (now - lastDHTRead >= dhtInterval) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (!isnan(temp) && !isnan(hum)) {
      Serial.print(" Temp: "); Serial.print(temp); Serial.print(" °C | ");
      Serial.print(" Humidity: "); Serial.print(hum); Serial.println(" %");
    } else {
      Serial.println(" Failed to read from DHT22");
    }

    lastDHTRead = now;
  }

  // === PIR Motion → LED2 briefly
  if (motionDetected) {
    Serial.println("🚶 PIR interrupt → LED2 ON");
    digitalWrite(led2Pin, HIGH);
    delay(1000);
    digitalWrite(led2Pin, LOW);
    motionDetected = false;
  }

  // === Button → Toggle LED1
  digitalWrite(led1Pin, led1State ? HIGH : LOW);

  // === LED3 is toggled automatically in the timer ISR
}

// === Button ISR
void handleButtonInterrupt() {
  unsigned long now = millis();
  if (now - lastButtonInterrupt > debounceDelay) {
    led1State = !led1State;
    Serial.print(" Button interrupt → LED1: ");
    Serial.println(led1State ? "ON" : "OFF");
    lastButtonInterrupt = now;
  }
}

// === PIR ISR
void handleMotionInterrupt() {
  unsigned long now = millis();
  if (now - lastMotionTime > pirCooldown) {
    motionDetected = true;
    lastMotionTime = now;
  }
}

