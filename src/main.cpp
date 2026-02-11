#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <IRremote.hpp>

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
DNSServer dnsServer;
WebServer server(80);

// --- Web Page ---

// --- Forward Declarations ---
void Clear();
void Red(); void Green(); void Blue(); void Yellow(); void Cyan(); void Magenta(); void White();
void Off(); void Fade(); void Strobeplus(); void RGBStrobe(); void Rainbow();
void Halfstrobe(); void BGStrobe(); void GRStrobe(); void Next(); void Demo(); void Previous();
void ChineseRed(); void ChineseGreen(); void ChineseBlue(); void ChineseWhite();
void ChineseBRTUp(); void ChineseBRTDown(); void ChineseOFF(); void ChineseON();
void ChineseFLASH(); void ChineseSTROBE(); void ChineseFADE(); void ChineseSMOOTH();
void extra_red_blue(); void extra_red_green(); void extra_red_white(); void extra_green_blue(); void extra_green_white(); void extra_blue_white();
void handlePattern();
String loadIndexHtml();
void handleStyle();
void handleScript();
void handleSetSpeed();

void handleRoot() {
    String htmlContent = loadIndexHtml();
    server.send(200, "text/html", htmlContent);
}

void handleAction() {
    String action = server.arg("do");
    if (action == "red") { currentPattern = 0; patternState = 0; Red(); }
    else if (action == "green") { currentPattern = 0; patternState = 0; Green(); }
    else if (action == "blue") { currentPattern = 0; patternState = 0; Blue(); }
    else if (action == "yellow") { currentPattern = 0; patternState = 0; Yellow(); }
    else if (action == "cyan") { currentPattern = 0; patternState = 0; Cyan(); }
    else if (action == "magenta") { currentPattern = 0; patternState = 0; Magenta(); }
    else if (action == "white") { currentPattern = 0; patternState = 0; White(); }
    else if (action == "off") { currentPattern = 0; patternState = 0; Off(); }
    else if (action == "fade") { currentPattern = 0; patternState = 0; Fade(); }
    else if (action == "strobeplus") { currentPattern = 0; patternState = 0; Strobeplus(); }
    else if (action == "rgbstrobe") { currentPattern = 0; patternState = 0; RGBStrobe(); }
    else if (action == "rainbow") { currentPattern = 0; patternState = 0; Rainbow(); }
    else if (action == "halfstrobe") { currentPattern = 0; patternState = 0; Halfstrobe(); }
    else if (action == "bgstrobe") { currentPattern = 0; patternState = 0; BGStrobe(); }
    else if (action == "grstrobe") { currentPattern = 0; patternState = 0; GRStrobe(); }
    else if (action == "next") { currentPattern = 0; patternState = 0; Next(); }
    else if (action == "demo") { currentPattern = 0; patternState = 0; Demo(); }
    else if (action == "previous") { currentPattern = 0; patternState = 0; Previous(); }
    else if (action == "chinese_red") { currentPattern = 0; patternState = 0; ChineseRed(); }
    else if (action == "chinese_green") { currentPattern = 0; patternState = 0; ChineseGreen(); }
    else if (action == "chinese_blue") { currentPattern = 0; patternState = 0; ChineseBlue(); }
    else if (action == "chinese_white") { currentPattern = 0; patternState = 0; ChineseWhite(); }
    else if (action == "chinese_brt_up") { currentPattern = 0; patternState = 0; ChineseBRTUp(); }
    else if (action == "chinese_brt_down") { currentPattern = 0; patternState = 0; ChineseBRTDown(); }
    else if (action == "chinese_off") { currentPattern = 0; patternState = 0; ChineseOFF(); }
    else if (action == "chinese_on") { currentPattern = 0; patternState = 0; ChineseON(); }
    else if (action == "chinese_flash") { currentPattern = 0; patternState = 0; ChineseFLASH(); }
    else if (action == "chinese_strobe") { currentPattern = 0; patternState = 0; ChineseSTROBE(); }
    else if (action == "chinese_fade") { currentPattern = 0; patternState = 0; ChineseFADE(); }
    else if (action == "chinese_smooth") { currentPattern = 0; patternState = 0; ChineseSMOOTH(); }
    else if (action == "extra_red_blue") extra_red_blue();
    else if (action == "extra_red_green") extra_red_green();
    else if (action == "extra_red_white") extra_red_white();
    else if (action == "extra_green_blue") extra_green_blue();
    else if (action == "extra_green_white") extra_green_white();
    else if (action == "extra_blue_white") extra_blue_white();
    else {
        server.send(400, "text/plain", "Invalid action");
        return;
    }
    server.send(200, "text/plain", "OK");
}

void handleNotFound() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}

void handleStyle() {
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS");
    server.send(500, "text/plain", "Filesystem error");
    return;
  }

  File file = LittleFS.open("/style.css", "r");
  if (!file) {
    Serial.println("Failed to open style.css");
    server.send(404, "text/plain", "File not found");
    return;
  }

  server.streamFile(file, "text/css");
  file.close();
}

void handleScript() {
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS");
    server.send(500, "text/plain", "Filesystem error");
    return;
  }

  File file = LittleFS.open("/script.js", "r");
  if (!file) {
    Serial.println("Failed to open script.js");
    server.send(404, "text/plain", "File not found");
    return;
  }

  server.streamFile(file, "application/javascript");
  file.close();
}

void handleSetSpeed() {
    String speedStr = server.arg("speed");
    if (speedStr.length() > 0) {
        unsigned long newDelay = speedStr.toInt();
        if (newDelay >= 100 && newDelay <= 5000) {
            color_pair_delay = newDelay;
            Serial.printf("Speed set to %lums\\n", color_pair_delay);
            server.send(200, "text/plain", "OK");
        } else {
            server.send(400, "text/plain", "Speed must be between 100 and 5000ms");
        }
    } else {
        server.send(400, "text/plain", "Missing speed parameter");
    }
}

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
    WiFi.softAP("K8_RGB_IR_REMOTE", NULL, 1, 0, 4);
    WiFi.setTxPower(WIFI_POWER_8_5dBm);    
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    // Setup DNS Server for Captive Portal
    dnsServer.start(53, "*", WiFi.softAPIP());

    // Setup Web Server
    server.on("/", HTTP_GET, handleRoot);
    server.on("/action", HTTP_GET, handleAction);
    server.on("/style.css", HTTP_GET, handleStyle);
    server.on("/script.js", HTTP_GET, handleScript);
    server.on("/set_speed", HTTP_GET, handleSetSpeed);
    server.onNotFound(handleNotFound);
    server.begin();

    Serial.println("HTTP server started");
}

unsigned long lastStatusCheck = 0;

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
    handlePattern();
    
    // Print status every 30 seconds
    if (millis() - lastStatusCheck > 30000) {
        lastStatusCheck = millis();
        Serial.printf("AP Status - IP: %s, Clients: %d\n", 
                     WiFi.softAPIP().toString().c_str(), 
                     WiFi.softAPgetStationNum());
    }
    
    delay(10);
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

String loadIndexHtml() {
  if (!LittleFS.begin()) {
    Serial.println("An error occurred while mounting LittleFS");
    return "Error loading page";
  }

  File file = LittleFS.open("/index.html", "r");
  if (!file) {
    Serial.println("Failed to open index.html");
    return "Error loading page";
  }

  String content = file.readString();
  file.close();
  return content;
}