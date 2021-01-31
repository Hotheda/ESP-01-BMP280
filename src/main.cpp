#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>

#include "credentials.h"

//For reading battery voltage
ADC_MODE(ADC_VCC);

#define POWER_PIN 3

// sleeping time
#define SLEEPING_TIME_IN_SECONDS  600 // 60 seconds

// MQTT: topic
#define myMQTT_SENSOR_TOPIC "home/outside/BMPsensor"

Adafruit_BMP280 BMP_Sensor;
WiFiClient wifiClient;
PubSubClient MQTTclient(wifiClient);

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {

}

int getVoltagePercent(float voltage){
  // from 2.5v to 3.3v
  float v = voltage - 2.5;
  if ( v < 0 ){
    v = 0;
  int voltagePercent = v / 0.8 * 100;
  return voltagePercent;
}

void SendMQTTData(float temp, float pressure, float voltage){
  StaticJsonDocument<200> jsonDoc;
  // INFO: the data must be converted into a string; a problem occurs when using floats...
  jsonDoc["temperature"] = (String)temp;
  jsonDoc["pressure"] = (String)pressure;
  jsonDoc["voltage"] = (String)getVoltagePercent(voltage);
  
  char data[200];
  serializeJson(jsonDoc, data);

  MQTTclient.publish(myMQTT_SENSOR_TOPIC, data, true );
}

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(myWifiSSID2, myWifiPASS2);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(500);
    //WiFi failed, retrying.
  }
  //WiFi connected

  // init the MQTT connection
  MQTTclient.setServer(myMQTT_SERVER_IP, myMQTT_SERVER_PORT);
  MQTTclient.setCallback(callback);
  MQTTclient.connect(myMQTT_CLIENT_ID, myMQTT_USER, myMQTT_PASSWORD);
  
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);
  delay(1000);

  //I2C stuff
    Wire.begin(0, 2);
}

void loop() {
  float voltage = ((float) ((int)(ESP.getVcc() / 10.24 )) / 100 );
  
  float temp = 99.0f;
  float pressure = 99.0f;
  if (BMP_Sensor.begin(0x77)) {
    temp = BMP_Sensor.readTemperature();
    pressure = BMP_Sensor.readPressure();
  }

  digitalWrite(POWER_PIN, LOW);
  SendMQTTData(temp, pressure, voltage);

  MQTTclient.disconnect();
  WiFi.disconnect();

  ESP.deepSleep(SLEEPING_TIME_IN_SECONDS * 1500000, WAKE_RF_DEFAULT);
  delay(500); // wait for deep sleep to happen
}