#include "DHT.h"
#include "WiFi.h"
#include "PubSubClient.h"

#include "global_data.h"

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
  char temperature[5];
  snprintf(temperature, 5, "%f", temperature_data);

  char humidity[5];
  snprintf(humidity, 5, "%f", humidity_data);

  try_publish("esp32/out/temperature", temperature);
  try_publish("esp32/out/humidity", humidity);
}
