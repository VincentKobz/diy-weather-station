#include "global_data.h"

// Try to publish data into MQTT topic
void try_publish(const char *topic, const char *data)
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

void try_publish_float(const char *topic, float data)
{
  sensor_data = convert_float_to_string(data, sensor_data);
  try_publish(topic, sensor_data);
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
