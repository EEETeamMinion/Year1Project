#include <Arduino.h>
#include <WiFi101.h>
#include <ArduinoMqttClient.h>
#include <vector>
#include <algorithm>
#include <math.h>



WiFiClient wifi;
MqttClient mqtt(wifi);

const int left_dir_pin = 2;
const int left_pwm_pin = 3;
const int right_dir_pin = 8;
const int right_pwm_pin = 9;

std::vector<int> wasd_array = {0,0,0,0};

const char* network_ssid = "Violakii";
const char* network_password = "88888888";

const char* mqtt_broker = "broker.hivemq.com";
const uint16_t mqtt_port = 1883;
const char* mqtt_move_topic = "imperialeeet4/move";
const String mqtt_sensor_topic = "imperialeeet4/sensors";


void move(const int left, const int right){
  digitalWrite(left_dir_pin, (left >0));
  digitalWrite(right_dir_pin, (right<0));
  analogWrite(left_pwm_pin,abs(left));
  analogWrite(right_pwm_pin,abs(right));
}

void handle_movement_command(char code) {
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
  int value;
  if (code == 'w'||code == 'a'||code == 's'||code == 'd'){
    value = 1;
  }
  else{
    code = char(code-1);
    value = 0;
  }

  switch(code){
    case 'w':
      wasd_array[0] = value;
      break;
    case 'a':
      wasd_array[1] = value;
      break;
    case 's':
      wasd_array[2] = value;
      break;
    case 'd':
      wasd_array[3] = value;
      break;
    default:
      break;
  }

  const int throttle = wasd_array[2] - wasd_array[0];
  const int turn = (throttle <= 0 ? 1:-1)*(wasd_array[1] - wasd_array[3]); //steering of car is mirrored in backward motion
  const double turn_factor = 0.4;
  double left  = throttle + turn_factor * turn;
  double right = throttle - turn_factor * turn;
  double max_val = max(abs(left),abs(right));
  if (max_val!=0){
    left = 255*left/max_val;
    right = 255*right/max_val;
  }
  else{
    left = right = 0;
  }

  move(left,right);
  

  Serial.print("Current motion_array: ");
  if (!wasd_array.empty()){
    for (int x : wasd_array) {
        Serial.print(int(x));
    }
  }
  Serial.println("");
  Serial.print("Now executing: ");
  Serial.print(left);
  Serial.print(" ");
  Serial.print(right);
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
