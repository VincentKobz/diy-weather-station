#include "DHT.h"
#include "WiFi.h"
#include <stdlib.h>
#include "PubSubClient.h"
#include <MQUnifiedsensor.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

#include "global_data.h"

// Classify CO2 ppm value to obtain air quality
void get_air_quality(float value)
{
  char *air_quality = NULL;
  if (value <= 600)
  {
    air_quality = "good";
  }
  else if (value <= 800)
  {
    air_quality = "acceptable";
  }
  else if (value <= 1000)
  {
    air_quality = "mediocre";
  }
  else
  {
    air_quality = "bad";
  }
  try_publish("esp32/out/air-quality", air_quality);
}

// Convert float to string
char *convert_float_to_string(float value, char *string)
{
  if (!string)
  {
    string = (char *)calloc(100, sizeof(char));
  }

  if (!string)
  {
    return NULL;
  }

  // Reset string
  memset(string, '\0', 100);
  // Fill string
  snprintf(string, 100, "%f", value);

  return string;
}

// Try to publish data into MQTT topic
void try_publish(char *topic, const char *data)
{
  char message[100];
  if (!client.connected())
  {
    reconnect();
  }
  if (!client.publish(topic, data))
  {
    snprintf(message, 100, "Failed to publish into %s\n", topic);
    Serial.println(message);
  }
  else
  {
    snprintf(message, 100, "Successfully publish %s into %s\n", data, topic);
    Serial.println(message);
  }
}

// MQTT callback
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.println("Message received !");
}

// Reconnect ESP32 to the MQTT server
void reconnect()
{
  if (!client.connect(MQTT_DEVICE_ID))
  {
    static char message[100];
    snprintf(message, 100, "Failed to connect to MQTT broker, error [%i] !", client.state());
    Serial.println(message);
  }
  else
  {
    Serial.println("Successfully connected to MQTT broker !");
  }
}

// Setup MQ135
void setup_mq135()
{
  MQ135.setRegressionMethod(1);
  MQ135.init();

  // MQ135 Calibration
  float calcR0 = 0;

  for(int i = 1; i<=10; i ++)
  {
    MQ135.update();
    calcR0 += MQ135.calibrate(RATIO_MQ135_CLEAN_AIR);
  }
  MQ135.setR0(calcR0/10);
}

// Setup MQTT broker
void setup_mqtt_broker()
{
  client.setServer(MQTT_SERVER_IP, MQTT_PORT);
  client.setCallback(callback);
}

// Connect ESP32 to a Wi-Fi hotspot
void setup_wifi()
{
  // Set connection to access point
  WiFi.mode(WIFI_STA);
  char message[100];

  while (true)
  {
    // Try to connect to SSID with given password
    WiFi.begin(SSID, WIFI_PWD);

    delay(1000);
    Serial.println("Trying to connect to WiFi ...");

    // Check Wi-Fi status
    switch (WiFi.status())
    {
    case WL_CONNECTED:
      snprintf(message, 100, "Successfully connected to %s WiFi !", SSID);
      Serial.println(message);
      return;
      break;
    case WL_NO_SHIELD:
      Serial.println("No WiFi shield present !");
      return;
      break;
    case WL_CONNECT_FAILED:
      snprintf(message, 100, "Failed to connect to %s WiFi !", SSID);
      Serial.println(message);
      return;
      break;
    case WL_NO_SSID_AVAIL:
      snprintf(message, 100, "No SSID are available for %s !", SSID);
      Serial.println(message);
      return;
      break;
    case WL_IDLE_STATUS:
      Serial.println("Retry to connect to WiFi !");
      break;
    default:
      snprintf(message, 100, "Error during WiFi connection [%i] !", WiFi.status());
      Serial.println(message);
      return;
      break;
    }
  }
}

// Setup
void setup() {
  Serial.begin(9600);
  Serial.println("ESP32 ready !");
  setup_wifi();
  setup_mqtt_broker();
  dht.begin();
  setup_mq135();
}

// Main loop
void loop() {
  // Wait for sensor
  delay(2000);

  // Check if Wi-Fi is connected
  while (WiFi.status() != WL_CONNECTED)
  {
    setup_wifi();
    delay(1000);
  }

  // [DHT22]

  // Read humidity
  float humidity_data = dht.readHumidity();
  sensor_data = convert_float_to_string(humidity_data, sensor_data);
  try_publish("esp32/out/humidity", sensor_data);
  // Read temperature
  float temperature_data = dht.readTemperature();
  sensor_data = convert_float_to_string(temperature_data, sensor_data);
  try_publish("esp32/out/temperature", sensor_data);

  // [MQ135]

  MQ135.update();

  // Get CO
  MQ135.setA(605.18); MQ135.setB(-3.937);
  float co = MQ135.readSensor();
  sensor_data = convert_float_to_string(co, sensor_data);
  try_publish("esp32/out/co", sensor_data);

  // Get Alcohol
  MQ135.setA(77.255); MQ135.setB(-3.18);
  float alcohol = MQ135.readSensor();
  sensor_data = convert_float_to_string(alcohol, sensor_data);
  try_publish("esp32/out/alcohol", sensor_data);

  // Get co2
  MQ135.setA(110.47); MQ135.setB(-2.862);
  float co2 = MQ135.readSensor() + 400;
  sensor_data = convert_float_to_string(co2, sensor_data);
  try_publish("esp32/out/co2", sensor_data);

  // Get Toluen
  MQ135.setA(44.947); MQ135.setB(-3.445);
  float toluen = MQ135.readSensor();
  sensor_data = convert_float_to_string(toluen, sensor_data);
  try_publish("esp32/out/toluen", sensor_data);

  // Get NH4
  MQ135.setA(102.2 ); MQ135.setB(-2.473);
  float nh4 = MQ135.readSensor();
  sensor_data = convert_float_to_string(nh4, sensor_data);
  try_publish("esp32/out/nh4", sensor_data);

  // Get Aceton
  MQ135.setA(34.668); MQ135.setB(-3.369);
  float aceton = MQ135.readSensor();
  sensor_data = convert_float_to_string(aceton, sensor_data);
  try_publish("esp32/out/aceton", sensor_data);

  // [BMP180]

  // Air pressure
  float pressure = bmp180.readPressure();
  sensor_data = convert_float_to_string(pressure, sensor_data);
  try_publish("esp32/out/pressure", sensor_data);

  // Altitude (considering that sea level has pressure of 10132 Pascal)
  float altitude = bmp180.readAltitude();
  sensor_data = convert_float_to_string(altitude, sensor_data);
  try_publish("esp32/out/altitude", sensor_data);
}
