#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <IRremote.hpp>

// --- Pin Definitions ---
const uint16_t kIrLedPin = 4;
const uint16_t rPin = 0;
const uint16_t gPin = 1;
const uint16_t bPin = 2;

// --- Objects ---
DNSServer dnsServer;
WebServer server(80);

// --- Web Page ---
const char INDEX_HTML[] = R"rawliteral(
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
        .tabs { display: flex; margin-bottom: 20px; border-bottom: 2px solid #444; }
        .tab-button { padding: 10px 20px; background: #555; border: none; color: white; cursor: pointer; border-radius: 5px 5px 0 0; margin-right: 5px; }
        .tab-button.active { background: #777; }
        .tab-content { display: none; }
        .tab-content.active { display: block; }
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
        <div class="tabs">
            <button class="tab-button active" data-tab="k8-tab">K8 Remote</button>
            <button class="tab-button" data-tab="chinese-tab">Chinese Remote</button>
        </div>
        <div id="k8-tab" class="tab-content active">
            <div class="grid" id="button-grid-k8">
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
        <div id="chinese-tab" class="tab-content">
            <div class="grid" id="button-grid-chinese">
                <button class="btn red" data-action="chinese_red">Red</button>
                <button class="btn green" data-action="chinese_green">Green</button>
                <button class="btn blue" data-action="chinese_blue">Blue</button>
                <button class="btn white" data-action="chinese_white">White</button>
                <button class="btn gray" data-action="chinese_brt_up">BRT Up</button>
                <button class="btn gray" data-action="chinese_brt_down">BRT Down</button>
                <button class="btn dark" data-action="chinese_off">OFF</button>
                <button class="btn gray" data-action="chinese_on">ON</button>
                <button class="btn gray" data-action="chinese_flash">FLASH</button>
                <button class="btn gray" data-action="chinese_strobe">STROBE</button>
                <button class="btn gray" data-action="chinese_fade">FADE</button>
                <button class="btn gray" data-action="chinese_smooth">SMOOTH</button>
            </div>
        </div>
    </div>
    <script>
        // Tab switching
        document.querySelectorAll('.tab-button').forEach(button => {
            button.addEventListener('click', () => {
                document.querySelectorAll('.tab-button').forEach(btn => btn.classList.remove('active'));
                document.querySelectorAll('.tab-content').forEach(content => content.classList.remove('active'));
                button.classList.add('active');
                document.getElementById(button.dataset.tab).classList.add('active');
            });
        });

        // Button click handling for both grids
        document.getElementById('button-grid-k8').addEventListener('click', handleButtonClick);
        document.getElementById('button-grid-chinese').addEventListener('click', handleButtonClick);

        function handleButtonClick(event) {
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
        }
    </script>
</body>
</html>
)rawliteral";

// --- Forward Declarations ---
void Clear();
void Red(); void Green(); void Blue(); void Yellow(); void Cyan(); void Magenta(); void White();
void Off(); void Fade(); void Strobeplus(); void RGBStrobe(); void Rainbow();
void Halfstrobe(); void BGStrobe(); void GRStrobe(); void Next(); void Demo(); void Previous();
void ChineseRed(); void ChineseGreen(); void ChineseBlue(); void ChineseWhite();
void ChineseBRTUp(); void ChineseBRTDown(); void ChineseOFF(); void ChineseON();
void ChineseFLASH(); void ChineseSTROBE(); void ChineseFADE(); void ChineseSMOOTH();

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
    IrSender.sendNECMSB(0x00F720DF, 32, false);
}
void ChineseGreen() { 
    Serial.println("ChineseGreen called");
    IrSender.sendNECMSB(0x00F7A05F, 32, false);
}
void ChineseBlue() { 
    Serial.println("ChineseBlue called");
    IrSender.sendNECMSB(0x00F7609F, 32, false);
}
void ChineseWhite() { 
    Serial.println("ChineseWhite called");
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
