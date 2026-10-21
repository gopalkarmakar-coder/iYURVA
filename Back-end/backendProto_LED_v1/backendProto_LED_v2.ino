#include <ArduinoBLE.h>
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

// Master speed unit (ms)
const int BASE_DELAY_MS = 300; 

// BLE UUIDs
const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char* CHAR_RX_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

BLEService iyurvaService(SERVICE_UUID);
BLEStringCharacteristic rxChar(CHAR_RX_UUID, BLEWrite, 128); 

bool isConnected = false;

// =========================================================================
//            DYNAMIC BITMAP CONVERTER (8 Rows x 12 Columns)
// =========================================================================
void drawBitmap(const uint8_t sprite[8][12]) {
  uint32_t frame[3] = {0, 0, 0};
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 12; c++) {
      if (sprite[r][c]) {
        int bitIndex = r * 12 + c;
        int element = bitIndex / 32;
        int shift = 31 - (bitIndex % 32);
        frame[element] |= ((uint32_t)1 << shift);
      }
    }
  }
  matrix.loadFrame(frame);
}

void clearScreen() {
  uint8_t blank[8][12] = {0};
  drawBitmap(blank);
}

// =========================================================================
//                   VISUAL 8x12 SPRITE DEFINITIONS
// =========================================================================

const uint8_t BT_ICON[8][12] = {
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,1,0,1,0,0,0,0},
  {0,0,0,0,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,1,0,1,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0}
};

const uint8_t TICK_ICON[8][12] = {
  {0,0,0,0,0,0,0,0,0,0,1,1},
  {0,0,0,0,0,0,0,0,0,1,1,0},
  {0,0,0,0,0,0,0,0,1,1,0,0},
  {1,0,0,0,0,0,0,1,1,0,0,0},
  {1,1,0,0,0,0,1,1,0,0,0,0},
  {0,1,1,0,0,1,1,0,0,0,0,0},
  {0,0,1,1,1,1,0,0,0,0,0,0},
  {0,0,0,1,1,0,0,0,0,0,0,0}
};

// Double Tick Icon (Two parallel checks across 12 columns)
const uint8_t DOUBLE_TICK_ICON[8][12] = {
  {0,0,0,0,0,0,0,0,0,1,0,0},
  {0,0,0,0,0,0,0,0,1,0,0,1},
  {0,0,0,0,0,0,0,1,0,0,1,0},
  {0,1,0,0,1,0,1,0,0,1,0,0},
  {0,0,1,0,0,1,0,0,1,0,0,0},
  {0,0,0,1,1,0,1,1,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0}
};

const uint8_t CROSS_ICON[8][12] = {
  {0,1,1,0,0,0,0,0,0,1,1,0},
  {0,0,1,1,0,0,0,0,1,1,0,0},
  {0,0,0,1,1,0,0,1,1,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,1,1,0,0,1,1,0,0,0},
  {0,0,1,1,0,0,0,0,1,1,0,0},
  {0,1,1,0,0,0,0,0,0,1,1,0}
};

const uint8_t GLASS_1[8][12] = {
  {0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,0,0,0,0,1,0,0,0},
  {0,0,0,1,0,0,0,0,1,0,0,0},
  {0,0,0,1,1,0,0,1,1,0,0,0},
  {0,0,0,1,1,0,0,1,1,0,0,0},
  {0,0,0,0,1,0,0,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,0,0}
};

const uint8_t GLASS_2[8][12] = {
  {0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,0,0,0,0,1,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,0,0}
};

const uint8_t POUCH_ICON[8][12] = {
  {0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,1,0,0,0,0,1,0,0,0},
  {0,0,1,1,1,1,1,1,1,1,0,0},
  {0,0,1,0,0,0,0,0,0,1,0,0},
  {0,0,1,0,0,0,0,0,0,1,0,0},
  {0,0,1,0,0,0,0,0,0,1,0,0},
  {0,0,1,0,0,0,0,0,0,1,0,0},
  {0,0,1,1,1,1,1,1,1,1,0,0}
};

const uint8_t FLUSH_1[8][12] = {
  {1,0,0,1,0,0,1,0,0,1,0,0},
  {0,1,0,0,1,0,0,1,0,0,1,0},
  {0,0,1,0,0,1,0,0,1,0,0,1},
  {1,0,0,1,0,0,1,0,0,1,0,0},
  {0,1,0,0,1,0,0,1,0,0,1,0},
  {0,0,1,0,0,1,0,0,1,0,0,1},
  {1,0,0,1,0,0,1,0,0,1,0,0},
  {0,1,0,0,1,0,0,1,0,0,1,0}
};

const uint8_t FLUSH_2[8][12] = {
  {0,1,0,0,1,0,0,1,0,0,1,0},
  {0,0,1,0,0,1,0,0,1,0,0,1},
  {1,0,0,1,0,0,1,0,0,1,0,0},
  {0,1,0,0,1,0,0,1,0,0,1,0},
  {0,0,1,0,0,1,0,0,1,0,0,1},
  {1,0,0,1,0,0,1,0,0,1,0,0},
  {0,1,0,0,1,0,0,1,0,0,1,0},
  {0,0,1,0,0,1,0,0,1,0,0,1}
};

const uint8_t WATERFALL[3][8][12] = {
  {
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0}
  },
  {
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0}
  },
  {
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1}
  }
};

const uint8_t NUMBERS[12][8][12] = {
  // 1
  {{0,0,0,0,0,0,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,0,0,0},{0,0,0,0,0,1,1,1,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
  // 2
  {{0,0,0,0,0,1,1,1,0,0,0,0},{0,0,0,0,1,0,0,0,1,0,0,0},{0,0,0,0,0,0,0,0,1,0,0,0},{0,0,0,0,0,0,0,1,0,0,0,0},{0,0,0,0,0,0,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,0,0,0},{0,0,0,0,1,1,1,1,1,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
  // 3
  {{0,0,0,0,0,1,1,1,0,0,0,0},{0,0,0,0,1,0,0,0,1,0,0,0},{0,0,0,0,0,0,0,0,1,0,0,0},{0,0,0,0,0,0,1,1,0,0,0,0},{0,0,0,0,0,0,0,0,1,0,0,0},{0,0,0,0,1,0,0,0,1,0,0,0},{0,0,0,0,0,1,1,1,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
  // 4
  {{0,0,0,0,0,0,0,1,0,0,0,0},{0,0,0,0,0,0,1,1,0,0,0,0},{0,0,0,0,0,1,0,1,0,0,0,0},{0,0,0,0,1,0,0,1,0,0,0,0},{0,0,0,0,1,1,1,1,1,0,0,0},{0,0,0,0,0,0,0,1,0,0,0,0},{0,0,0,0,0,0,0,1,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
  // 5
  {{0,0,0,0,1,1,1,1,1,0,0,0},{0,0,0,0,1,0,0,0,0,0,0,0},{0,0,0,0,1,1,1,1,0,0,0,0},{0,0,0,0,0,0,0,0,1,0,0,0},{0,0,0,0,0,0,0,0,1,0,0,0},{0,0,0,0,1,0,0,0,1,0,0,0},{0,0,0,0,0,1,1,1,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
  // 6
  {{0,0,0,0,0,1,1,1,0,0,0,0},{0,0,0,0,1,0,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,0,0,0,0},{0,0,0,0,1,1,1,1,0,0,0,0},{0,0,0,0,1,0,0,0,1,0,0,0},{0,0,0,0,1,0,0,0,1,0,0,0},{0,0,0,0,0,1,1,1,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
  // 7
  {{0,0,0,0,1,1,1,1,1,0,0,0},{0,0,0,0,0,0,0,0,1,0,0,0},{0,0,0,0,0,0,0,1,0,0,0,0},{0,0,0,0,0,0,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
  // 8
  {{0,0,0,0,0,1,1,1,0,0,0,0},{0,0,0,0,1,0,0,0,1,0,0,0},{0,0,0,0,1,0,0,0,1,0,0,0},{0,0,0,0,0,1,1,1,0,0,0,0},{0,0,0,0,1,0,0,0,1,0,0,0},{0,0,0,0,1,0,0,0,1,0,0,0},{0,0,0,0,0,1,1,1,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
  // 9
  {{0,0,0,0,0,1,1,1,0,0,0,0},{0,0,0,0,1,0,0,0,1,0,0,0},{0,0,0,0,1,0,0,0,1,0,0,0},{0,0,0,0,0,1,1,1,1,0,0,0},{0,0,0,0,0,0,0,0,1,0,0,0},{0,0,0,0,0,0,0,0,1,0,0,0},{0,0,0,0,0,1,1,1,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
  // 10
  {{0,1,0,0,0,1,1,1,0,0,0,0},{1,1,0,0,1,0,0,0,1,0,0,0},{0,1,0,0,1,0,0,0,1,0,0,0},{0,1,0,0,1,0,0,0,1,0,0,0},{0,1,0,0,1,0,0,0,1,0,0,0},{0,1,0,0,1,0,0,0,1,0,0,0},{1,1,1,0,0,1,1,1,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
  // 11
  {{0,1,0,0,0,1,0,0,0,0,0,0},{1,1,0,0,1,1,0,0,0,0,0,0},{0,1,0,0,0,1,0,0,0,0,0,0},{0,1,0,0,0,1,0,0,0,0,0,0},{0,1,0,0,0,1,0,0,0,0,0,0},{0,1,0,0,0,1,0,0,0,0,0,0},{1,1,1,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}},
  // 12
  {{0,1,0,0,0,1,1,1,0,0,0,0},{1,1,0,0,1,0,0,0,1,0,0,0},{0,1,0,0,0,0,0,0,1,0,0,0},{0,1,0,0,0,0,0,1,0,0,0,0},{0,1,0,0,0,0,1,0,0,0,0,0},{0,1,0,0,0,1,0,0,0,0,0,0},{1,1,1,0,1,1,1,1,1,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}}
};

// =========================================================================
//                           ANIMATION FUNCTIONS
// =========================================================================

void animateBluetoothBlink() {
  drawBitmap(BT_ICON);
  delay(BASE_DELAY_MS * 2);
  clearScreen();
  delay(BASE_DELAY_MS * 2);
}

void showConnectionSuccess() {
  drawBitmap(TICK_ICON);
  delay(BASE_DELAY_MS * 5);
  clearScreen();
}

void showDisconnection() {
  drawBitmap(CROSS_ICON);
  delay(BASE_DELAY_MS * 5);
  clearScreen();
}

void showGlassAnimation() {
  for (int i = 0; i < 3; i++) {
    drawBitmap(GLASS_1);
    delay(BASE_DELAY_MS * 1.5);
    drawBitmap(GLASS_2);
    delay(BASE_DELAY_MS * 1.5);
  }
}

void showPouchAnimation() {
  for (int i = 0; i < 3; i++) {
    drawBitmap(POUCH_ICON);
    delay(BASE_DELAY_MS * 1.5);
    clearScreen();
    delay(BASE_DELAY_MS * 1.0);
  }
}

void showBoilerFlushAnimation() {
  for (int i = 0; i < 4; i++) {
    drawBitmap(FLUSH_1);
    delay(BASE_DELAY_MS * 1.2);
    drawBitmap(FLUSH_2);
    delay(BASE_DELAY_MS * 1.2);
  }
}

void showWaterfallAnimation() {
  for (int cycle = 0; cycle < 2; cycle++) {
    for (int frame = 0; frame < 3; frame++) {
      drawBitmap(WATERFALL[frame]);
      delay(BASE_DELAY_MS * 1.2);
    }
  }
}

void showDoubleTick() {
  drawBitmap(DOUBLE_TICK_ICON);
  delay(BASE_DELAY_MS * 6);
  clearScreen();
}

void displaySlotNumber(int slotNum) {
  if (slotNum >= 1 && slotNum <= 12) {
    drawBitmap(NUMBERS[slotNum - 1]);
  } else {
    clearScreen();
  }
}

// =========================================================================
//                         MAIN CODE & BLE LOGIC
// =========================================================================

void setup() {
  Serial.begin(115200);
  matrix.begin();

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
    if (!isConnected) {
      isConnected = true;
      Serial.print("Connected to central: ");
      Serial.println(central.address());

      // Show Tick mark on connect
      showConnectionSuccess();
    }

    while (central.connected()) {
      if (rxChar.written()) {
        String cmd = rxChar.value();
        Serial.print("Received Command: ");
        Serial.println(cmd);
        
        processCommand(cmd);
      }
    }
    
    // Disconnection event
    isConnected = false;
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());

    // Show Cross mark on disconnect
    showDisconnection();
  } else {
    // Keep flashing Bluetooth icon while disconnected or waiting
    animateBluetoothBlink();
  }
}

void processCommand(String command) {
  if (command == "CMD|BOILER_FLUSH") {
    showBoilerFlushAnimation();
    clearScreen();
    return;
  }

  if (command.startsWith("DISPENSE|")) {
    bool isDrink = command.indexOf("Water") != -1;

    if (isDrink) {
      showGlassAnimation();
    } else {
      showPouchAnimation();
    }

    int startIndex = 0;
    while (startIndex < command.length()) {
      int nextIndex = command.indexOf('|', startIndex);
      if (nextIndex == -1) nextIndex = command.length();

      String token = command.substring(startIndex, nextIndex);
      
      if (token.startsWith("Slot ")) {
        int commaIdx = token.indexOf(',');
        if (commaIdx != -1) {
          int slotNum = token.substring(5, commaIdx).toInt();
          
          displaySlotNumber(slotNum);
          delay(BASE_DELAY_MS * 6); 
        }
      }

      startIndex = nextIndex + 1;
    }

    showWaterfallAnimation();
    showDoubleTick();
  }
}