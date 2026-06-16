#ifndef COMMS_H
#define COMMS_H

#include <Arduino.h>
#include <WiFi101.h>
#include <ArduinoMqttClient.h>

extern WiFiClient wifi;
extern MqttClient mqtt;

extern const char *network_ssid;
extern const char *network_password;
extern const char *mqtt_broker;
extern const uint16_t mqtt_port;
extern const char *mqtt_move_topic;
extern const String mqtt_sensor_topic;

void connect_wifi();
void connect_mqtt();
void publish_sensor_data(const char* sensor, const int reading);
void publish_log(const char* log);

#endif