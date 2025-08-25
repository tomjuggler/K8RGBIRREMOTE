#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <FastLED.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// --- Pin Definitions ---
const uint16_t kLedPin = 8;
const uint16_t kIrLedPin = 4;

// --- Objects ---
DNSServer dnsServer;
WebServer server(80);
IRsend irsend(kIrLedPin);  // Create IRsend object on the correct pin
CRGB leds[1];

// --- Web Page ---
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>K8 Remote Control</title>
    <style>
        body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; margin: 0; background-color: #2c2c2c; color: white; text-align: center; }
        .container { padding: 20px; max-width: 800px; margin: 0 auto; }
        h1 { margin-bottom: 30px; }
        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(120px, 1fr)); gap: 15px; }
        .btn { padding: 20px; border: none; border-radius: 10px; font-size: 1rem; font-weight: bold; color: white; cursor: pointer; transition: transform 0.1s ease; }
        .btn:active { transform: scale(0.95); }
        .red { background-color: #e74c3c; }
        .green { background-color: #2ecc71; }
        .blue { background-color: #3498db; }
        .yellow { background-color: #f1c40f; }
        .cyan { background-color: #1abc9c; }
        .magenta { background-color: #9b59b6; }
        .white { background-color: #ecf0f1; color: #2c3e3c; }
        .gray { background-color: #7f8c8d; }
        .dark { background-color: #34495e; }
    </style>
</head>
<body>
    <div class="container">
        <h1>K8 RGB IR Remote</h1>
        <div class="grid" id="button-grid">
            <button class="btn red" data-action="red">Red</button>
            <button class="btn green" data-action="green">Green</button>
            <button class="btn blue" data-action="blue">Blue</button>
            <button class="btn yellow" data-action="yellow">Yellow</button>
            <button class="btn cyan" data-action="cyan">Cyan</button>
            <button class="btn magenta" data-action="magenta">Magenta</button>
            <button class="btn white" data-action="white">White</button>
            <button class="btn dark" data-action="off">Off</button>
            <button class="btn gray" data-action="fade">Fade</button>
            <button class="btn gray" data-action="strobeplus">Strobe+</button>
            <button class="btn gray" data-action="rgbstrobe">RGB Strobe</button>
            <button class="btn gray" data-action="rainbow">Rainbow</button>
            <button class="btn gray" data-action="halfstrobe">Half Strobe</button>
            <button class="btn gray" data-action="bgstrobe">BG Strobe</button>
            <button class="btn gray" data-action="grstrobe">GR Strobe</button>
            <button class="btn gray" data-action="next">Next</button>
            <button class="btn gray" data-action="demo">Demo</button>
            <button class="btn gray" data-action="previous">Previous</button>
        </div>
    </div>
    <script>
        document.getElementById('button-grid').addEventListener('click', (event) => {
            if (event.target.tagName === 'BUTTON') {
                const action = event.target.dataset.action;
                fetch(`/action?do=${action}`)
                    .then(response => {
                        if (!response.ok) {
                            console.error('Error sending command');
                        }
                    })
                    .catch(error => console.error('Fetch error:', error));
            }
        });
    </script>
</body>
</html>
)rawliteral";

// --- Forward Declarations ---
void Clear();
void Red(); void Green(); void Blue(); void Yellow(); void Cyan(); void Magenta(); void White();
void Off(); void Fade(); void Strobeplus(); void RGBStrobe(); void Rainbow();
void Halfstrobe(); void BGStrobe(); void GRStrobe(); void Next(); void Demo(); void Previous();

void handleRoot() {
    server.send(200, "text/html", INDEX_HTML);
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

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    // Setup FastLED
    FastLED.addLeds<NEOPIXEL, kLedPin>(leds, 1);
    Clear();

    // Setup IR Sender
    irsend.begin();

    // Setup WiFi AP
    WiFi.softAP("K8_RGB_IR_REMOTE");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    // Setup DNS Server for Captive Portal
    dnsServer.start(53, "*", WiFi.softAPIP());

    // Setup Web Server
    server.on("/", HTTP_GET, handleRoot);
    server.on("/action", HTTP_GET, handleAction);
    server.onNotFound(handleNotFound); // Captive portal redirect
    server.begin();

    Serial.println("HTTP server started");
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
}

// --- Color and Command Functions ---
void Clear() { 
    Serial.println("Clear called");
    leds[0] = CRGB::Black; 
    FastLED.show(); 
}
void Red() { 
    Serial.println("Red called");
    Clear(); 
    leds[0] = CRGB::Red; 
    FastLED.show(); 
    irsend.sendNEC(0xFF10EF); 
}
void Green() { 
    Serial.println("Green called");
    Clear(); 
    leds[0] = CRGB::Green; 
    FastLED.show(); 
    irsend.sendNEC(0xFF906F); 
}
void Blue() { 
    Serial.println("Blue called");
    Clear(); 
    leds[0] = CRGB::Blue; 
    FastLED.show(); 
    irsend.sendNEC(0xFF50AF); 
}
void Yellow() { 
    Serial.println("Yellow called");
    Clear(); 
    leds[0].setRGB(150, 150, 0); 
    FastLED.show(); 
    irsend.sendNEC(0xFFD02F); 
}
void Cyan() { 
    Serial.println("Cyan called");
    Clear(); 
    leds[0].setRGB(0, 150, 150); 
    FastLED.show(); 
    irsend.sendNEC(0xFFB04F); 
}
void Magenta() { 
    Serial.println("Magenta called");
    Clear(); 
    leds[0].setRGB(200, 0, 140); 
    FastLED.show(); 
    irsend.sendNEC(0xFF30CF); 
}
void White() { 
    Serial.println("White called");
    leds[0].setRGB(140, 140, 140); 
    FastLED.show(); 
    irsend.sendNEC(0xFF708F); 
}
void Off() { 
    Serial.println("Off called");
    Clear(); 
    irsend.sendNEC(0xFFE01F); 
}
void Fade() { 
    Serial.println("Fade called");
    irsend.sendNEC(0xFFF00F); 
}
void Strobeplus() { 
    Serial.println("Strobeplus called");
    irsend.sendNEC(0xFFA857); 
}
void RGBStrobe() { 
    Serial.println("RGBStrobe called");
    irsend.sendNEC(0xFF28D7); 
}
void Rainbow() { 
    Serial.println("Rainbow called");
    irsend.sendNEC(0xFF6897); 
}
void Halfstrobe() { 
    Serial.println("Halfstrobe called");
    irsend.sendNEC(0xFFE817); 
}
void BGStrobe() { 
    Serial.println("BGStrobe called");
    irsend.sendNEC(0xFF9867); 
}
void GRStrobe() { 
    Serial.println("GRStrobe called");
    irsend.sendNEC(0xFF18E7); 
}
void Next() { 
    Serial.println("Next called");
    irsend.sendNEC(0xFF20DF); 
}
void Demo() { 
    Serial.println("Demo called");
    irsend.sendNEC(0xFF58A7); 
}
void Previous() { 
    Serial.println("Previous called");
    irsend.sendNEC(0xFFA05F); 
}