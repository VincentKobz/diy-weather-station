#include "mqtt_services.h"
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

// Read and publish data from DHT22
void get_dht22_data()
{
  // Read humidity
  try_publish_float("esp32/out/humidity", dht.readHumidity());
  // Read temperature
  try_publish_float("esp32/out/temperature", dht.readTemperature());
}

// Read and publish data from MQ135
void get_mq135_data()
{
  MQ135.update();

  // Get CO
  MQ135.setA(605.18); MQ135.setB(-3.937);
  try_publish_float("esp32/out/co", MQ135.readSensor());

  // Get Alcohol
  MQ135.setA(77.255); MQ135.setB(-3.18);
  try_publish_float("esp32/out/alcohol", MQ135.readSensor());

  // Get co2
  MQ135.setA(110.47); MQ135.setB(-2.862);
  float co2_value = MQ135.readSensor() + 400;
  get_air_quality(co2_value);
  try_publish_float("esp32/out/co2", co2_value);

  // Get Toluen
  MQ135.setA(44.947); MQ135.setB(-3.445);
  try_publish_float("esp32/out/toluen", MQ135.readSensor());

  // Get NH4
  MQ135.setA(102.2 ); MQ135.setB(-2.473);
  try_publish_float("esp32/out/nh4", MQ135.readSensor());

  // Get Aceton
  MQ135.setA(34.668); MQ135.setB(-3.369);
  try_publish_float("esp32/out/aceton", MQ135.readSensor());
}

// Read and publish data from BMP180
void get_bmp180_data()
{
  // Air pressure
  try_publish_float("esp32/out/pressure", bmp180.readPressure());
  // Altitude (considering that sea level has pressure of 10132 Pascal)
  try_publish_float("esp32/out/altitude", bmp180.readAltitude());
}
