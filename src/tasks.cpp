#include <Arduino.h>
#include "tasks.h"
#include <WiFi.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <DNSServer.h>
#include <ArduinoJson.h>

// Global variables (defined in main.cpp)
extern AsyncWebServer server;
extern DNSServer dnsServer;
extern bool otaInProgress;
extern bool captivePortalActive;

// Our pattern control variables (from main.cpp)
extern unsigned long color_pair_delay;
extern int currentPattern;
extern int patternState;
extern unsigned long lastPatternTime;

// Pin definitions (from main.cpp)
extern const uint16_t kIrLedPin;
extern const uint16_t rPin;
extern const uint16_t gPin;
extern const uint16_t bPin;

// DNS server IP (captive portal) - using AP default
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMask(255, 255, 255, 0);

// ============================================================================
// LittleFS Helpers
// ============================================================================

String getContentType(String filename) {
  if (filename.endsWith(".htm")) return "text/html";
  if (filename.endsWith(".html")) return "text/html";
  if (filename.endsWith(".css")) return "text/css";
  if (filename.endsWith(".js")) return "application/javascript";
  if (filename.endsWith(".png")) return "image/png";
  if (filename.endsWith(".gif")) return "image/gif";
  if (filename.endsWith(".jpg")) return "image/jpeg";
  if (filename.endsWith(".ico")) return "image/x-icon";
  if (filename.endsWith(".xml")) return "text/xml";
  if (filename.endsWith(".pdf")) return "application/x-pdf";
  if (filename.endsWith(".zip")) return "application/x-zip";
  if (filename.endsWith(".gz")) return "application/x-gzip";
  if (filename.endsWith(".bin")) return "application/octet-stream";
  return "text/plain";
}

bool initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS Mount Failed");
    return false;
  }
  Serial.println("LittleFS mounted successfully");
  return true;
}

String readFile(const char* path) {
  Serial.printf("Reading file: %s\n", path);
  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return String();
  }
  String content;
  while (file.available()) {
    content += (char)file.read();
  }
  file.close();
  return content;
}

bool writeFile(const char* path, const String& content) {
  Serial.printf("Writing file: %s\n", path);
  File file = LittleFS.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return false;
  }
  if (file.print(content)) {
    file.close();
    Serial.println("File written successfully");
    return true;
  }
  Serial.println("Write failed");
  file.close();
  return false;
}

// ============================================================================
// Request Handlers (adapted from main.cpp)
// ============================================================================

void handleRoot(AsyncWebServerRequest *request) {
  if (!LittleFS.begin()) {
    request->send(500, "text/plain", "Filesystem error");
    return;
  }
  File file = LittleFS.open("/index.html", "r");
  if (!file) {
    request->send(404, "text/plain", "File not found");
    return;
  }
  request->send(LittleFS, "/index.html", "text/html");
  file.close();
}

void handleAction(AsyncWebServerRequest *request) {
  if (!request->hasParam("do")) {
    request->send(400, "text/plain", "Missing 'do' parameter");
    return;
  }
  
  String action = request->getParam("do")->value();
  
  // Handle all actions - same as original handleAction() function
  // We'll call the appropriate functions which are defined in main.cpp
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
    request->send(400, "text/plain", "Invalid action");
    return;
  }
  request->send(200, "text/plain", "OK");
}

void handleSetSpeed(AsyncWebServerRequest *request) {
  if (!request->hasParam("speed")) {
    request->send(400, "text/plain", "Missing 'speed' parameter");
    return;
  }
  
  String speedStr = request->getParam("speed")->value();
  if (speedStr.length() > 0) {
    unsigned long newDelay = speedStr.toInt();
    if (newDelay >= 100 && newDelay <= 5000) {
      color_pair_delay = newDelay;
      Serial.printf("Speed set to %lums\n", color_pair_delay);
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Speed must be between 100 and 5000ms");
    }
  } else {
    request->send(400, "text/plain", "Missing speed parameter");
  }
}

void handleStyle(AsyncWebServerRequest *request) {
  if (!LittleFS.begin()) {
    request->send(500, "text/plain", "Filesystem error");
    return;
  }
  request->send(LittleFS, "/style.css", "text/css");
}

void handleScript(AsyncWebServerRequest *request) {
  if (!LittleFS.begin()) {
    request->send(500, "text/plain", "Filesystem error");
    return;
  }
  request->send(LittleFS, "/script.js", "application/javascript");
}

// ============================================================================
// ElegantOTA Task (combines web server and OTA)
// ============================================================================

// ElegantOTA callbacks
void onOTAStart() {
  Serial.println("OTA update started!");
  otaInProgress = true;
}

void onOTAProgress(size_t current, size_t final) {
  static unsigned long ota_progress_millis = 0;
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress: %u/%u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("OTA update failed!");
  }
  otaInProgress = false;
}
void elegantOTATask(void *parameter) {
  Serial.println("ElegantOTA task started");

  // Setup web server routes

  // Serve index.html (our control page) at root
  server.on("/", HTTP_GET, handleRoot);

  // Serve our action handler
  server.on("/action", HTTP_GET, handleAction);

  // Serve static files
  server.on("/style.css", HTTP_GET, handleStyle);
  server.on("/script.js", HTTP_GET, handleScript);

  // Speed control
  server.on("/set_speed", HTTP_GET, handleSetSpeed);

  // Captive portal redirects for various devices
  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/");  // Android captive portal check
  });
  server.on("/hotspot-detect.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/");  // Apple captive portal check
  });
  server.on("/connectivity-check.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/");  // Windows/Linux captive portal check
  });

  // System info endpoint (optional, for debugging)
  server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request) {
    DynamicJsonDocument doc(256);
    doc["freeHeap"] = ESP.getFreeHeap();
    doc["chipModel"] = ESP.getChipModel();
    doc["apIP"] = WiFi.softAPIP().toString();
    doc["connectedClients"] = WiFi.softAPgetStationNum();
    doc["otaInProgress"] = otaInProgress;
    String jsonStr;
    serializeJson(doc, jsonStr);
    request->send(200, "application/json", jsonStr);
  });

  // 404 handler - redirect to root for captive portal
  server.onNotFound([](AsyncWebServerRequest *request) {
    if (captivePortalActive) {
      request->redirect("/");
    } else {
      request->send(404, "text/plain", "Not found");
    }
  });

  // Start ElegantOTA
  ElegantOTA.begin(&server);
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  server.begin();
  Serial.println("EasyOTA web server started");

  // Main task loop
  for (;;) {
    ElegantOTA.loop();
    if (captivePortalActive) {
      dnsServer.processNextRequest();
    }
    vTaskDelay(pdMS_TO_TICKS(10)); // 10ms delay
  }
}
