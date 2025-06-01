#include <ArduinoBLE.h>

const int ledPin = 3;  // Using digital pin 3 (PWM)

BLEService parkingService("180C");  // Custom service UUID
BLECharacteristic brightnessChar("2A56", BLEWrite, 20);  // Writable characteristic

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(ledPin, OUTPUT);

  if (!BLE.begin()) {
    Serial.println("BLE start failed!");
    while (1);
  }

  BLE.setLocalName("ParkingActuator");
  BLE.setAdvertisedService(parkingService);
  parkingService.addCharacteristic(brightnessChar);
  BLE.addService(parkingService);
  BLE.advertise();

  Serial.println("Waiting for connection...");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to: ");
    Serial.println(central.address());

    while (central.connected()) {
      if (brightnessChar.written()) {
        int len = brightnessChar.valueLength();
        const uint8_t* val = brightnessChar.value();

        // Print raw bytes for debugging
        Serial.print("Received raw: ");
        for (int i = 0; i < len; i++) {
          Serial.print(val[i], HEX);
          Serial.print(" ");
        }
        Serial.println();

        // Convert to String
        String brightnessStr = "";
        for (int i = 0; i < len; i++) {
          brightnessStr += (char)val[i];
        }

        int brightness = brightnessStr.toInt();
        brightness = constrain(brightness, 0, 255);

        Serial.print("Parsed brightness: ");
        Serial.println(brightness);

        analogWrite(ledPin, brightness);
      }
    }

    Serial.println("Disconnected.");
  }
}


