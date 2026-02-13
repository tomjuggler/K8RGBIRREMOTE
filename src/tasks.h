#ifndef TASKS_H
#define TASKS_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

// Global variables that need to be shared between files
extern AsyncWebServer server;
extern DNSServer dnsServer;
extern bool otaInProgress;
extern bool captivePortalActive;

// For our project - we're keeping AP mode only
// So we don't need the complex WiFi settings structure
// But we need to keep our pattern control variables
extern unsigned long color_pair_delay;
extern int currentPattern;
extern int patternState;
extern unsigned long lastPatternTime;

// Our action handler function declarations
void handleRoot(AsyncWebServerRequest *request);
void handleAction(AsyncWebServerRequest *request);
void handleSetSpeed(AsyncWebServerRequest *request);
void handleStyle(AsyncWebServerRequest *request);
void handleScript(AsyncWebServerRequest *request);
String getContentType(String filename);

// EasyOTA task function
void elegantOTATask(void *parameter);

// Color and command functions (declared in main.cpp, but we need them for tasks.cpp)
// These will be defined in main.cpp
void Clear();
void Red(); void Green(); void Blue(); void Yellow(); void Cyan(); void Magenta(); void White();
void Off(); void Fade(); void Strobeplus(); void RGBStrobe(); void Rainbow();
void Halfstrobe(); void BGStrobe(); void GRStrobe(); void Next(); void Demo(); void Previous();
void ChineseRed(); void ChineseGreen(); void ChineseBlue(); void ChineseWhite();
void ChineseBRTUp(); void ChineseBRTDown(); void ChineseOFF(); void ChineseON();
void ChineseFLASH(); void ChineseSTROBE(); void ChineseFADE(); void ChineseSMOOTH();
void extra_red_blue(); void extra_red_green(); void extra_red_white(); 
void extra_green_blue(); void extra_green_white(); void extra_blue_white();

#endif // TASKS_H
