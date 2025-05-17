#include <WiFiNINA.h>
#include <WiFiSSLClient.h>
#include <PubSubClient.h>
//#include <Arduino.h>

#define WIFI_SSID       "PRE170"
#define WIFI_PASSWORD   "seno8686"

#define MQTT_SERVER     "0b254a7084bc40f2bbaf0407610a381b.s1.eu.hivemq.cloud"
#define MQTT_PORT       8883
#define MQTT_USER       "hivemq.webclient.1747462423953"
#define MQTT_PASSWORD   "L3XRH6SEb?wa@:p4h&1e"
#define MQTT_TOPIC      "smartbin/status"

#define PIR_PIN         2
#define GREEN_LED_PIN   12
#define RED_LED_PIN     11
#define BUZZER_PIN      4
#define TRIG_PIN        9
#define ECHO_PIN        10
#define FULL_THRESHOLD_CM 20

WiFiSSLClient wifiClient;
PubSubClient client(wifiClient);

unsigned long lastMQTTPublish = 0;
unsigned long motionTimer = 0;

void setup_wifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("SmartBinClient", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println(" connected!");
    } else {
      Serial.print(" failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

long readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;
  return duration / 58;
}

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);

}

void loop() {


  if (!client.connected()) {
    reconnect_mqtt();
    return;
  }

  client.loop();

  // Read sensors
  int pir = digitalRead(PIR_PIN);
  if (pir == HIGH) motionTimer = millis();
  bool motion = (millis() - motionTimer < 3000);

  long distance = readDistanceCM();

  // LED + buzzer logic
  if (distance > 0 && distance < FULL_THRESHOLD_CM) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
  } else if (motion) {
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // DEBUG PRINTS
  Serial.print("LOOP RUNNING | PIR: ");
  Serial.print(pir);
  Serial.print(" | Motion: ");
  Serial.print(motion);
  Serial.print(" | Distance: ");
  Serial.print(distance);
  Serial.print(" cm | RED: ");
  Serial.print(digitalRead(RED_LED_PIN));
  Serial.print(" | GREEN: ");
  Serial.print(digitalRead(GREEN_LED_PIN));
  Serial.print(" | BUZZER: ");
  Serial.println(digitalRead(BUZZER_PIN));

  // MQTT publish every 2s
  if (millis() - lastMQTTPublish >= 2000) {
    lastMQTTPublish = millis();
    String payload = "{ \"motion\": ";
    payload += motion;
    payload += ", \"distance\": ";
    payload += distance;
    payload += " }";

    Serial.println("Publishing MQTT: " + payload);
    client.publish(MQTT_TOPIC, payload.c_str());
  }
}

