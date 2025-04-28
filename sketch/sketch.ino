#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "EmonLib.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
EnergyMonitor emon1;

#define CURRENT_PIN A0  

float current = 0;
float voltage = 220.0;  
float power = 0;

#define CURRENT_CAL 111.1    
float noise_level = 0.1;      

unsigned long lastDisplay = 0;
const int displayInterval = 1000;  

void setup() {
  Serial.begin(9600);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Current Sensor"));
  display.println(F("Initializing..."));
  display.display();
  delay(2000);
  
  emon1.current(CURRENT_PIN, CURRENT_CAL);  
  
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
  
  float total = 0;
  int samples = 10;
  
  for(int i=0; i<samples; i++) {
    total += emon1.calcIrms(1480);
    delay(200);
  }
  
  noise_level = total / samples + 0.05;  
  
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
  current = emon1.calcIrms(1480);  
  
  if (current < noise_level) {
    current = 0;
  }
  
  power = current * voltage;
  
  Serial.print("Current: ");
  Serial.print(current);
  Serial.print(" A, Voltage: ");
  Serial.print(voltage);
  Serial.print(" V, Power: ");
  Serial.print(power);
  Serial.println(" W");
  
  if (millis() - lastDisplay >= displayInterval) {
    lastDisplay = millis();
    updateDisplay();
  }
}

void updateDisplay() {
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Power Monitor"));
  
  display.setCursor(0,10);
  display.print(F("Current: "));
  if (current == 0) {
    display.println(F("0.00 A"));
  } else {
    display.print(current, 2);
    display.println(F(" A"));
  }
  
  display.setCursor(0,20);
  display.print(F("Voltage: "));
  display.print(voltage, 1);
  display.println(F(" V"));
  
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
