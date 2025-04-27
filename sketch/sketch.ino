#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "EmonLib.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C // Common OLED I2C address

// Create instances
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
EnergyMonitor emon1;

// Define input pins
#define CURRENT_PIN A0  // Connect SCT-013 to this pin (with proper circuit)

// Variables
float current = 0;
float voltage = 220.0;  // Default voltage value if no voltage sensor
float power = 0;

// Calibration variables
#define CURRENT_CAL 111.1    // Current calibration (for SCT-013-000 100A/50mA)
float noise_level = 0.1;     // Threshold below which current reading is considered noise

unsigned long lastDisplay = 0;
const int displayInterval = 1000;  // Update every second

void setup() {
  Serial.begin(9600);
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Display startup message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Current Sensor"));
  display.println(F("Initializing..."));
  display.display();
  delay(2000);
  
  // Initialize energy monitor
  emon1.current(CURRENT_PIN, CURRENT_CAL);  // Current: input pin, calibration
  
  // Calibrate zero point
  calibrateZeroPoint();
  
  display.clearDisplay();
}

void calibrateZeroPoint() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Calibrating..."));
  display.println(F("Keep sensor away"));
  display.println(F("from current"));
  display.display();
  
  // Take several readings and average them to get a baseline
  float total = 0;
  int samples = 10;
  
  for(int i=0; i<samples; i++) {
    total += emon1.calcIrms(1480);
    delay(200);
  }
  
  noise_level = total / samples + 0.05;  // Add a small margin
  
  Serial.print("Noise level calibrated to: ");
  Serial.println(noise_level);
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("Calibration done!"));
  display.print(F("Noise level: "));
  display.println(noise_level);
  display.display();
  delay(2000);
}

void loop() {
  // Read current
  current = emon1.calcIrms(1480);  // Calculate Irms only, 1480 sample points
  
  // Apply noise filter - if reading is below noise threshold, consider it zero
  if (current < noise_level) {
    current = 0;
  }
  
  // Calculate power (P = IV)
  power = current * voltage;
  
  // Print values to serial monitor for debugging
  Serial.print("Current: ");
  Serial.print(current);
  Serial.print(" A, Voltage: ");
  Serial.print(voltage);
  Serial.print(" V, Power: ");
  Serial.print(power);
  Serial.println(" W");
  
  // Update display at specified intervals
  if (millis() - lastDisplay >= displayInterval) {
    lastDisplay = millis();
    updateDisplay();
  }
}

void updateDisplay() {
  display.clearDisplay();
  
  // Title
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Power Monitor"));
  
  // Current
  display.setCursor(0,10);
  display.print(F("Current: "));
  if (current == 0) {
    display.println(F("0.00 A"));
  } else {
    display.print(current, 2);
    display.println(F(" A"));
  }
  
  // Voltage
  display.setCursor(0,20);
  display.print(F("Voltage: "));
  display.print(voltage, 1);
  display.println(F(" V"));
  
  // Power
  display.setCursor(60,20);
  display.print(F("P: "));
  if (power < 1) {
    display.println(F("0 W"));
  } else {
    display.print(power, 0);
    display.println(F(" W"));
  }
  
  display.display();
}
