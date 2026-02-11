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

// --- Testing Configuration ---
bool testing = false;
unsigned long test_delay = 100;
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
void test_red_blue();
String loadIndexHtml();
void handleStyle();
void handleScript();

void handleRoot() {
    String htmlContent = loadIndexHtml();
    server.send(200, "text/html", htmlContent);
}

void handleAction() {
    String action = server.arg("do");
    if (action == "red") Red();
    else if (action == "green") Green();
    else if (action == "blue") Blue();
    else if (action == "yellow") Yellow();
    else if (action == "cyan") Cyan();
    else if (action == "magenta") Magenta();
    else if (action == "white") White();
    else if (action == "off") Off();
    else if (action == "fade") Fade();
    else if (action == "strobeplus") Strobeplus();
    else if (action == "rgbstrobe") RGBStrobe();
    else if (action == "rainbow") Rainbow();
    else if (action == "halfstrobe") Halfstrobe();
    else if (action == "bgstrobe") BGStrobe();
    else if (action == "grstrobe") GRStrobe();
    else if (action == "next") Next();
    else if (action == "demo") Demo();
    else if (action == "previous") Previous();
    else if (action == "chinese_red") ChineseRed();
    else if (action == "chinese_green") ChineseGreen();
    else if (action == "chinese_blue") ChineseBlue();
    else if (action == "chinese_white") ChineseWhite();
    else if (action == "chinese_brt_up") ChineseBRTUp();
    else if (action == "chinese_brt_down") ChineseBRTDown();
    else if (action == "chinese_off") ChineseOFF();
    else if (action == "chinese_on") ChineseON();
    else if (action == "chinese_flash") ChineseFLASH();
    else if (action == "chinese_strobe") ChineseSTROBE();
    else if (action == "chinese_fade") ChineseFADE();
    else if (action == "chinese_smooth") ChineseSMOOTH();
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
    server.onNotFound(handleNotFound);
    server.begin();

    Serial.println("HTTP server started");
}

unsigned long lastStatusCheck = 0;

void loop() {
    if (testing) {
        test_red_blue();
        return;
    }
    dnsServer.processNextRequest();
    server.handleClient();
    
    // Print status every 30 seconds
    if (millis() - lastStatusCheck > 30000) {
        lastStatusCheck = millis();
        Serial.printf("AP Status - IP: %s, Clients: %d\n", 
                     WiFi.softAPIP().toString().c_str(), 
                     WiFi.softAPgetStationNum());
    }
    
    delay(10);
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

void test_red_blue() {
    ChineseRed();
    delay(test_delay);
    ChineseBlue();
    delay(test_delay);
    yield();
}