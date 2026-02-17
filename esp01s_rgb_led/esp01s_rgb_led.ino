/*
 * ESP-01S RGB LED Control with Web Interface
 * 
 * This program creates a WiFi access point and web server to control
 * an RGB LED connected to an ESP-01S module. Users can control colors
 * via a web interface or let it cycle through colors automatically.
 * 
 * Hardware Requirements:
 * - ESP-01S module
 * - RGB LED module with built-in resistors
 * 
 * Pin Connections:
 * - GPIO0 (Pin 5) -> R (Red pin of RGB LED module)
 * - GPIO2 (Pin 3) -> G (Green pin of RGB LED module)  
 * - GPIO1/TX (Pin 2) -> B (Blue pin of RGB LED module)
 * - GND -> GND (Ground pin of RGB LED module)
 * 
 * Note: After uploading, TX can be used as a GPIO pin
 * 
 * WiFi Access Point:
 * - SSID: RGB-Server
 * - IP Address: 192.168.4.1
 * - Connect to the network and navigate to http://192.168.4.1
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Define GPIO pins for RGB LED
#define RED_PIN   0  // GPIO0
#define GREEN_PIN 2  // GPIO2
#define BLUE_PIN  1  // GPIO1 (TX)

// Transition speed settings
#define TRANSITION_DELAY 15    // Delay between each step (lower = faster)
#define FADE_STEPS 100         // Number of steps in each color transition

// WiFi Access Point settings
const char* ssid = "RGB-Server";
const char* password = "";  // Open network (no password)

// Web server on port 80
ESP8266WebServer server(80);

// Current RGB values
int currentRed = 255;
int currentGreen = 0;
int currentBlue = 0;

// Control mode: true = manual control, false = auto cycling
bool manualMode = false;

// Variables for smooth cycling in auto mode
unsigned long lastTransitionTime = 0;
int cycleState = 0;  // Which transition we're in
int transitionStep = 0;  // Current step in the transition

void setup() {
  // Initialize GPIO pins as outputs
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  // Start with red
  setColor(255, 0, 0);
  
  // Set up WiFi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  // Wait a bit for AP to start
  delay(100);
  
  // Configure web server routes
  server.on("/", handleRoot);
  server.on("/setColor", handleSetColor);
  server.on("/setMode", handleSetMode);
  server.on("/getStatus", handleGetStatus);
  
  // Start web server
  server.begin();
}

void loop() {
  // Handle web server requests
  server.handleClient();
  
  // If in auto mode, run the color cycling logic
  if (!manualMode) {
    unsigned long currentTime = millis();
    
    // Check if it's time for the next transition step
    if (currentTime - lastTransitionTime >= TRANSITION_DELAY) {
      lastTransitionTime = currentTime;
      
      // Define all transitions in the color wheel
      int transitions[6][6] = {
        {255, 0, 0, 255, 255, 0},      // Red to Yellow
        {255, 255, 0, 0, 255, 0},      // Yellow to Green
        {0, 255, 0, 0, 255, 255},      // Green to Cyan
        {0, 255, 255, 0, 0, 255},      // Cyan to Blue
        {0, 0, 255, 255, 0, 255},      // Blue to Magenta
        {255, 0, 255, 255, 0, 0}       // Magenta to Red
      };
      
      // Get current transition
      int r1 = transitions[cycleState][0];
      int g1 = transitions[cycleState][1];
      int b1 = transitions[cycleState][2];
      int r2 = transitions[cycleState][3];
      int g2 = transitions[cycleState][4];
      int b2 = transitions[cycleState][5];
      
      // Calculate intermediate color values
      currentRed = r1 + (r2 - r1) * transitionStep / FADE_STEPS;
      currentGreen = g1 + (g2 - g1) * transitionStep / FADE_STEPS;
      currentBlue = b1 + (b2 - b1) * transitionStep / FADE_STEPS;
      
      setColor(currentRed, currentGreen, currentBlue);
      
      // Move to next step
      transitionStep++;
      
      // If transition complete, move to next one
      if (transitionStep > FADE_STEPS) {
        transitionStep = 0;
        cycleState = (cycleState + 1) % 6;  // Loop through 6 transitions
      }
    }
  }
}

// Web server handler: Serve the main webpage
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>RGB LED Control</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; max-width: 500px; margin: 50px auto; padding: 20px; background: #f0f0f0; }";
  html += ".container { background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
  html += "h1 { color: #333; text-align: center; margin-bottom: 30px; }";
  html += ".control-group { margin-bottom: 25px; }";
  html += "label { display: block; font-weight: bold; margin-bottom: 8px; color: #555; }";
  html += "input[type='range'] { width: 100%; height: 8px; border-radius: 5px; outline: none; }";
  html += "input[type='range']::-webkit-slider-thumb { width: 20px; height: 20px; border-radius: 50%; cursor: pointer; }";
  html += "#redSlider { background: linear-gradient(to right, #000, #f00); }";
  html += "#redSlider::-webkit-slider-thumb { background: #f00; }";
  html += "#greenSlider { background: linear-gradient(to right, #000, #0f0); }";
  html += "#greenSlider::-webkit-slider-thumb { background: #0f0; }";
  html += "#blueSlider { background: linear-gradient(to right, #000, #00f); }";
  html += "#blueSlider::-webkit-slider-thumb { background: #00f; }";
  html += ".value-display { text-align: right; font-size: 18px; color: #333; margin-top: 5px; }";
  html += ".preview { width: 100%; height: 80px; border-radius: 5px; margin: 20px 0; border: 2px solid #ddd; }";
  html += ".mode-buttons { display: flex; gap: 10px; margin-top: 20px; }";
  html += "button { flex: 1; padding: 12px; font-size: 16px; border: none; border-radius: 5px; cursor: pointer; transition: 0.3s; }";
  html += ".btn-auto { background: #4CAF50; color: white; }";
  html += ".btn-auto:hover { background: #45a049; }";
  html += ".btn-manual { background: #2196F3; color: white; }";
  html += ".btn-manual:hover { background: #0b7dda; }";
  html += ".btn-active { box-shadow: 0 0 15px rgba(0,0,0,0.3); transform: scale(1.05); }";
  html += ".status { text-align: center; margin-top: 15px; padding: 10px; border-radius: 5px; font-weight: bold; }";
  html += ".status-auto { background: #c8e6c9; color: #2e7d32; }";
  html += ".status-manual { background: #bbdefb; color: #1565c0; }";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>RGB LED Control</h1>";
  html += "<div id='preview' class='preview'></div>";
  html += "<div class='control-group'>";
  html += "<label>Red: <span id='redValue'>0</span></label>";
  html += "<input type='range' id='redSlider' min='0' max='255' value='0'>";
  html += "</div>";
  html += "<div class='control-group'>";
  html += "<label>Green: <span id='greenValue'>0</span></label>";
  html += "<input type='range' id='greenSlider' min='0' max='255' value='0'>";
  html += "</div>";
  html += "<div class='control-group'>";
  html += "<label>Blue: <span id='blueValue'>0</span></label>";
  html += "<input type='range' id='blueSlider' min='0' max='255' value='0'>";
  html += "</div>";
  html += "<div class='mode-buttons'>";
  html += "<button class='btn-auto' onclick='setMode(\"auto\")'>Auto Cycle</button>";
  html += "<button class='btn-manual' onclick='setMode(\"manual\")'>Manual Control</button>";
  html += "</div>";
  html += "<div id='status' class='status'></div>";
  html += "</div>";
  html += "<script>";
  html += "let manualMode = false;";
  html += "function updateColor() {";
  html += "  if (!manualMode) return;";
  html += "  const r = document.getElementById('redSlider').value;";
  html += "  const g = document.getElementById('greenSlider').value;";
  html += "  const b = document.getElementById('blueSlider').value;";
  html += "  document.getElementById('redValue').textContent = r;";
  html += "  document.getElementById('greenValue').textContent = g;";
  html += "  document.getElementById('blueValue').textContent = b;";
  html += "  document.getElementById('preview').style.backgroundColor = `rgb(${r},${g},${b})`;";
  html += "  fetch(`/setColor?r=${r}&g=${g}&b=${b}`);";
  html += "}";
  html += "function setMode(mode) {";
  html += "  manualMode = (mode === 'manual');";
  html += "  fetch(`/setMode?mode=${mode}`).then(() => {";
  html += "    updateStatus();";
  html += "    if (manualMode) updateColor();";
  html += "  });";
  html += "}";
  html += "function updateStatus() {";
  html += "  fetch('/getStatus').then(r => r.json()).then(data => {";
  html += "    const status = document.getElementById('status');";
  html += "    const btns = document.querySelectorAll('button');";
  html += "    btns.forEach(b => b.classList.remove('btn-active'));";
  html += "    if (data.mode === 'auto') {";
  html += "      status.textContent = 'Mode: Auto Cycling';";
  html += "      status.className = 'status status-auto';";
  html += "      btns[0].classList.add('btn-active');";
  html += "    } else {";
  html += "      status.textContent = 'Mode: Manual Control';";
  html += "      status.className = 'status status-manual';";
  html += "      btns[1].classList.add('btn-active');";
  html += "    }";
  html += "    document.getElementById('redSlider').value = data.red;";
  html += "    document.getElementById('greenSlider').value = data.green;";
  html += "    document.getElementById('blueSlider').value = data.blue;";
  html += "    document.getElementById('redValue').textContent = data.red;";
  html += "    document.getElementById('greenValue').textContent = data.green;";
  html += "    document.getElementById('blueValue').textContent = data.blue;";
  html += "    document.getElementById('preview').style.backgroundColor = `rgb(${data.red},${data.green},${data.blue})`;";
  html += "  });";
  html += "}";
  html += "document.getElementById('redSlider').addEventListener('input', updateColor);";
  html += "document.getElementById('greenSlider').addEventListener('input', updateColor);";
  html += "document.getElementById('blueSlider').addEventListener('input', updateColor);";
  html += "updateStatus();";
  html += "setInterval(updateStatus, 1000);";
  html += "</script></body></html>";
  
  server.send(200, "text/html", html);
}

// Web server handler: Set RGB color values
void handleSetColor() {
  if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
    currentRed = server.arg("r").toInt();
    currentGreen = server.arg("g").toInt();
    currentBlue = server.arg("b").toInt();
    
    // Constrain values to valid range
    currentRed = constrain(currentRed, 0, 255);
    currentGreen = constrain(currentGreen, 0, 255);
    currentBlue = constrain(currentBlue, 0, 255);
    
    setColor(currentRed, currentGreen, currentBlue);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing parameters");
  }
}

// Web server handler: Set control mode (auto/manual)
void handleSetMode() {
  if (server.hasArg("mode")) {
    String mode = server.arg("mode");
    if (mode == "auto") {
      manualMode = false;
      // Reset cycling state to start from the beginning
      cycleState = 0;
      transitionStep = 0;
      lastTransitionTime = millis();
    } else if (mode == "manual") {
      manualMode = true;
    }
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing mode parameter");
  }
}

// Web server handler: Get current status
void handleGetStatus() {
  String json = "{";
  json += "\"mode\":\"" + String(manualMode ? "manual" : "auto") + "\",";
  json += "\"red\":" + String(currentRed) + ",";
  json += "\"green\":" + String(currentGreen) + ",";
  json += "\"blue\":" + String(currentBlue);
  json += "}";
  server.send(200, "application/json", json);
}

// Function to set RGB color
// Parameters: red, green, blue (0-255)
void setColor(int red, int green, int blue) {
  // Use analogWrite for PWM control (brightness levels)
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}
