#ifndef GLOBAL_DATA_H
#define GLOBAL_DATA_H

// DHT22
#define DHT_PIN 23
#define DHT_TYPE DHT22

// Wi-Fi
#define SSID "ssid"
#define WIFI_PWD "password"

// MQTT
#define MQTT_SERVER_IP "mqtt_ip"
#define MQTT_PORT 1883
#define MQTT_DEVICE_ID "my_device"

// MQ135
#define BOARD ("ESP-32")
#define VOLTAGE_RESOLUTION 3.3
#define PIN 39
#define TYPE "MQ-135"
#define ADC_BIT_RESOLUTION 12
#define RATIO_MQ135_CLEAN_AIR 3.6

WiFiClient wifi_client;
PubSubClient client(wifi_client);
MQUnifiedsensor MQ135(BOARD, VOLTAGE_RESOLUTION, ADC_BIT_RESOLUTION, PIN, TYPE);

static DHT dht(DHT_PIN, DHT_TYPE);
static char *sensor_data = NULL;
static Adafruit_BMP085 bmp180;

#endif // GLOBAL_DATA_H
