#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "ESP8266HTTPClient.h"

// Wi-Fi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Google Script ID and Google Sheets settings
const char* host = "script.google.com";
const int httpsPort = 443;
const String GAS_ID = "YOUR_GOOGLE_SCRIPT_ID"; // Replace with your Google Apps Script ID

// Sensor pins
const int PROXIMITY_SENSOR_PIN = D1;
const int RAIN_SENSOR_PIN = D2;
const int RELAY_PIN = D3;


WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  delay(10);

  // Initialize sensor pins
  pinMode(PROXIMITY_SENSOR_PIN, INPUT);
  pinMode(RAIN_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Set time via NTP, as required for x.509 validation
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
}

void loop() {
  // Read sensor data
  int proximityValue = digitalRead(PROXIMITY_SENSOR_PIN);
  int rainValue = digitalRead(RAIN_SENSOR_PIN);

  // Control relay based on sensor readings
  bool relayState = (proximityValue == LOW || rainValue == HIGH);
  digitalWrite(RELAY_PIN, relayState);

  // Prepare data string
  String proximityStatus = (proximityValue == LOW) ? "Detected" : "Not Detected";
  String rainStatus = (rainValue == HIGH) ? "Rain" : "No Rain";
  String relayStatus = relayState ? "ON" : "OFF";
  String imageData = "image_placeholder"; // In a real scenario, this would be actual image data

  String dataString = "proximity=" + proximityStatus + 
                      "&rain=" + rainStatus + 
                      "&relay=" + relayStatus + 
                      "&image=" + imageData;

  // Send data to Google Sheets
  sendData(dataString);

  // Wait for 1 minute before next reading
  delay(60000);
}

// Function to send data to Google Sheets
void sendData(String params
