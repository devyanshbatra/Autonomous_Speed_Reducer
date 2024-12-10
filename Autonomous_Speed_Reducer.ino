#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

// Wi-Fi Credentials
const char* ssid = "Zeus";
const char* password = "devyansh07";

// Google Apps Script Web App URL
const char* googleScriptURL = "https://script.google.com/u/0/home/projects/1WD4qkEa4CWuIZ-3EUJ5b6IydhjaED6lde1rbTA7fRko64ueCgUSDDUEa/edit";


// NTP Server details
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800; // Adjust for your timezone, e.g., -21600 for CST
const int daylightOffset_sec = 3600;

// Pin definitions
const int HYDRAULIC_RELAY_PIN = 2;
const int LIGHT_RELAY_PIN = 3;
const int LIGHT_SENSOR_PIN = A0;

// Weather sensor
Adafruit_BME280 bme;

const int MIN_LIGHT = 0;
const int MAX_LIGHT = 255;
int currentLightIntensity = 128;

// Visibility sensor
const int VISIBILITY_THRESHOLD = 15; 
const int VISIBILITY_SENSOR_ADDRESS = 0x52; 

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi!");

  // Initialize NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("NTP Initialized!");

  // Initialize weather sensor
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  
  // Set up relay pins
  pinMode(HYDRAULIC_RELAY_PIN, OUTPUT);
  pinMode(LIGHT_RELAY_PIN, OUTPUT);
  
  // Initialize light control
  analogWrite(LIGHT_RELAY_PIN, currentLightIntensity);
  
  Wire.begin(); // Initialize I2C communication
}

void loop() {
  // Read weather data
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;
  
  // Read visibility
  float visibility = readVisibility();
  
  // Get current timestamp
  String timestamp = getFormattedTime();
  
  // Send weather data to Google Sheets
  sendToGoogleSheets(temperature, humidity, pressure, visibility, timestamp);
  
  // Check visibility threshold
  if (visibility >= 0 && visibility < VISIBILITY_THRESHOLD) {
    Serial.println("WARNING:LowVisibility");
  }
  
  // Read light sensor and adjust light intensity
  int lightLevel = analogRead(LIGHT_SENSOR_PIN);
  adjustLightIntensity(lightLevel);
  
  // Delay before next loop iteration
  delay(60000); // Send data every 60 seconds
}

void sendToGoogleSheets(float temp, float hum, float pres, float vis, String timestamp) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(googleScriptURL) + "?temperature=" + String(temp) + 
                 "&humidity=" + String(hum) + "&pressure=" + String(pres) + 
                 "&visibility=" + String(vis) + "&timestamp=" + timestamp;
                 
    http.begin(url);
    int httpCode = http.GET();
    
    if (httpCode > 0) {
      Serial.println("Data sent successfully!");
      Serial.println(http.getString());
    } else {
      Serial.println("Error sending data!");
    }
    
    http.end();
  } else {
    Serial.println("Wi-Fi disconnected!");
  }
}

void adjustLightIntensity(int sensorValue) {
  int mappedValue = map(sensorValue, 0, 1023, MIN_LIGHT, MAX_LIGHT);
  if (abs(mappedValue - currentLightIntensity) > 10) {
    setLightIntensity(mappedValue);
  }
}

void setLightIntensity(int intensity) {
  currentLightIntensity = constrain(intensity, MIN_LIGHT, MAX_LIGHT);
  analogWrite(LIGHT_RELAY_PIN, currentLightIntensity);
}

float readVisibility() {
  Wire.beginTransmission(VISIBILITY_SENSOR_ADDRESS);
  Wire.write(0x00); 
  Wire.endTransmission();
  
  Wire.requestFrom(VISIBILITY_SENSOR_ADDRESS, 2);
  if (Wire.available() == 2) {
    uint16_t visibilityRaw = Wire.read() << 8 | Wire.read();
    return visibilityRaw / 10.0; 
  }
  return -1; // Error reading
}

String getFormattedTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "N/A";
  }
  char buffer[25];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}
