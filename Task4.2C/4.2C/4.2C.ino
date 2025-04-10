const int buttonPin = 2;
const int pirPin = 3;
const int led1Pin = 10;
const int led2Pin = 5;

volatile bool led1State = false;
volatile bool motionDetected = false;

unsigned long lastMotionTime = 0;
unsigned long lastButtonInterrupt = 0;
const unsigned long debounceDelay = 200;   // ms
const unsigned long pirCooldown = 5000;    // ms - 5 seconds

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(pirPin, INPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(buttonPin), toggleLED1, FALLING);
  attachInterrupt(digitalPinToInterrupt(pirPin), handleMotion, RISING);

  Serial.println("System ready: interrupts armed.");
}

void loop() {
  // Reflect LED1 state
  digitalWrite(led1Pin, led1State);

  // Handle motion-triggered LED
  if (motionDetected) {
    digitalWrite(led2Pin, HIGH);
    Serial.println("Motion interrupt → LED2 ON");

    delay(1000);  // Keep LED2 on for 1s
    digitalWrite(led2Pin, LOW);

    motionDetected = false;
  }
}

// Button ISR
void toggleLED1() {
  unsigned long now = millis();
  if (now - lastButtonInterrupt > debounceDelay) {
    led1State = !led1State;
    Serial.print("Button interrupt → LED1: ");
    Serial.println(led1State ? "ON" : "OFF");
    lastButtonInterrupt = now;
  }
}

// PIR ISR
void handleMotion() {
  unsigned long now = millis();
  if (now - lastMotionTime > pirCooldown) {
    motionDetected = true;
    lastMotionTime = now;
  }
}

