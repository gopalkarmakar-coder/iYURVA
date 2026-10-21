#include <ArduinoBLE.h>

// UUIDs matching your web front-end
const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char* CHAR_RX_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

// Set up the BLE Service and RX Characteristic
BLEService iyurvaService(SERVICE_UUID);
// Using a string characteristic with a max length of 64 bytes for commands
BLEStringCharacteristic rxChar(CHAR_RX_UUID, BLEWrite, 64); 

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  // Set the advertised local name and service
  BLE.setLocalName("iYurva-Machine");
  BLE.setAdvertisedService(iyurvaService);
  iyurvaService.addCharacteristic(rxChar);
  BLE.addService(iyurvaService);

  BLE.advertise();
  Serial.println("BLE Peripheral active. Waiting for connections...");
}

void loop() {
  // Listen for BLE central devices connecting
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    // While the central is connected, keep polling for written values
    while (central.connected()) {
      if (rxChar.written()) {
        Serial.print("Received Command: ");
        Serial.println(rxChar.value());
      }
    }
    
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}