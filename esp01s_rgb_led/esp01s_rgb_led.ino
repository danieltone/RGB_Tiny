/*
 * ESP-01S RGB LED Color Cycling Program
 * 
 * This program cycles through different colors on an RGB LED
 * connected to an ESP-01S module.
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
 */

// Define GPIO pins for RGB LED
#define RED_PIN   0  // GPIO0
#define GREEN_PIN 2  // GPIO2
#define BLUE_PIN  1  // GPIO1 (TX)

// Transition speed settings
#define TRANSITION_DELAY 15    // Delay between each step (lower = faster)
#define FADE_STEPS 100         // Number of steps in each color transition

void setup() {
  // Initialize GPIO pins as outputs
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  // Start with red
  setColor(255, 0, 0);
}

void loop() {
  // Smooth color wheel effect that repeats indefinitely
  // Transitions through the full spectrum in a smooth gradient
  
  // Red to Yellow (increase green)
  smoothTransition(255, 0, 0, 255, 255, 0);
  
  // Yellow to Green (decrease red)
  smoothTransition(255, 255, 0, 0, 255, 0);
  
  // Green to Cyan (increase blue)
  smoothTransition(0, 255, 0, 0, 255, 255);
  
  // Cyan to Blue (decrease green)
  smoothTransition(0, 255, 255, 0, 0, 255);
  
  // Blue to Magenta (increase red)
  smoothTransition(0, 0, 255, 255, 0, 255);
  
  // Magenta to Red (decrease blue)
  smoothTransition(255, 0, 255, 255, 0, 0);
  
  // Loop automatically repeats from red again
}

// Function to smoothly transition from one color to another
void smoothTransition(int r1, int g1, int b1, int r2, int g2, int b2) {
  for (int i = 0; i <= FADE_STEPS; i++) {
    // Calculate intermediate color values
    int red = r1 + (r2 - r1) * i / FADE_STEPS;
    int green = g1 + (g2 - g1) * i / FADE_STEPS;
    int blue = b1 + (b2 - b1) * i / FADE_STEPS;
    
    setColor(red, green, blue);
    delay(TRANSITION_DELAY);
  }
}

// Function to set RGB color
// Parameters: red, green, blue (0-255)
void setColor(int red, int green, int blue) {
  // Use analogWrite for PWM control (brightness levels)
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}
