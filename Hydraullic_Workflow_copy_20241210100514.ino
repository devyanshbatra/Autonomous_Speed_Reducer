#include <Wire.h>

// Pin definitions
const int HYDRAULIC_RELAY_PIN = 2; // GPIO pin connected to the hydraulic control relay

// Visibility sensor I2C address
const int VISIBILITY_SENSOR_ADDRESS = 0x76; 
const int VISIBILITY_REGISTER = 0x00;      

// Visibility threshold
const float VISIBILITY_THRESHOLD = 15.0; // Threshold in meters

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Initialize hydraulic control pin
  pinMode(HYDRAULIC_RELAY_PIN, OUTPUT);
  digitalWrite(HYDRAULIC_RELAY_PIN, HIGH); // Default to HIGH state
}

void loop() {
  // Read visibility from the sensor
  float visibility = readVisibility();

  if (visibility >= 0) {
    if (visibility < VISIBILITY_THRESHOLD) {
      Serial.println("Low visibility - Turning hydraulic DOWN");
      digitalWrite(HYDRAULIC_RELAY_PIN, LOW); 
    } else {
      Serial.println("Good visibility - Turning hydraulic UP");
      digitalWrite(HYDRAULIC_RELAY_PIN, HIGH); 
    }
  } else {
    Serial.println("Error reading visibility sensor!");
  }
  delay(1000);

float readVisibility() {
  // Request data from the visibility sensor
  Wire.beginTransmission(VISIBILITY_SENSOR_ADDRESS);
  Wire.write(VISIBILITY_REGISTER); // Register to read
  Wire.endTransmission();

  // Request 2 bytes from the sensor
  Wire.requestFrom(VISIBILITY_SENSOR_ADDRESS, 2);
  if (Wire.available() == 2) {
    uint16_t visibilityRaw = Wire.read() << 8 | Wire.read();
    return visibilityRaw / 10.0; 
  }
  return -1.0;
}
