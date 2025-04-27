#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASS";
ESP8266WebServer server(80); // HTTP server on port 80void setup() {
 Serial.begin(115200);    
 WiFi.disconnect(); // Disconnect AP
 WiFi.mode(WIFI_STA);  
 WiFi.begin(ssid, password); // Connect to WIFI network// Wait for connection
 while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.println(".");
 }
 Serial.print("Connected to ");
 Serial.println(ssid);
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());server.on("/", [](){
  server.send(200, "text/plain", "Hello World");
 });server.begin(); // Start HTTP server
 Serial.println("HTTP server started.");
}void loop() {
 server.handleClient();
}
