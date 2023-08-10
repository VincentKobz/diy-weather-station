#include "global_data_2.h"
#include "sensor_data.h"
#include <stdlib.h>
#include <Wire.h>

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

  while (!bmp180.begin())
  {
    Serial.println("Bmp180 not find !");
  }
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
  get_dht22_data();

  // [MQ135]
  get_mq135_data();

  // [BMP180]
  get_bmp180_data();
}
