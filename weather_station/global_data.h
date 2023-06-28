#ifndef GLOBAL_DATA_H
#define GLOBAL_DATA_H

#define DHT_PIN 23
#define DHT_TYPE DHT22
#define SSID "ssid"
#define WIFI_PWD "password"

WiFiClient wifi_client;

static DHT dht(DHT_PIN, DHT_TYPE);

#endif // GLOBAL_DATA_H
