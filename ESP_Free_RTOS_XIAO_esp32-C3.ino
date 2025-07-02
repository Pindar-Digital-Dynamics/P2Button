
//  * Full FreeRTOS BLE Camera Control Code with Multi-Characteristic Support (ESP32-C3)
//  * This sketch integrates all original functionalities using FreeRTOS.
//  * Copy this entire file into Arduino IDE (.ino).
//  */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "esp_bt.h"               // Needed for BLE transmit power control
#include "esp_gap_ble_api.h"      // For esp_ble_tx_power_set()


// === Pin Definitions ===
#define BUTTON_PIN        21
#define LED_BLUE_PIN      9
#define LED_GREEN_PIN     10
#define LED_RED_PIN       20
#define BUZZER_PIN        5
#define UNPAIR_BUTTON_PIN 3
#define BATTERY_ADC_PIN   34
#define VOLTAGE_PIN       4


#define SCREEN_WIDTH 128      // OLED width
#define SCREEN_HEIGHT 64      // OLED height

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// === Logo Bitmap ===
static const unsigned char PROGMEM epd_bitmap_P2CAMimage[] = {/* P2CAM Logo bitmap data here */
  0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x1f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x7f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x03, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x07, 0xff, 0xc1, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x07, 0xff, 0x80, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0f, 0xff, 0x80, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0f, 0xff, 0x00, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0f, 0xff, 0x00, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0f, 0xff, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x1f, 0xff, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x1f, 0xfe, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x1f, 0xfe, 0x01, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x1f, 0xfe, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3f, 0xfe, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3f, 0xfc, 0x03, 0xff, 0xc0, 0x1f, 0xe0, 0x00, 0x3f, 0x80, 0x00, 0x7f, 0x83, 0xfc, 0x00, 
	0x00, 0x3f, 0xfc, 0x03, 0xff, 0x80, 0xff, 0xfc, 0x01, 0xff, 0xf0, 0x03, 0xff, 0xcf, 0xff, 0x00, 
	0x00, 0x3f, 0xfc, 0x07, 0xff, 0x81, 0xff, 0xfe, 0x07, 0xff, 0xf8, 0x0f, 0xff, 0xff, 0xff, 0x80, 
	0x00, 0x7f, 0xfc, 0x07, 0xff, 0x87, 0xff, 0xfe, 0x0f, 0xff, 0xfc, 0x1f, 0xff, 0xff, 0xff, 0xc0, 
	0x00, 0x7f, 0xfc, 0x07, 0xff, 0x87, 0xf9, 0xff, 0x1f, 0xe3, 0xfc, 0x3f, 0xe7, 0xfe, 0x3f, 0xc0, 
	0x00, 0x7f, 0xf8, 0x07, 0xff, 0x0f, 0xf0, 0xff, 0x3f, 0xc1, 0xfc, 0x3f, 0xc3, 0xfc, 0x3f, 0xc0, 
	0x00, 0x7f, 0xf8, 0x0f, 0xff, 0x0f, 0xf0, 0xfe, 0x3f, 0xc3, 0xfc, 0x7f, 0x83, 0xf8, 0x3f, 0xc0, 
	0x00, 0x7f, 0xf8, 0x0f, 0xff, 0x1f, 0xe0, 0xfe, 0x3f, 0xc3, 0xfc, 0x7f, 0x83, 0xf8, 0x3f, 0xc0, 
	0x00, 0xff, 0xf8, 0x0f, 0xfe, 0x1f, 0xe0, 0xfe, 0x3f, 0x83, 0xfc, 0x7f, 0x87, 0xf8, 0x3f, 0x80, 
	0x00, 0xff, 0xf0, 0x1f, 0xfe, 0x1f, 0xe1, 0xfe, 0x7f, 0x83, 0xf8, 0x7f, 0x07, 0xf8, 0x7f, 0x80, 
	0x00, 0xff, 0xf0, 0x7f, 0xfc, 0x1f, 0xe1, 0xfc, 0x7f, 0x87, 0xf8, 0xff, 0x07, 0xf0, 0x7f, 0x80, 
	0x00, 0xff, 0xff, 0xc0, 0x00, 0x3f, 0xc1, 0xfc, 0x7f, 0x87, 0xf8, 0xff, 0x0f, 0xf0, 0x7f, 0x80, 
	0x01, 0xff, 0xff, 0x80, 0x00, 0x3f, 0xc1, 0xfc, 0x7f, 0x07, 0xf8, 0xff, 0x0f, 0xf0, 0x7f, 0x00, 
	0x01, 0xff, 0xff, 0x1f, 0xf0, 0x3f, 0xc0, 0x00, 0xff, 0x07, 0xf0, 0xfe, 0x0f, 0xf0, 0xff, 0x00, 
	0x01, 0xff, 0xfe, 0x3f, 0xf8, 0x3f, 0xc0, 0x00, 0xff, 0x0f, 0xf1, 0xfe, 0x0f, 0xe0, 0xff, 0x00, 
	0x01, 0xff, 0xfc, 0x7c, 0xfc, 0x3f, 0x80, 0x00, 0xff, 0xff, 0xf1, 0xfe, 0x1f, 0xe0, 0xff, 0x00, 
	0x03, 0xff, 0xfc, 0xf8, 0xfc, 0x7f, 0x80, 0x00, 0xff, 0xff, 0xf1, 0xfe, 0x1f, 0xe0, 0xfe, 0x00, 
	0x03, 0xff, 0xec, 0xf8, 0xf8, 0x7f, 0x80, 0x01, 0xff, 0xff, 0xe1, 0xfc, 0x1f, 0xe1, 0xfe, 0x00, 
	0x03, 0xff, 0xc0, 0xf8, 0xf8, 0x7f, 0x80, 0x01, 0xff, 0xff, 0xe3, 0xfc, 0x1f, 0xc1, 0xfe, 0x00, 
	0x03, 0xff, 0xc0, 0xf0, 0xf8, 0xff, 0x00, 0x01, 0xff, 0xff, 0xe3, 0xfc, 0x3f, 0xc1, 0xfe, 0x00, 
	0x07, 0xff, 0xc0, 0x01, 0xf8, 0xff, 0x00, 0x01, 0xff, 0xff, 0xe3, 0xfc, 0x3f, 0xc1, 0xfc, 0x00, 
	0x07, 0xff, 0x80, 0x07, 0xf0, 0xff, 0x07, 0xf3, 0xfc, 0x1f, 0xc3, 0xf8, 0x3f, 0xc3, 0xfc, 0x00, 
	0x07, 0xff, 0x80, 0x0f, 0xe0, 0xff, 0x0f, 0xe3, 0xfc, 0x3f, 0xc7, 0xf8, 0x3f, 0x83, 0xfc, 0x00, 
	0x07, 0xff, 0x80, 0x3f, 0x81, 0xfe, 0x0f, 0xe3, 0xfc, 0x3f, 0xc7, 0xf8, 0x7f, 0x83, 0xfc, 0x00, 
	0x0f, 0xff, 0x80, 0x7f, 0x01, 0xfe, 0x0f, 0xe3, 0xf8, 0x3f, 0xc7, 0xf8, 0x7f, 0x83, 0xf8, 0x00, 
	0x0f, 0xff, 0x01, 0xfc, 0x01, 0xfe, 0x0f, 0xe7, 0xf8, 0x3f, 0x8f, 0xf0, 0x7f, 0x87, 0xf8, 0x00, 
	0x0f, 0xff, 0x03, 0xf8, 0x01, 0xfe, 0x1f, 0xc7, 0xf8, 0x7f, 0x8f, 0xf0, 0x7f, 0x07, 0xf8, 0x00, 
	0x0f, 0xff, 0x03, 0xe0, 0x03, 0xfc, 0x1f, 0xc7, 0xf8, 0x7f, 0x8f, 0xf0, 0xff, 0x07, 0xf0, 0x00, 
	0x1f, 0xff, 0x07, 0xc0, 0x03, 0xfc, 0x1f, 0xc7, 0xf0, 0x7f, 0x8f, 0xf0, 0xff, 0x0f, 0xf0, 0x00, 
	0x1f, 0xfe, 0x07, 0xc0, 0x03, 0xfc, 0x3f, 0x8f, 0xf0, 0x7f, 0x1f, 0xe0, 0xff, 0x0f, 0xf0, 0x00, 
	0x1f, 0xfe, 0x07, 0x80, 0x03, 0xff, 0xff, 0x8f, 0xf0, 0xff, 0x1f, 0xe0, 0xfe, 0x0f, 0xf0, 0x00, 
	0x1f, 0xfe, 0x0f, 0x80, 0x03, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0x1f, 0xe1, 0xfe, 0x0f, 0xe0, 0x00, 
	0x3f, 0xfe, 0x0f, 0xff, 0x81, 0xff, 0xfe, 0x0f, 0xe0, 0xff, 0x1f, 0xe1, 0xfe, 0x1f, 0xe0, 0x00, 
	0x3f, 0xfe, 0x0f, 0xff, 0x80, 0xff, 0xf8, 0x08, 0x00, 0x00, 0x00, 0x01, 0xfe, 0x00, 0x00, 0x00, 
	0x3f, 0xfc, 0x0f, 0xff, 0x80, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x00
};

// === FreeRTOS objects ===
static QueueHandle_t commandQueue;
static SemaphoreHandle_t bleMutex;



// === Command Types ===
enum CommandType {
  CMD_UNLOCK,
  CMD_START_RECORD,
  CMD_UPLOAD,
  CMD_SETUP,
  CMD_LOGIN,
  CMD_LOGOUT,
  CMD_NONE
};

// === Command Struct ===
struct Command {
  CommandType type;
  String data;
};

// === BLE definitions ===
struct BLECommandDef {
  const char* serviceUUID;
  const char* charUUID;
  bool propRead;
  bool propWrite;
  bool propNotify;
  bool propIndicate;
  CommandType type;
  BLECharacteristic* characteristic;
};

// === BLE characteristic definitions ===
BLECommandDef bleCommands[] = {
  {"19b10000-e8f2-537e-4f6c-d104768a1214","19b10001-e8f2-537e-4f6c-d104768a1214", false,false,false,true,  CMD_UNLOCK,      nullptr},
  {"19b10000-e8f2-537e-4f6c-d104768a1214","19b10002-e8f2-537e-4f6c-d104768a1214", false,true, false,false, CMD_UNLOCK,      nullptr},
  {"19b10000-e8f2-537e-4f6c-d104768a1214","19b10003-e8f2-537e-4f6c-d104768a1214", false,false,false,true,  CMD_START_RECORD,nullptr},
  {"19b10000-e8f2-537e-4f6c-d104768a1214","19b10004-e8f2-537e-4f6c-d104768a1214", false,true, false,false, CMD_START_RECORD,nullptr},
  {"19b10000-e8f2-537e-4f6c-d104768a1214","19b10005-e8f2-537e-4f6c-d104768a1214", false,false,false,true,  CMD_UPLOAD,      nullptr},
  {"19b20000-e8f2-537e-4f6c-d104768a1214","19b20001-e8f2-537e-4f6c-d104768a1214", false,true, false,false, CMD_UPLOAD,      nullptr},
  {"19b20000-e8f2-537e-4f6c-d104768a1214","19b20002-e8f2-537e-4f6c-d104768a1214", false,true, false,false, CMD_SETUP,       nullptr},
  {"19b20000-e8f2-537e-4f6c-d104768a1214","19b20003-e8f2-537e-4f6c-d104768a1214", false,true, false,false, CMD_LOGIN,       nullptr},
  {"19b20000-e8f2-537e-4f6c-d104768a1214","19b20004-e8f2-537e-4f6c-d104768a1214", false,true, false,false, CMD_LOGOUT,      nullptr},
  {"19b20000-e8f2-537e-4f6c-d104768a1214","19b20005-e8f2-537e-4f6c-d104768a1214", true, false, false,false, CMD_NONE,        nullptr}
};



const int NUM_COMMANDS = sizeof(bleCommands)/sizeof(bleCommands[0]);

// === Global state ===
volatile CommandType currentState = CMD_LOGOUT;

// === Task prototypes ===
void BLECommandTask(void* pvParameters);
void ButtonTask(void* pvParameters);
void OLEDTask(void* pvParameters);
void BuzzerTask(void* pvParameters);

// BLE Server callbacks
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    Serial.println("Client Connected!");
  }
  void onDisconnect(BLEServer* pServer) override {
    Serial.println("Client Disconnected!");

    Command cmd = { CMD_LOGOUT, "" };
    if (commandQueue != NULL) {
      xQueueSend(commandQueue, &cmd, 0);
    }

    //  BLE  Advertising
    BLEDevice::startAdvertising(); 
  }
};

// Characteristic callbacks
class CmdCallbacks : public BLECharacteristicCallbacks {
  // void onWrite(BLECharacteristic* pChar) override {
  //   for (int i = 0; i < NUM_COMMANDS; i++) {
  //     if (bleCommands[i].characteristic == pChar && bleCommands[i].propWrite) {
  //       Command cmd = { bleCommands[i].type, pChar->getValue().c_str() };
  //       xQueueSend(commandQueue, &cmd, 0);
  //     }
  //   }
  // }
  void onWrite(BLECharacteristic* pChar) override {
  std::string val = pChar->getValue().c_str();
  String commandStr = String(val.c_str());

  if (commandStr.startsWith("Login")) {
    Command cmd = { CMD_LOGIN, commandStr };
    xQueueSend(commandQueue, &cmd, 0);
  } else if (commandStr.equals("Unlock")) {
    Command cmd = { CMD_UNLOCK, commandStr };
    xQueueSend(commandQueue, &cmd, 0);
  } else if (commandStr.equals("Record")) {
    Command cmd = { CMD_START_RECORD, commandStr };
    xQueueSend(commandQueue, &cmd, 0);
  } else if (commandStr.equals("Upload")) {
    Command cmd = { CMD_UPLOAD, commandStr };
    xQueueSend(commandQueue, &cmd, 0);
  } else if (commandStr.equals("Logout")) {
    Command cmd = { CMD_LOGOUT, commandStr };
    xQueueSend(commandQueue, &cmd, 0);
  } else {
    Serial.println("Invalid command: " + commandStr);
  }
}

  void onRead(BLECharacteristic* pChar) override {
    for (int i = 0; i < NUM_COMMANDS; i++) {
      if (bleCommands[i].characteristic == pChar && bleCommands[i].propRead) {
        uint16_t raw = analogRead(VOLTAGE_PIN);
        float vol = raw * (3.3 / 4095.0);
        char buf[8];
        snprintf(buf, sizeof(buf), "%.2f", vol);
        pChar->setValue(buf);
      }
    }
  }
};

void showLogo() {
  display.clearDisplay();
  display.drawBitmap((SCREEN_WIDTH - 128) / 2,
                     (SCREEN_HEIGHT - 55) / 2,
                     epd_bitmap_P2CAMimage,
                     128, 55,
                     SSD1306_WHITE);
  display.display();
}


void createCharacteristic(BLECommandDef& cmd, BLEService* svc, BLECharacteristicCallbacks* cb) {
  uint32_t props = 0;
  if (cmd.propRead)     props |= BLECharacteristic::PROPERTY_READ;
  if (cmd.propWrite)    props |= BLECharacteristic::PROPERTY_WRITE;
  if (cmd.propNotify)   props |= BLECharacteristic::PROPERTY_NOTIFY;
  if (cmd.propIndicate) props |= BLECharacteristic::PROPERTY_INDICATE;

  Serial.printf("→ Creating %s\n", cmd.charUUID);
  cmd.characteristic = svc->createCharacteristic(cmd.charUUID, props);
  if (!cmd.characteristic) {
    Serial.printf("❌ Failed %s\n", cmd.charUUID);
    return;
  }
  if (cmd.propNotify||cmd.propIndicate) {
    cmd.characteristic->addDescriptor(new BLE2902());
  }
  if (cmd.propRead||cmd.propWrite) {
    cmd.characteristic->setCallbacks(cb);
  }
  Serial.printf("✔️ Created %s\n", cmd.charUUID);
}

// === BLE setup ===
void setupBLE() {
  Serial.println("Initializing BLE...");
  BLEDevice::init("P2CAM");

  // === Set BLE Transmit Power for Maximum Range ===
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9);  // Max +9 dBm
  // You can also optionally configure ADV and CONN transmit power:
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL0, ESP_PWR_LVL_P9);


  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  CmdCallbacks* cmdCB = new CmdCallbacks();

  // Services
  BLEService* svc1 = pServer->createService("19b10000-e8f2-537e-4f6c-d104768a1214");
  BLEService* svc2 = pServer->createService("19b20000-e8f2-537e-4f6c-d104768a1214");

  for (int i = 0; i < NUM_COMMANDS; i++) {
    createCharacteristic(bleCommands[i], (i<5)?svc1:svc2, cmdCB);
  }
  svc1->start();
  svc2->start();

  // Advertising
  auto adv = BLEDevice::getAdvertising();
  adv->addServiceUUID("19b10000-e8f2-537e-4f6c-d104768a1214");
  adv->addServiceUUID("19b20000-e8f2-537e-4f6c-d104768a1214");
  adv->setScanResponse(true);
  BLEDevice::startAdvertising();
  Serial.println("BLE advertising started.");
}

// === Arduino setup / loop ===
void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(VOLTAGE_PIN, INPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(UNPAIR_BUTTON_PIN, INPUT_PULLUP);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  showLogo(); delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("P2Cam Booting...");
  display.display();

  setupBLE();

  commandQueue = xQueueCreate(10, sizeof(Command));
  bleMutex     = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(BLECommandTask, "BLECmd", 4096, NULL, 3, NULL, 0);
  xTaskCreatePinnedToCore(ButtonTask,     "Button", 2048, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(OLEDTask,       "OLED",   4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(BuzzerTask,     "Buzzer", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(LEDTask,        "LED",    2048, NULL, 1, NULL, 0);

}


void loop() {
  // nothing here, FreeRTOS tasks handle everything
}
const char* commandName(CommandType cmd) {
  switch (cmd) {
    case CMD_UNLOCK:        return "CMD_UNLOCK";
    case CMD_START_RECORD:  return "CMD_START_RECORD";
    case CMD_UPLOAD:        return "CMD_UPLOAD";
    case CMD_SETUP:         return "CMD_SETUP";
    case CMD_LOGIN:         return "CMD_LOGIN";
    case CMD_LOGOUT:        return "CMD_LOGOUT";
    case CMD_NONE:          return "CMD_NONE";
    default:                return "UNKNOWN";
  }
}

void BLECommandTask(void* pvParameters) {
  Command cmd;
  for (;;) {
    if (xQueueReceive(commandQueue, &cmd, portMAX_DELAY)) {
      Serial.printf("[BLECommandTask] 🔁 Received command: %s\n", commandName(cmd.type));
      
      xSemaphoreTake(bleMutex, portMAX_DELAY);
      currentState = cmd.type;

      for (int i = 0; i < NUM_COMMANDS; i++) {
        if (bleCommands[i].type == cmd.type && bleCommands[i].propIndicate) {
          bleCommands[i].characteristic->setValue("");
          bleCommands[i].characteristic->indicate();
          Serial.printf("[BLECommandTask] 📡 Indicating: %s\n", commandName(cmd.type));
        }
      }

      xSemaphoreGive(bleMutex);
    }
  }
}


void ButtonTask(void* pvParameters) {
  static unsigned long pressStart = 0;
  static bool countdownStarted = false;
  bool last = HIGH;

  for (;;) {
    bool curr = digitalRead(BUTTON_PIN);
    if (last == HIGH && curr == LOW) {
      pressStart = millis();
      countdownStarted = false;
    } else if (last == LOW && curr == LOW) {
      unsigned long elapsed = millis() - pressStart;
      if ((currentState == CMD_LOGIN || currentState == CMD_START_RECORD) && elapsed >= 500 && !countdownStarted) {
        countdownStarted = true;
      }
      if (countdownStarted) {
        int secs = (elapsed - 500) / 1000;
        int remain = 2 - secs;
        display.clearDisplay(); display.setCursor(0, 0);
        if (remain > 0) {
          if (currentState == CMD_LOGIN) display.printf("Unlock in %d", remain);
          else display.printf("Upload in %d", remain);
        } else {
          display.println("Release button");
        }
        display.display();
      }
      if (elapsed >= 2000) {
        if (currentState == CMD_LOGIN) {
          Command cmd = {CMD_UNLOCK, ""};
          xQueueSend(commandQueue, &cmd, 0);
          vTaskDelay(pdMS_TO_TICKS(1000));
        } else if (currentState == CMD_START_RECORD) {
          Command cmd = {CMD_UPLOAD, ""};
          xQueueSend(commandQueue, &cmd, 0);
          vTaskDelay(pdMS_TO_TICKS(1000));
        }
      }
    } else if (last == LOW && curr == HIGH) {
      // Button released: handle short vs long press
      unsigned long pressDuration = millis() - pressStart;
      // Short press in CAMERA_UNLOCKED state triggers RECORDING
      if (currentState == CMD_UNLOCK && pressDuration < 2000) {
        Command cmd = {CMD_START_RECORD, ""};
        xQueueSend(commandQueue, &cmd, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
      }
      // Reset countdown for long-press transitions
      countdownStarted = false;
    }
    last = curr;
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}


// void ButtonTask(void* pvParameters) {
//   static unsigned long pressStart = 0;
//   static bool countdownStarted = false;
//   bool last = HIGH;

//   for (;;) {
//     bool curr = digitalRead(BUTTON_PIN);

//     if (last == HIGH && curr == LOW) {
//       pressStart = millis();
//       countdownStarted = false;
//     }

    
//     else if (last == LOW && curr == LOW) {
//       unsigned long elapsed = millis() - pressStart;

      
//       if (elapsed >= 500 && !countdownStarted) {
//         countdownStarted = true;
//       }

    
//       if (countdownStarted) {
//         int remain = 2 - (elapsed - 500) / 1000;
//         display.clearDisplay();
//         display.setCursor(0, 0);
//         if (remain > 0) display.printf("Hold: %d", remain);
//         else            display.println("Release!");
//         display.display();
//       }

      
//       if (elapsed >= 2000) {
//         Command cmd;
//         switch (currentState) {
//           case CMD_LOGIN:
//             cmd = { CMD_UNLOCK, "" };
//             break;
//           case CMD_UNLOCK:
//             cmd = { CMD_START_RECORD, "" };
//             break;
//           case CMD_START_RECORD:
//             cmd = { CMD_UPLOAD, "" };
//             break;
//           default:
//             cmd = { CMD_NONE, "" };
//             break;
//         }
//         if (cmd.type != CMD_NONE) {
//           xQueueSend(commandQueue, &cmd, 0);
//           vTaskDelay(pdMS_TO_TICKS(1000)); 
//         }
//       }
//     }

//     else if (last == LOW && curr == HIGH) {
//       unsigned long pressDuration = millis() - pressStart;

  
//       if (currentState == CMD_UNLOCK && pressDuration < 2000) {
//         Command cmd = { CMD_START_RECORD, "" };
//         xQueueSend(commandQueue, &cmd, 0);
//         vTaskDelay(pdMS_TO_TICKS(1000));
//       }

//       countdownStarted = false;
//     }

//     last = curr;
//     vTaskDelay(pdMS_TO_TICKS(50));
//   }
// }






void OLEDTask(void* pvParameters) {
  CommandType lastState = CMD_NONE;
  for (;;) {
    if (lastState != currentState) {
      display.clearDisplay(); display.setCursor(0, 0);
      switch (currentState) {
        case CMD_UNLOCK:       display.println("Camera Unlocked"); break;
        case CMD_START_RECORD: display.println("Recording...");   break;
        case CMD_UPLOAD:       display.println("Uploading...");   break;
        case CMD_SETUP:        display.println("Setup Mode");     break;
        case CMD_LOGIN:        display.println("Logged In");display.println("Logged In");      break;
        case CMD_LOGOUT:       display.println("Logged Out"); showLogo();     break;
        default:               display.println("Idle");           break;
      }
      display.display();
      lastState = currentState;
    }
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void BuzzerTask(void* pvParameters) {
  CommandType lastState = CMD_NONE;
  for (;;) {
    if (lastState != currentState) {
      digitalWrite(BUZZER_PIN, HIGH); vTaskDelay(pdMS_TO_TICKS(100));
      digitalWrite(BUZZER_PIN, LOW);
      lastState = currentState;
    }
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void setLED(bool r, bool g, bool b) {
  digitalWrite(LED_RED_PIN,   r ? HIGH : LOW);
  digitalWrite(LED_GREEN_PIN, g ? HIGH : LOW);
  digitalWrite(LED_BLUE_PIN,  b ? HIGH : LOW);
}



void LEDTask(void* pvParameters) {
  CommandType lastState = CMD_NONE;
  bool blinkState = false;

  for (;;) {
    if (currentState != lastState && 
        (lastState == CMD_START_RECORD || lastState == CMD_UPLOAD)) {
      setLED(false, false, false);
    }
    if (currentState == CMD_START_RECORD) {//Red Blinking
      blinkState = !blinkState;
      setLED(blinkState, false, false);
    }
    else if (currentState == CMD_UPLOAD) {//BLUE blinking
      blinkState = !blinkState;
      setLED(false, false, blinkState);
    }
    else if (currentState != lastState && currentState != CMD_SETUP) {
      switch (currentState) {
        case CMD_LOGOUT: setLED(false, false, false); break;
        case CMD_LOGIN:  setLED(false, false,  true); break; //BLUE
        case CMD_UNLOCK: setLED(false, true,  false);  break; // GREEN
        default:         setLED(false, false, false); break;
      }
      blinkState = false;  // reset blinking
    }
    lastState = currentState;
    vTaskDelay(pdMS_TO_TICKS(500)); // Blinking LEDs
  }
}