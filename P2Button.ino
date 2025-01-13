#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

// OLED display setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// P2CAM Logo
#define LOGO_WIDTH 128
#define LOGO_HEIGHT 55

static const unsigned char PROGMEM epd_bitmap_P2CAMimage[] = {// P2CAM logo image
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



// Pin definitions
const int buttonPin = 21;
const int ledBluePin = 9;
const int ledGreenPin = 10;
const int ledRedPin = 20;
const int buzzerPin = 5;
const int unpairButtonPin = 3; // Button to unpair
const int batteryPin = 34;

// BLE setup
BLEServer *pServer = NULL;

// Define BLE properties
struct BLEProperties {
    bool canRead;
    bool canWrite;
    bool canIndicate;
    bool canNotify;
};

// Define BLE command structure
struct BLECommand {
    const char *serviceUUID;       // UUID of the service
    const char *charUUID;          // UUID of the characteristic
    BLEProperties properties;      // Properties (Read, Write, Indicate, Notify)
    String commandString;          // String describing the command
    BLECharacteristic *characteristic; // Pointer to the BLE characteristic object
};

// Define individual characteristics
BLECommand unlockButtonChar = {"19b10000-e8f2-537e-4f6c-d104768a1214", "19b10001-e8f2-537e-4f6c-d104768a1214", {false, false, true, false}, "Unlock (Button)", NULL};
BLECommand unlockChar = {"19b10000-e8f2-537e-4f6c-d104768a1214", "19b10002-e8f2-537e-4f6c-d104768a1214", {false, true, false, false}, "Unlock", NULL};
BLECommand recordButtonChar = {"19b10000-e8f2-537e-4f6c-d104768a1214", "19b10003-e8f2-537e-4f6c-d104768a1214", {false, false, true, false}, "Record (Button)", NULL};
BLECommand recordChar = {"19b10000-e8f2-537e-4f6c-d104768a1214", "19b10004-e8f2-537e-4f6c-d104768a1214", {false, true, false, false}, "Record", NULL};
BLECommand uploadButtonChar = {"19b10000-e8f2-537e-4f6c-d104768a1214", "19b10005-e8f2-537e-4f6c-d104768a1214", {false, false, true, false}, "Upload (Button)", NULL};
BLECommand uploadChar = {"19b20000-e8f2-537e-4f6c-d104768a1214", "19b20001-e8f2-537e-4f6c-d104768a1214", {false, true, false, false}, "Upload", NULL};
BLECommand setupChar = {"19b20000-e8f2-537e-4f6c-d104768a1214", "19b20002-e8f2-537e-4f6c-d104768a1214", {false, true, false, false}, "Setup", NULL};
BLECommand loginChar = {"19b20000-e8f2-537e-4f6c-d104768a1214", "19b20003-e8f2-537e-4f6c-d104768a1214", {false, true, false, false}, "Login", NULL};
BLECommand logoutChar = {"19b20000-e8f2-537e-4f6c-d104768a1214", "19b20004-e8f2-537e-4f6c-d104768a1214", {false, true, false, false}, "Logout", NULL};
BLECommand sleepChar = {"19b20000-e8f2-537e-4f6c-d104768a1214", "19b20005-e8f2-537e-4f6c-d104768a1214", {false, true, false, false}, "Sleep", NULL};

// Change from BLECommand to BLECommand*
BLECommand* commands[] = {
    &unlockButtonChar, &unlockChar, &recordButtonChar, &recordChar, &uploadButtonChar,
    &uploadChar, &setupChar, &loginChar, &logoutChar, &sleepChar
};



// State definitions
enum DeviceState {
    NOT_PAIRED,
    PAIRED_SETUP,
    LOGGED_LOCKED,
    CAMERA_UNLOCKED,
    RECORDING,
    UPLOADING,
    SLEEP_MODE
};

DeviceState state = NOT_PAIRED; // Default state

// Function prototypes
void setupBLE();
void createCharacteristic(BLECommand *command, BLEService *service);
void handleWriteCommand(String command);
void updateState(DeviceState newState);
void sendIndication(BLECommand *command);
void displayCountdown(int countdown);
void displayCenteredMessage(const char* message, int textSize);
void soundBuzzer(int duration, int repeat);
void setLEDs(bool blue, bool green, bool red);
void ledTextAnimator(const char* text, int ledPin, unsigned long blinkInterval);
void displayMessage(const char *line1, const char *line2, int size1, int size2);



// Custom server callback to handle pairing events
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) override {
        Serial.println("Client connected!");
       
        delay(800);
    }

    void onDisconnect(BLEServer *pServer) override {
        Serial.println("Client disconnected!");
        updateState(NOT_PAIRED); // Transition to NOT_PAIRED state
        BLEDevice::startAdvertising(); // Restart advertising
        Serial.println("Advertising restarted.");
    }
};



void setupBLE() {
    Serial.println("Initializing BLE...");
    BLEDevice::init("P2CAM");

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Service 1: UUID 19b10000-e8f2-537e-4f6c-d104768a1214
    Serial.println("Creating Service 1: UUID = 19b10000-e8f2-537e-4f6c-d104768a1214");
    BLEService *service1 = pServer->createService("19b10000-e8f2-537e-4f6c-d104768a1214");
    if (service1 == nullptr) {
        Serial.println("Error: Failed to create Service 1.");
        return;
    }

    // Create characteristics for Service 1
    for (int i = 0; i < 5; i++) {
        Serial.println("Creating characteristic for Service 1: UUID = " + String(commands[i]->charUUID));
        createCharacteristic(commands[i], service1);
        if (commands[i]->characteristic == nullptr) {
            Serial.println("Error: Failed to create characteristic with UUID = " + String(commands[i]->charUUID));
        }
    }


    // Service 2: UUID 19b20000-e8f2-537e-4f6c-d104768a1214
    Serial.println("Creating Service 2: UUID = 19b20000-e8f2-537e-4f6c-d104768a1214");
    BLEService *service2 = pServer->createService("19b20000-e8f2-537e-4f6c-d104768a1214");
    if (service2 == nullptr) {
        Serial.println("Error: Failed to create Service 2.");
        return;
    }

    // Create characteristics for Service 2
    for (int i = 5; i < 10; i++) {
        Serial.println("Creating characteristic for Service 2: UUID = " + String(commands[i]->charUUID));
        createCharacteristic(commands[i], service2);
        if (commands[i]->characteristic == nullptr) {
            Serial.println("Error: Failed to create characteristic with UUID = " + String(commands[i]->charUUID));
        }
    }



    // Start Service 2
    service2->start();
    Serial.println("Service 2 started successfully.");

    // Start Service 1
    service1->start();
    Serial.println("Service 1 started successfully.");

    


    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID("19b10000-e8f2-537e-4f6c-d104768a1214"); // Advertise Service 1
    pAdvertising->addServiceUUID("19b20000-e8f2-537e-4f6c-d104768a1214"); // Advertise Service 2
    pAdvertising->setScanResponse(true);

    BLEDevice::startAdvertising();
    Serial.println("BLE advertising started.");
}


// Custom write callback for handling app commands
class WriteCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override {
        String command = pCharacteristic->getValue().c_str(); // Read command string
        Serial.println("Command received: " + command);
        handleWriteCommand(command);
    }
};


void createCharacteristic(BLECommand *command, BLEService *service) {
    uint32_t properties = 0;

    // Determine properties and log them
    Serial.println("Creating characteristic with UUID: " + String(command->charUUID));
    if (command->properties.canRead) {
        properties |= BLECharacteristic::PROPERTY_READ;
        Serial.println(" - Property: Read");
    }
    if (command->properties.canWrite) {
        properties |= BLECharacteristic::PROPERTY_WRITE;
        Serial.println(" - Property: Write");
    }
    if (command->properties.canIndicate) {
        properties |= BLECharacteristic::PROPERTY_INDICATE;
        Serial.println(" - Property: Indicate");
    }
    if (command->properties.canNotify) {
        properties |= BLECharacteristic::PROPERTY_NOTIFY;
        Serial.println(" - Property: Notify");
    }

    // Attempt to create the characteristic
    command->characteristic = service->createCharacteristic(command->charUUID, properties);

    // Check if the characteristic was created successfully
    if (command->characteristic == nullptr) {
        Serial.println("Error: Failed to create characteristic for UUID = " + String(command->charUUID));
        return;
    } else {
        Serial.println("Successfully created characteristic: UUID = " + String(command->charUUID));
    }

    // Add descriptor for indications/notifications and log
    if (command->properties.canIndicate || command->properties.canNotify) {
        command->characteristic->addDescriptor(new BLE2902());
        Serial.println(" - Descriptor added for Indicate/Notify");
    }

    // Attach write callback if writable
    if (command->properties.canWrite) {
        command->characteristic->setCallbacks(new WriteCallback());
        Serial.println(" - Write callback attached");
    }

    // Log the characteristic creation details
    Serial.println("Characteristic created successfully: UUID = " + String(command->charUUID));
}





void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE...");
    pinMode(ledBluePin, OUTPUT);
    pinMode(ledGreenPin, OUTPUT);
    pinMode(ledRedPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(unpairButtonPin, INPUT_PULLUP); // Initialize unpair button as input with internal pullup
    pinMode(batteryPin, INPUT);

    //Display setup
    Wire.begin(6, 7);// SDA = GPIO 6, SCL = GPIO 7
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for most I2C OLED displays
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(""); // shows that device not connected yet
    display.display();
  

    setLEDs(false, false, false);
    setupBLE();
}

void loop() {
    static unsigned long pressStartTime = 0;
    static bool buttonWasPressed = false;
    static bool countdownStarted = false;
    static int countdown = 2; // Countdown starts from 2 seconds
    int buttonStateNow = digitalRead(buttonPin);

    if (state == NOT_PAIRED) {
        showP2CAMLogo(); 
        delay(500); // Prevent display flickering
        return; // Exit loop to avoid unnecessary checks
    }

    if (buttonStateNow == LOW) { // Button is pressed
        if (!buttonWasPressed) {
            pressStartTime = millis(); // Record the time the button was first pressed
            buttonWasPressed = true;
            countdownStarted = false;
            Serial.println("Button pressed");
        }
        // Calculate how long the button has been pressed
        unsigned long elapsedTime = millis() - pressStartTime;

        // Start the countdown if the button is held for more than 500ms
       if (state == LOGGED_LOCKED || state == RECORDING) {
        
        if (elapsedTime >= 500 && !countdownStarted) {
              countdownStarted = true; // Countdown begins
              countdown = 2; // Reset countdown to 2 seconds
            }
        if (elapsedTime < 500 ){displayCenteredMessage("Hold button",1);}

        }

        // Handle the countdown
        if (countdownStarted) {
            int secondsElapsed = (elapsedTime - 200) / 1000; // Account for 500ms delay
            int currentCountdown = countdown - secondsElapsed;

            if (currentCountdown > 0) {
                displayCountdown(currentCountdown); // Display countdown
            } else {
                // Display message to release the button
                displayMessage("Release", "Button", 2, 2);
            }
        }
    } else if (buttonWasPressed) { // Button is released
        countdownStarted = false;
        buttonWasPressed = false;
        unsigned long pressDuration = millis() - pressStartTime;
        bool isLongPress = (pressDuration >= 2000);

        if (state == LOGGED_LOCKED && isLongPress) {
            Serial.println("Transitioning to CAMERA_UNLOCKED");
            updateState(CAMERA_UNLOCKED);
            sendIndication(&unlockButtonChar);
        } else if (state == CAMERA_UNLOCKED && !isLongPress) {
            Serial.println("Transitioning to RECORDING");
            updateState(RECORDING);
            sendIndication(&recordButtonChar);
        } else if (state == RECORDING && isLongPress) {
            Serial.println("Transitioning to UPLOADING");
            updateState(UPLOADING);
            sendIndication(&uploadButtonChar);
        }
    }

    if (state == RECORDING && buttonStateNow == HIGH ) ledTextAnimator("RECORDING" ,ledRedPin , 300);
    if (state == UPLOADING) ledTextAnimator("UPLOADING" ,ledBluePin, 400);
}



// ********************Communictions***************************



void handleWriteCommand(String command) {
    if (command.startsWith("Login")) updateState(LOGGED_LOCKED);
    else if (command.equals("Unlock")) updateState(CAMERA_UNLOCKED);
    else if (command.equals("Record")) updateState(RECORDING);
    else if (command.equals("Upload")) updateState(UPLOADING);
    else if (command.equals("Logout")) updateState(NOT_PAIRED);
    else if (command.equals("Sleep")) updateState(SLEEP_MODE);
    else Serial.println("Invalid command: " + command);
}


void updateState(DeviceState newState) {
    setLEDs(false, false, false);
    state = newState;
    Serial.println("State updated to: " + String(state));
    switch (state) {
        case SLEEP_MODE: display.clearDisplay(); display.display(); setLEDs(false, false, false); break;
        case NOT_PAIRED:   setLEDs(false, false, false); break;
        case PAIRED_SETUP: setLEDs(true, true, true); delay(20); soundBuzzer(200,1); delay(200); soundBuzzer(200,2) ; delay(20); setLEDs(false, false, false); break;
        case LOGGED_LOCKED: 
        soundBuzzer(200,2);
        displayCenteredMessage("LOGGED IN" , 2);
        delay(1000);
        setLEDs(true, false, false); 
        displayMessage("Press and Hold","for 2 Seconds",1,1);
          break;
        case CAMERA_UNLOCKED: soundBuzzer(400,1); displayCenteredMessage("READY" , 2); delay(20); setLEDs(false, true, false); delay(2000); displayCenteredMessage("Press Button",1); break;
        case RECORDING:  break;
        case UPLOADING:  break;
    }
}



void sendIndication(BLECommand *command) {
    if (command->characteristic == nullptr) {
        Serial.println("Error: Characteristic is null, cannot send indication for UUID = " + String(command->charUUID));
        return;
    }
    

    command->characteristic->setValue(command->commandString.c_str());
    if (command->properties.canIndicate) {
        command->characteristic->indicate();
        Serial.println("Indication sent for: " + command->commandString);
    } else {
        Serial.println("Error: Characteristic does not support indication: " + command->commandString);
    }
}


// ********************TOOLS*****************************

void displayCountdown(int countdown) {
    display.clearDisplay();
    display.setTextSize(3); // Set text size large
    display.setTextColor(SSD1306_WHITE);

    // Convert countdown value to a string
    String text = String(countdown);
    int textWidth = text.length() * 18;  // 18 pixels per character for size 3
    int x = (SCREEN_WIDTH - textWidth) / 2; // Center the text horizontally
    int y = (SCREEN_HEIGHT - 24) / 2;  // Center the text vertically

    // Display the countdown value
    display.setCursor(x, y);
    display.print(text);
    display.display();
}

void displayMessage(const char* line1, const char* line2, int size1, int size2) {
  display.clearDisplay();
  display.setTextSize(size1);
  display.setCursor(0, 0);
  display.println(line1);
  display.setTextSize(size2);
  display.setCursor(0, 16 * size1);
  display.println(line2);
  display.display();
}
void showP2CAMLogo() {
    display.clearDisplay();
    display.drawBitmap((SCREEN_WIDTH - LOGO_WIDTH) / 2, (SCREEN_HEIGHT - LOGO_HEIGHT) / 2,
                       epd_bitmap_P2CAMimage, LOGO_WIDTH, LOGO_HEIGHT, SSD1306_WHITE);
    display.display();
}

void setLEDs(bool blue, bool green, bool red) {
    digitalWrite(ledBluePin, blue ? HIGH : LOW);
    digitalWrite(ledGreenPin, green ? HIGH : LOW);
    digitalWrite(ledRedPin, red ? HIGH : LOW);
}
void ledTextAnimator(const char* text, int ledPin, unsigned long blinkInterval) {
    static unsigned long previousTime = 0; // Tracks the last time the LED toggled
    static bool ledState = false;          // Tracks the current state of the LED
    static int animationStep = 0;          // Tracks the animation step for the text

    unsigned long currentTime = millis();

    // Check if it's time to toggle the LED
    if (currentTime - previousTime >= blinkInterval) {
        previousTime = currentTime;
        ledState = !ledState; // Toggle the LED state

        // Set the LED state
        digitalWrite(ledPin, ledState ? HIGH : LOW);

        // Update OLED display
        display.clearDisplay();
        display.setTextSize(1); // Set text size
        
        // Generate animated text with dots
        String animatedText = String(text);
        for (int i = 0; i <= animationStep; i++) {
            animatedText += ".";
        }

        // Calculate position for centering the text
        int charWidth = 6; // Approximate character width for text size 1
        int charHeight = 8; // Approximate character height for text size 1
        int textLength = animatedText.length();
        int xPos = (SCREEN_WIDTH - (textLength * charWidth)) / 2; // Center horizontally
        int yPos = (SCREEN_HEIGHT - charHeight) / 2;             // Center vertically

        // Set the cursor to the calculated position
        display.setCursor(xPos, yPos);

        // Print the animated text to the OLED
        display.print(animatedText);
        display.display();

        // Increment animation step and loop back to 0 after 3 steps
        animationStep = (animationStep + 1) % 4;
    }
}


void displayCenteredMessage(const char* message, int textSize) {
    display.clearDisplay();
    // Set text size
    display.setTextSize(textSize);
    // Calculate position for centering the text
    int charWidth = 6 * textSize; // Approximate character width in pixels
    int charHeight = 8 * textSize; // Approximate character height in pixels
    int textLength = strlen(message); // Get the number of characters in the message
    int xPos = (SCREEN_WIDTH - (textLength * charWidth)) / 2; // Center horizontally
    int yPos = (SCREEN_HEIGHT - charHeight) / 2;             // Center vertically

    // Set the cursor to the calculated position
    display.setCursor(xPos, yPos);

    // Print the message to the OLED
    display.println(message);
    display.display();
}


void soundBuzzer(int duration, int repeat) {
  for (int i = 0; i < repeat; i++) {
    digitalWrite(buzzerPin, HIGH); // Turn on the buzzer
    delay(duration);               // Buzzer on for the specified duration
    digitalWrite(buzzerPin, LOW);  // Turn off the buzzer
    if (i < repeat - 1) {
      delay(duration); // Delay between beeps
    }
  }
}
