#include <WiFiNINA.h>          // Use WiFiNINA for Arduino Nano 33 IoT
#include <WiFiSSLClient.h>      // SSL Client for secure MQTT communication (from WiFiNINA)
#include <PubSubClient.h>       // MQTT library

// ----- Wi-Fi Credentials -----
#define WIFI_SSID       "PRE170"
#define WIFI_PASSWORD   "seno8686"

// ----- MQTT Broker Credentials -----
#define MQTT_SERVER     "0b254a7084bc40f2bbaf0407610a381b.s1.eu.hivemq.cloud"
#define MQTT_PORT       8883
#define MQTT_TOPIC      "sensor/motion"
#define MQTT_USER       "hivemq.webclient.1742372852403"   
#define MQTT_PASSWORD   "x1n@Kp0Df!5Wc:I3T?sJ"             

// ----- PIR Sensor Pin -----
const int pirSensorPin = 12;  // GPIO 12 on Arduino Nano 33 IoT for PIR sensor input
const int ledPin = 13;        // GPIO 13 for the onboard LED (Arduino Nano 33 IoT)
int pirState = LOW;           // We start, assuming no motion detected
int val = 0;                  // Variable for reading the pin status

// Wi-Fi and MQTT Clients
WiFiSSLClient wifiSSLClient;  // Use WiFiSSLClient from WiFiNINA for secure connection
PubSubClient mqttClient(wifiSSLClient);

void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT broker...");
    String clientID = "ArduinoNano33IoT_" + String(random(0xffff), HEX);
    
    if (mqttClient.connect(clientID.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT broker!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" | Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(pirSensorPin, INPUT_PULLDOWN);  // Set the PIR sensor pin as input with internal pull-down
  pinMode(ledPin, OUTPUT);  // Set the LED pin as output

  // Connect to Wi-Fi
  while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set up MQTT Server
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  // Connect to MQTT
  connectMQTT();
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();  // Keep MQTT connection alive

  // Read the PIR sensor (motion detection)
  val = digitalRead(pirSensorPin);

  if (val == HIGH) {  // If motion is detected (HIGH)
    digitalWrite(ledPin, HIGH);  // Turn the LED ON
    
    if (pirState == LOW) {
      Serial.println("Motion detected!");  // Print on output change
      mqttClient.publish(MQTT_TOPIC, "Motion detected");  // Publish to MQTT
      pirState = HIGH;
    }
  } else {  // If no motion is detected (LOW)
    digitalWrite(ledPin, LOW);  // Turn the LED OFF
    
    if (pirState == HIGH) {
      Serial.println("Motion ended!");  // Print on output change
      mqttClient.publish(MQTT_TOPIC, "No motion detected");  // Publish to MQTT
      pirState = LOW;
    }
  }

  delay(1000); // Wait for 1 second before checking again
}


