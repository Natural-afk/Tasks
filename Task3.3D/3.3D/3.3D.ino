#include <WiFiNINA.h>
#include <PubSubClient.h>

// ===== Wi-Fi Credentials =====
#define WIFI_SSID       "PRE170"
#define WIFI_PASSWORD   "seno8686"

// ===== MQTT Broker Info =====
#define MQTT_SERVER     "broker.emqx.io"
#define MQTT_PORT       1883
#define MQTT_TOPIC      "SIT210/wave"

// ===== Pin Assignments =====
const int trigPin = 6;     // Ultrasonic TRIG
const int echoPin = 7;     // Ultrasonic ECHO
const int ledPin  = 2;     // LED

// ===== Network Clients =====
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// ===== Timing for Detection =====
unsigned long lastDetectTime = 0;
const long detectInterval = 1000; // 1s check interval

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // === Connect to Wi-Fi ===
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ WiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("❌ WiFi failed to connect.");
    while (true); // Halt
  }

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  connectToMQTT();
}

void loop() {
  if (!mqttClient.connected()) {
    connectToMQTT();
  }
  mqttClient.loop();

  unsigned long now = millis();
  if (now - lastDetectTime > detectInterval) {
    lastDetectTime = now;
    long distance = getDistance();

    if (distance > 0 && distance < 20) {
      Serial.println("👋 Wave detected!");
      mqttClient.publish(MQTT_TOPIC, "Elias has waved 👋");
    }
  }
}

void connectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("Nano33Client")) {
      Serial.println("✅ MQTT connected.");
      mqttClient.subscribe(MQTT_TOPIC);
    } else {
      Serial.print("❌ MQTT failed. Code: ");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg = "";

  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("📩 Message received: ");
  Serial.println(msg);

  if (msg.indexOf("wave") >= 0) {
    // Wave: Flash LED 3x
    for (int i = 0; i < 3; i++) {
      digitalWrite(ledPin, HIGH);
      delay(250);
      digitalWrite(ledPin, LOW);
      delay(250);
    }
  } else if (msg.indexOf("pat") >= 0) {
    // Pat: LED solid ON 1 second
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
  }
}

long getDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000); // timeout 20ms
  if (duration == 0) {
    Serial.println("⚠️ No reading (timeout)");
    return -1;
  }

  long distance = duration * 0.034 / 2;
  Serial.print("📏 Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  return distance;
}

