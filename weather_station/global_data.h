#ifndef GLOBAL_DATA_H
#define GLOBAL_DATA_H

#define DHT_PIN 23
#define DHT_TYPE DHT22
#define SSID "ssid"
#define WIFI_PWD "password"
#define MQTT_SERVER_IP "mqtt_ip"
#define MQTT_PORT 1883
#define MQTT_DEVICE_ID "my_device"

WiFiClient wifi_client;
PubSubClient client(wifi_client);

static DHT dht(DHT_PIN, DHT_TYPE);

#endif // GLOBAL_DATA_H
