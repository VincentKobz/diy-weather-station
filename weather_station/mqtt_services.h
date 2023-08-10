#ifndef MQTT_SERVICES_H
#define MQTT_SERVICES_H

void try_publish(const char *topic, const char *data);
void try_publish_float(const char *topic, float data);
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

#endif /* MQTT_SERVICES_H */
