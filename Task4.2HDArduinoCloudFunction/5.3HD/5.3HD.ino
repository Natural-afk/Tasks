#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

// ----- Wi-Fi Credentials -----
#define WIFI_SSID       "PRE170"
#define WIFI_PASSWORD   "seno8686"

// ----- Firebase Info -----
const char* host = "remotetrafficcontrol-36828-default-rtdb.firebaseio.com";
const int httpsPort = 443;
WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, host, httpsPort);

// ----- LED Pins -----
const int redPin = 2;
const int yellowPin = 3;
const int greenPin = 4;

void setup() {
  Serial.begin(9600);

  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
}

void loop() {
  client.get("/traffic/command.json");

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Firebase says: ");
  Serial.println(response);

  response.trim();
  response.replace("\"", "");

  // Reset all LEDs
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, LOW);
  digitalWrite(greenPin, LOW);

  if (response == "red") digitalWrite(redPin, HIGH);
  else if (response == "yellow") digitalWrite(yellowPin, HIGH);
  else if (response == "green") digitalWrite(greenPin, HIGH);

  delay(3000); // Poll every 3 seconds
}

