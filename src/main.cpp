#include <Arduino.h>
#include <Adafruit_BMP280.h>

//For reading battery voltage
ADC_MODE(ADC_VCC);
float voltage;

#define POWER_PIN D3

Adafruit_BMP280 BMP_Sensor;

void setup() {
  voltage = ((float) ((int)(ESP.getVcc() / 10.24 )) / 100 );

  Serial.begin(9600);
  Serial.print("\nSketch started voltage = "+(String)voltage+"v\n");
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);
  delay(500);

  /*
  if (!BMP_Sensor.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }*/
  
}

void loop() {
  if (!BMP_Sensor.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
  Serial.print("Temprature: ");
  Serial.print(BMP_Sensor.readTemperature());
  Serial.print(" *C\n");
  Serial.print("Pressure: ");
  Serial.print(BMP_Sensor.readPressure());
  Serial.print(" *C\n");

  digitalWrite(POWER_PIN, LOW);  
  delay(5000);
  digitalWrite(POWER_PIN, HIGH);
  delay(500);
}