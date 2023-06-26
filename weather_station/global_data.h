#ifndef GLOBAL_DATA_H
#define GLOBAL_DATA_H

#define DHT_PIN 23
#define DHT_TYPE DHT22

const char *ssid = "ssid";
const char *password = "password";

static DHT dht(DHT_PIN, DHT_TYPE);
static WiFiClient wifi_client;

#endif // GLOBAL_DATA_H
