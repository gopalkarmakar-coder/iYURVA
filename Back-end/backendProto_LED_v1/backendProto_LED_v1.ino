#include <ArduinoBLE.h>
#include <Adafruit_NeoPixel.h>

// =========================================================================
//                  GLOBAL TWEAKABLE MASTER CONSTANTS
// =========================================================================
const int BASE_DELAY_MS       = 300; // Master speed unit (ms). Change to speed up/slow down everything!
const uint8_t BASE_BRIGHTNESS = 70; // Master brightness limit (0 - 255)

// Hardware Configuration
#define LED_PIN       6     // Pin connected to the NeoPixel ring
#define NUMPIXELS     16     // Number of LEDs in the ring

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// BLE UUIDs matching your web front-end
const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char* CHAR_RX_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

BLEService iyurvaService(SERVICE_UUID);
BLEStringCharacteristic rxChar(CHAR_RX_UUID, BLEWrite, 128); 

// Helper Color Definitions
const uint32_t COLOR_OFF    = pixels.Color(0, 0, 0);
const uint32_t COLOR_WHITE  = pixels.Color(200, 200, 200);
const uint32_t COLOR_BLUE   = pixels.Color(0, 0, 255);
const uint32_t COLOR_GREEN  = pixels.Color(0, 255, 0);
const uint32_t COLOR_PURPLE = pixels.Color(180, 0, 220);
const uint32_t COLOR_ORANGE = pixels.Color(255, 50, 10);  // Replaced Yellow with Orange
const uint32_t COLOR_RED    = pixels.Color(255, 0, 0);    // Red for Connection feedback
const uint32_t COLOR_YELLOW = pixels.Color(255, 255, 0);
// Function Declarations
void setRingSolid(uint32_t color);
void flashRing(uint32_t color, int times, float multiplier);
void flashConnection();
void flashDisconnection();
void displaySlotNumber(int slotNum);
void processCommand(String command);

void setup() {
  Serial.begin(115200);
  
  // Initialize LED Ring
  pixels.begin();
  pixels.setBrightness(BASE_BRIGHTNESS); // Global brightness cap
  pixels.clear();
  pixels.show();

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("iYurva-Machine");
  BLE.setAdvertisedService(iyurvaService);
  iyurvaService.addCharacteristic(rxChar);
  BLE.addService(iyurvaService);

  BLE.advertise();
  Serial.println("BLE Peripheral active. Waiting for connections...");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    // Flash Red 2 times on successful connection
    flashConnection();

    while (central.connected()) {
      if (rxChar.written()) {
        String cmd = rxChar.value();
        Serial.print("Received Command: ");
        Serial.println(cmd);
        
        processCommand(cmd);
      }
    }
    
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());

    // Flash Red and Orange alternately on disconnection
    flashDisconnection();
  }
}

// Set entire ring to a single solid color
void setRingSolid(uint32_t color) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}

// Flash the entire ring scaled relative to BASE_DELAY_MS
void flashRing(uint32_t color, int times, float multiplier) {
  int calculatedDelay = (int)(BASE_DELAY_MS * multiplier);
  for (int t = 0; t < times; t++) {
    setRingSolid(color);
    delay(calculatedDelay);
    setRingSolid(COLOR_OFF);
    delay(calculatedDelay);
  }
}

// Connection Sequence: Flash Red 2 times
void flashConnection() {
  flashRing(COLOR_RED, 2, 0.75);
}

// Disconnection Sequence: Alternating Red and yellow flashes
void flashDisconnection() {
  int calculatedDelay = (int)(BASE_DELAY_MS * 0.75);
  for (int t = 0; t < 3; t++) {
    setRingSolid(COLOR_RED);
    delay(calculatedDelay);
    setRingSolid(COLOR_YELLOW);
    delay(calculatedDelay);
  }
  setRingSolid(COLOR_OFF);
}

// Display slot numbers (1 through 12) on an 8-LED ring
void displaySlotNumber(int slotNum) {
  pixels.clear();
  
  if (slotNum <= 0 || slotNum > 12) {
    pixels.show();
    return;
  }

  if (slotNum <= 8) {
    // 1 to 8: Turn on corresponding count in White
    for (int i = 0; i < slotNum; i++) {
      pixels.setPixelColor(i, COLOR_WHITE);
    }
  } else {
    // 9 to 12: First LED is Blue, remaining corresponding count (slotNum - 8) in White
    pixels.setPixelColor(0, COLOR_BLUE);
    int whiteCount = slotNum - 8;
    for (int i = 1; i <= whiteCount; i++) {
      pixels.setPixelColor(i, COLOR_WHITE);
    }
  }
  
  pixels.show();
}

// Parse and execute BLE incoming commands
void processCommand(String command) {
  // Check for Boiler Flush Command
  if (command == "CMD|BOILER_FLUSH") {
    flashRing(COLOR_PURPLE, 3, 1.25); // 1.25x BASE_DELAY_MS per pulse
    return;
  }

  // Check for Dispense Command
  if (command.startsWith("DISPENSE|")) {
    bool isDrink = command.indexOf("Water") != -1;

    // Flash Mode Indicator (Orange for Drink / Blue for Packing)
    if (isDrink) {
      flashRing(COLOR_ORANGE, 2, 1.0); // 1.0x BASE_DELAY_MS per pulse
    } else {
      flashRing(COLOR_BLUE, 2, 1.0);   // 1.0x BASE_DELAY_MS per pulse
    }

    // Iterate through items in the payload
    int startIndex = 0;
    while (startIndex < command.length()) {
      int nextIndex = command.indexOf('|', startIndex);
      if (nextIndex == -1) nextIndex = command.length();

      String token = command.substring(startIndex, nextIndex);
      
      // Parse slot token (e.g., "Slot 9, 1.50g")
      if (token.startsWith("Slot ")) {
        int commaIdx = token.indexOf(',');
        if (commaIdx != -1) {
          int slotNum = token.substring(5, commaIdx).toInt();
          
          // Display slot number on ring LED
          displaySlotNumber(slotNum);
          
          // Hold slot visualization for 5.0x BASE_DELAY_MS
          delay((int)(BASE_DELAY_MS * 5.0)); 
        }
      }

      startIndex = nextIndex + 1;
    }

    // Dispensing Complete: Flash Green Light
    flashRing(COLOR_GREEN, 3, 2.25); // 1.25x BASE_DELAY_MS per pulse
  }
}