#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <IRremote.hpp>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "tasks.h"

// ESPAsyncWebServer and ElegantOTA are included in tasks.h
// AsyncTCP is required for ESPAsyncWebServer

// --- Pin Definitions ---
const uint16_t kIrLedPin = 4;
const uint16_t rPin = 0;
const uint16_t gPin = 1;
const uint16_t bPin = 2;


unsigned long color_pair_delay = 500; // ms delay between color commands

// Pattern control variables
int currentPattern = 0; // 0=off, 1=red_blue, 2=red_green, 3=red_white, 4=green_blue, 5=green_white, 6=blue_white
int patternState = 0; // 0=first color, 1=second color
unsigned long lastPatternTime = 0;


// --- WiFi Event Handler ---
void WiFiEvent(WiFiEvent_t event) {
    switch(event) {
        case SYSTEM_EVENT_AP_START:
            Serial.println("AP Started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            Serial.println("AP Stopped");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            Serial.println("Client Connected");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            Serial.println("Client Disconnected");
            break;
        default:
            break;
    }
}

// --- Objects ---
// Global variables that need to be shared between files
DNSServer dnsServer; // Still needed for captive portal
AsyncWebServer server(80); // Changed from WebServer to AsyncWebServer for ElegantOTA
bool otaInProgress = false;
bool captivePortalActive = true; // We're using AP mode only for now

// FreeRTOS task handle
TaskHandle_t elegantOTATaskHandle = NULL;

// --- Forward Declarations ---
// Color and command functions (same as before, these are defined later in this file)
void Clear();
void Red(); void Green(); void Blue(); void Yellow(); void Cyan(); void Magenta(); void White();
void Off(); void Fade(); void Strobeplus(); void RGBStrobe(); void Rainbow();
void Halfstrobe(); void BGStrobe(); void GRStrobe(); void Next(); void Demo(); void Previous();
void ChineseRed(); void ChineseGreen(); void ChineseBlue(); void ChineseWhite();
void ChineseBRTUp(); void ChineseBRTDown(); void ChineseOFF(); void ChineseON();
void ChineseFLASH(); void ChineseSTROBE(); void ChineseFADE(); void ChineseSMOOTH();
void extra_red_blue(); void extra_red_green(); void extra_red_white(); void extra_green_blue(); void extra_green_white(); void extra_blue_white();

// Pattern handler (still needed)
void handlePattern();

// LittleFS helpers (defined in tasks.cpp)
bool initLittleFS();
String readFile(const char* path);
bool writeFile(const char* path, const String& content);


void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    // Setup digital pins for RGB LED
    pinMode(rPin, OUTPUT);
    pinMode(gPin, OUTPUT);
    pinMode(bPin, OUTPUT);
    Clear();

    // Setup IR Sender
    IrSender.begin(kIrLedPin);

    // Improved WiFi AP Setup
    WiFi.onEvent(WiFiEvent);
    WiFi.mode(WIFI_AP);
    WiFi.softAP("K8_RGB_IR_REMOTE", "SmartOne", 1, 0, 4);
    WiFi.setTxPower(WIFI_POWER_8_5dBm);    
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    // Setup DNS Server for Captive Portal
    dnsServer.start(53, "*", WiFi.softAPIP());

    // Initialize LittleFS (for serving our web page)
    if (!initLittleFS()) {
        Serial.println("Failed to initialize LittleFS, but continuing...");
    }

    // Create ElegantOTA task (handles web server and OTA updates)
    xTaskCreatePinnedToCore(
        elegantOTATask,      // Task function
        "ElegantOTA Task",   // Name
        8192,                // Stack size
        NULL,                // Parameters
        1,                   // Priority
        &elegantOTATaskHandle, // Task handle
        1                    // Core (1 = APP_CPU)
    );

    Serial.println("EasyOTA task started");
}

unsigned long lastStatusCheck = 0;
void loop() {
    // The web server and DNS are handled by the ElegantOTA task
    // We just need to handle our pattern logic and status updates

    handlePattern();

    // Print status every 30 seconds
    if (millis() - lastStatusCheck > 30000) {
        lastStatusCheck = millis();
        Serial.printf("AP Status - IP: %s, Clients: %d\n", 
                     WiFi.softAPIP().toString().c_str(), 
                     WiFi.softAPgetStationNum());
    }

    delay(10);  // Small delay to prevent watchdog issues
}


void handlePattern() {
    if (currentPattern == 0) return;

    unsigned long now = millis();
    if (now - lastPatternTime >= color_pair_delay) {
        lastPatternTime = now;

        switch (currentPattern) {
            case 1: // red_blue
                if (patternState == 0) {
                    ChineseRed();
                    patternState = 1;
                } else {
                    ChineseBlue();
                    patternState = 0;
                }
                break;
            case 2: // red_green
                if (patternState == 0) {
                    ChineseRed();
                    patternState = 1;
                } else {
                    ChineseGreen();
                    patternState = 0;
                }
                break;
            case 3: // red_white
                if (patternState == 0) {
                    ChineseRed();
                    patternState = 1;
                } else {
                    ChineseWhite();
                    patternState = 0;
                }
                break;
            case 4: // green_blue
                if (patternState == 0) {
                    ChineseGreen();
                    patternState = 1;
                } else {
                    ChineseBlue();
                    patternState = 0;
                }
                break;
            case 5: // green_white
                if (patternState == 0) {
                    ChineseGreen();
                    patternState = 1;
                } else {
                    ChineseWhite();
                    patternState = 0;
                }
                break;
            case 6: // blue_white
                if (patternState == 0) {
                    ChineseBlue();
                    patternState = 1;
                } else {
                    ChineseWhite();
                    patternState = 0;
                }
                break;
            default:
                currentPattern = 0;
                patternState = 0;
                break;
        }
    }
}

// --- Color and Command Functions ---
void Clear() { 
    // Serial.println("Clear called");
    digitalWrite(rPin, LOW);
    digitalWrite(gPin, LOW);
    digitalWrite(bPin, LOW);
}
void Red() { 
    Serial.println("Red called");
    Clear(); 
    digitalWrite(rPin, HIGH);
    IrSender.sendNECMSB(0xFF10EF, 32, false);
}
void Green() { 
    Serial.println("Green called");
    Clear(); 
    digitalWrite(gPin, HIGH);
    IrSender.sendNECMSB(0xFF906F, 32, false);
}
void Blue() { 
    Serial.println("Blue called");
    Clear(); 
    digitalWrite(bPin, HIGH);
    IrSender.sendNECMSB(0xFF50AF, 32, false);
}
void Yellow() { 
    Serial.println("Yellow called");
    Clear(); 
    digitalWrite(rPin, HIGH);
    digitalWrite(gPin, HIGH);
    IrSender.sendNECMSB(0xFFD02F, 32, false);
}
void Cyan() { 
    Serial.println("Cyan called");
    Clear(); 
    digitalWrite(gPin, HIGH);
    digitalWrite(bPin, HIGH);
    IrSender.sendNECMSB(0xFFB04F, 32, false);
}
void Magenta() { 
    Serial.println("Magenta called");
    Clear(); 
    digitalWrite(rPin, HIGH);
    digitalWrite(bPin, HIGH);
    IrSender.sendNECMSB(0xFF30CF, 32, false);
}
void White() { 
    Serial.println("White called");
    digitalWrite(rPin, HIGH);
    digitalWrite(gPin, HIGH);
    digitalWrite(bPin, HIGH);
    IrSender.sendNECMSB(0xFF708F, 32, false);
}
void Off() { 
    Serial.println("Off called");
    Clear(); 
    IrSender.sendNECMSB(0xFFE01F, 32, false);
}
void Fade() { 
    Serial.println("Fade called");
    IrSender.sendNECMSB(0xFFF00F, 32, false);
}
void Strobeplus() { 
    Serial.println("Strobeplus called");
    IrSender.sendNECMSB(0xFFA857, 32, false);
}
void RGBStrobe() { 
    Serial.println("RGBStrobe called");
    IrSender.sendNECMSB(0xFF28D7, 32, false);
}
void Rainbow() { 
    Serial.println("Rainbow called");
    IrSender.sendNECMSB(0xFF6897, 32, false);
}
void Halfstrobe() { 
    Serial.println("Halfstrobe called");
    IrSender.sendNECMSB(0xFFE817, 32, false);
}
void BGStrobe() { 
    Serial.println("BGStrobe called");
    IrSender.sendNECMSB(0xFF9867, 32, false);
}
void GRStrobe() { 
    Serial.println("GRStrobe called");
    IrSender.sendNECMSB(0xFF18E7, 32, false);
}
void Next() { 
    Serial.println("Next called");
    IrSender.sendNECMSB(0xFF20DF, 32, false);
}
void Demo() { 
    Serial.println("Demo called");
    IrSender.sendNECMSB(0xFF58A7, 32, false);
}
void Previous() { 
    Serial.println("Previous called");
    IrSender.sendNECMSB(0xFFA05F, 32, false);
}

void ChineseRed() { 
    Serial.println("ChineseRed called");
    Clear(); 
    digitalWrite(rPin, HIGH);
    IrSender.sendNECMSB(0x00F720DF, 32, false);
}
void ChineseGreen() { 
    Serial.println("ChineseGreen called");
    Clear(); 
    digitalWrite(gPin, HIGH);
    IrSender.sendNECMSB(0x00F7A05F, 32, false);
}
void ChineseBlue() { 
    Serial.println("ChineseBlue called");
    Clear(); 
    digitalWrite(bPin, HIGH);
    IrSender.sendNECMSB(0x00F7609F, 32, false);
}
void ChineseWhite() { 
    Serial.println("ChineseWhite called");
    Clear(); 
    digitalWrite(rPin, HIGH);
    digitalWrite(gPin, HIGH);
    digitalWrite(bPin, HIGH);
    IrSender.sendNECMSB(0x00F7E01F, 32, false);
}
void ChineseBRTUp() { 
    Serial.println("ChineseBRTUp called");
    IrSender.sendNECMSB(0x00F700FF, 32, false);
}
void ChineseBRTDown() { 
    Serial.println("ChineseBRTDown called");
    IrSender.sendNECMSB(0x00F7807F, 32, false);
}
void ChineseOFF() { 
    Serial.println("ChineseOFF called");
    IrSender.sendNECMSB(0x00F740BF, 32, false);
}
void ChineseON() { 
    Serial.println("ChineseON called");
    IrSender.sendNECMSB(0x00F7C03F, 32, false);
}
void ChineseFLASH() { 
    Serial.println("ChineseFLASH called");
    IrSender.sendNECMSB(0x00F7D02F, 32, false);
}
void ChineseSTROBE() { 
    Serial.println("ChineseSTROBE called");
    IrSender.sendNECMSB(0x00F7F00F, 32, false);
}
void ChineseFADE() { 
    Serial.println("ChineseFADE called");
    IrSender.sendNECMSB(0x00F7C837, 32, false);
}
void ChineseSMOOTH() { 
    Serial.println("ChineseSMOOTH called");
    IrSender.sendNECMSB(0x00F7E817, 32, false);
}

void extra_red_blue() {
    Serial.println("extra_red_blue called");
    currentPattern = 1;
    patternState = 0;
    lastPatternTime = millis();
    ChineseRed();
}
void extra_red_green() {
    Serial.println("extra_red_green called");
    currentPattern = 2;
    patternState = 0;
    lastPatternTime = millis();
    ChineseRed();
}
void extra_red_white() {
    Serial.println("extra_red_white called");
    currentPattern = 3;
    patternState = 0;
    lastPatternTime = millis();
    ChineseRed();
}
void extra_green_blue() {
    Serial.println("extra_green_blue called");
    currentPattern = 4;
    patternState = 0;
    lastPatternTime = millis();
    ChineseGreen();
}
void extra_green_white() {
    Serial.println("extra_green_white called");
    currentPattern = 5;
    patternState = 0;
    lastPatternTime = millis();
    ChineseGreen();
}
void extra_blue_white() {
    Serial.println("extra_blue_white called");
    currentPattern = 6;
    patternState = 0;
    lastPatternTime = millis();
    ChineseBlue();
}
