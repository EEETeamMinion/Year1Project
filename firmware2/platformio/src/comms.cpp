#include "comms.hpp"
#include <WiFi101.h>
#include <ArduinoMqttClient.h>

WiFiClient wifi;
MqttClient mqtt(wifi);

const char *network_ssid = "raspi";
const char *network_password = "123456789";
const char* mqtt_broker = "broker.hivemq.com";
const uint16_t mqtt_port = 1883;
const char* mqtt_move_topic = "imperialeeet4/move";
const String mqtt_sensor_topic = "imperialeeet4/sensors";
const String mqtt_logs_topic = "imperialeeet4/logs";

void connect_wifi() {
  Serial.println("Attempting to connect to wifi");
  WiFi.begin(network_ssid, network_password);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  };
  Serial.println("Connected to WiFi!");
}

void connect_mqtt() {
  const bool connected = mqtt.connect(mqtt_broker, mqtt_port);
  if (!connected) {
    Serial.print("MQTT Connection Error:");
    Serial.println(mqtt.connectError());
  } else {
    Serial.println("Successfully connected to HiveMQ broker!");
  }

  mqtt.subscribe(mqtt_move_topic);
}

void publish_sensor_data(const char* sensor, const int reading) {
  mqtt.beginMessage(mqtt_sensor_topic + "/" + sensor);
  mqtt.print(reading);
  mqtt.endMessage();
}

void publish_log(const char* log) {
  mqtt.beginMessage(mqtt_logs_topic);
  mqtt.print(log);
  mqtt.endMessage();
}