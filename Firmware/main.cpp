#include <Arduino.h>
#include <WiFi101.h>
#include <ArduinoMqttClient.h>
#include <vector>
#include <algorithm>



WiFiClient wifi;
MqttClient mqtt(wifi);

const int left_dir_pin = 2;
const int left_pwm_pin = 3;
const int right_dir_pin = 8;
const int right_pwm_pin = 9;

std::vector<char> motion_array = {};

const char* network_ssid = "Violakii";
const char* network_password = "88888888";

const char* mqtt_broker = "broker.hivemq.com";
const uint16_t mqtt_port = 1883;
const char* mqtt_move_topic = "imperialeeet4/move";
const String mqtt_sensor_topic = "imperialeeet4/sensors";


void move(const int left_dir, const int left_pwm, const int right_dir, const int right_pwm){
  digitalWrite(left_dir_pin, left_dir);
  digitalWrite(right_dir_pin, right_dir);
  analogWrite(left_pwm_pin,left_pwm);
  analogWrite(right_pwm_pin,right_pwm);
}

void handle_movement_command(const char code) {
  // Message Commands:
  // w press: w
  // w release : x
  // a press : a
  // a release : b
  // s press: s
  // s release : t
  // d press: d
  // d release: e
  Serial.print("Received movement command: ");
  Serial.println(code);
  if (code == 'w'||code == 'a'||code == 's'||code == 'd'){
    if (std::find(motion_array.begin(), motion_array.end(), code) == motion_array.end()){
      motion_array.push_back(code);
    }
  }

  else{
    char remove_code = char(code-1);
    motion_array.erase(remove(motion_array.begin(), motion_array.end(), remove_code), motion_array.end());
  }

  Serial.print("Current motion_array: ");
  if (!motion_array.empty()){
    for (int x : motion_array) {
        Serial.print(char(x));
    }
  }
  Serial.println("");
  Serial.print("Now executing: ");
  Serial.println(motion_array.back());

  switch(motion_array.back()){
    case 'w':
      move(HIGH,255,LOW,255);
      break;
    case 'a':
      move(HIGH,255,HIGH,255);
      break;
    case 's':
      move(LOW,255,HIGH,255);
      break;
    case 'd':
      move(LOW,255,LOW,255);
      break;
    default:
      move(LOW,0,LOW,0);
      break;
  }
}

void publish_sensor_data(const String sensor, const float reading) {
  mqtt.beginMessage(mqtt_sensor_topic + '/' + sensor);
  mqtt.print(reading);
  mqtt.endMessage();
}

void setup() {
  delay(2500);
  Serial.begin(9600);
  delay(2500);

  Serial.println("Attempting to connect to wifi...");
  WiFi.begin(network_ssid, network_password);
  while(WiFi.status() != WL_CONNECTED) { 
    delay(250); 
    Serial.print(".");
  };
  Serial.println("Connected to WiFi");

  const bool connected = mqtt.connect(mqtt_broker, mqtt_port);
  if (!connected) {
    Serial.print("MQTT Connection Error:");
    Serial.println(mqtt.connectError());
  } else {
    Serial.println("Successfully connected to HiveMQ broker!");
  }

  mqtt.subscribe(mqtt_move_topic);

  pinMode(left_dir_pin, OUTPUT);
  pinMode(left_pwm_pin, OUTPUT);
  pinMode(right_dir_pin, OUTPUT);
  pinMode(right_pwm_pin, OUTPUT);

  digitalWrite(left_dir_pin,LOW);
  analogWrite(left_pwm_pin, 0);
  digitalWrite(right_dir_pin,LOW);
  analogWrite(right_pwm_pin, 0);
}



void loop() {
  mqtt.poll();

  if (mqtt.available()) {
    const String topic = mqtt.messageTopic();
    const String message = mqtt.readString();

    if (topic == mqtt_move_topic) {
      const char code = message[0];
      handle_movement_command(code);
    }
  }

  const float infra_red_sensors = 123;
  const float magnetic_sensors = 245.6;
  const float radio_sensor = 111.11;
  publish_sensor_data("infra_red", infra_red_sensors);
  publish_sensor_data("magnetic", magnetic_sensors);
  publish_sensor_data("radio", radio_sensor);
  delay(50);
}
