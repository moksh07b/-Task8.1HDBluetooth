#include <ArduinoBLE.h>
#include <Wire.h>
#include <BH1750.h>

// Define a custom BLE service with a UUID
BLEService customService("f47ac10b-58cc-4372-a567-0e02b2c3d479");                                              
// Create a BLE characteristic for sending data, with read and notify properties
BLEStringCharacteristic customCharacteristic("f47ac10b-58cc-4372-a567-0e02b2c3d480", BLERead | BLENotify, 20);  
// Initialize the BH1750 light sensor with its I2C address
BH1750 lightSensor(0x23);

unsigned long lastMeasurementTime = 0;           // Timestamp of the last light measurement
const unsigned long measurementInterval = 1000;  // Interval for taking measurements in milliseconds

void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud rate

  // Initialize I2C communication and the light sensor
  Wire.begin();
  lightSensor.begin();

  BLE.begin();  // Initialize BLE
  // Check if BLE initialization was successful
  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);  // Halt execution if BLE initialization fails
  }

  // Set the local name for the BLE device and advertise the custom service
  BLE.setLocalName("Nano33IoT");
  BLE.setAdvertisedService(customService);

  // Add the characteristic to the custom service
  customService.addCharacteristic(customCharacteristic);

  // Add the custom service to the BLE stack
  BLE.addService(customService);

  // Start advertising the BLE service
  BLE.advertise();

  Serial.println("BLE device active, waiting for connection...");
}

void loop() {
  // Wait for a central device (e.g., a smartphone) to connect
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());  // Print the address of the connected central device

    // Continue as long as the central device is connected
    while (central.connected()) {
      // Check if it's time for the next light measurement
      if (millis() - lastMeasurementTime >= measurementInterval) {
        lastMeasurementTime = millis();  // Update the last measurement timestamp

        // Read the light level from the sensor
        float lux = lightSensor.readLightLevel();

        // Verify if the reading was successful
        if (lux >= 0) {
          Serial.print("Light Intensity: ");
          Serial.print(lux);
          Serial.println(" lx");  // Output the light intensity in lux

          // Convert the light intensity to a String for BLE transmission
          String luxString = String((int)lux); // Convert lux value to a String

          // Send the light intensity value over BLE
          customCharacteristic.writeValue(luxString);
        } else {
          Serial.println("Failed to read light level.");  // Output error if reading fails
        }
      }

      BLE.poll();  // Poll BLE events
    }

    Serial.println("Disconnected from central");  // Indicate disconnection from the central device
  }

  BLE.poll();  // Poll BLE events when not connected
}
