#include "DHT.h"
#include "WiFi.h"
#include <stdlib.h>
#include "PubSubClient.h"
#include <MQUnifiedsensor.h>

#include "global_data.h"

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

// Try to publish data into topic
void try_publish(char *topic, const char *data)
{
  if (!client.connected())
  {
    reconnect();
  }
  if (!client.publish(topic, data))
  {
    Serial.print("Failed to publish into ");
    Serial.println(topic);
  }
  else
  {
    Serial.print("Successfully publish ");
    Serial.print(data);
    Serial.print(" into ");
    Serial.println(topic);
  }
}

// Mqtt callback
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.println("Message received !");
}

// Reconnect ESP32 to the mqtt server
void reconnect()
{
  if (!client.connect(MQTT_DEVICE_ID))
  {
    Serial.println("Failed to connect to MQTT broker !");
    Serial.println(client.state());
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

// Setup mqtt broker
void setup_mqtt_broker()
{
  client.setServer(MQTT_SERVER_IP, MQTT_PORT);
  client.setCallback(callback);
}

// Connect ESP32 to a WiFi hotspot
void setup_wifi()
{
  // Set connection to access point
  WiFi.mode(WIFI_STA);

  while (true)
  {
    // Try to connect to ssid with given password
    WiFi.begin(SSID, WIFI_PWD);

    delay(1000);
    Serial.println("Trying to connect to WiFi ...");

    // Check WiFi status
    switch (WiFi.status())
    {
    case WL_CONNECTED:
      Serial.print("Successfully connected to ");
      Serial.print(SSID);
      Serial.println(" WiFi");
      return;
      break;
    case WL_NO_SHIELD:
      Serial.println("No WiFi shield present !");
      return;
      break;
    case WL_CONNECT_FAILED:
      Serial.print("Failed to connect to ");
      Serial.print(SSID);
      Serial.println(" WiFi");
      return;
      break;
    case WL_NO_SSID_AVAIL:
      Serial.print("No SSID are available for ");
      Serial.println(SSID);
      return;
      break;
    case WL_IDLE_STATUS:
      Serial.println("Retry to connect to WiFi !");
      break;
    default:
      Serial.print("Error during WiFi connection ");
      Serial.println(WiFi.status());
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

  // Check if WiFi is connected
  while (WiFi.status() != WL_CONNECTED)
  {
    setup_wifi();
    delay(1000);
  }

  // Read humidity
  float humidity_data = dht.readHumidity();
  // Read temperature
  float temperature_data = dht.readTemperature();

  // Try to publish data into mqtt topics
  sensor_data = convert_float_to_string(temperature_data, sensor_data);
  try_publish("esp32/out/temperature", sensor_data);

  sensor_data = convert_float_to_string(humidity_data, sensor_data);
  try_publish("esp32/out/humidity", sensor_data);

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
}
