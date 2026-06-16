#include <Arduino.h>
#include <Gauss.h>
#include <vector>
#include <string>

#include "comms.hpp"
#include "movement.hpp"
#include "sensors.hpp"


const byte interruptPin = 15;
//Corresponds to A1 pin on board
volatile long int pulseCount = 0;
unsigned long firstPulse;
bool rockdetermined = false, vectorsinitialised = false;
String rocktype;
std::vector<float> probabilities547;
std::vector<float> probabilities312;

bool store = false;
String buf = "";

//ISR
void pulseInterrupt() {
  pulseCount++;
  Serial.println("interrupt " + pulseCount);
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(600);
  delay(2000);

  // Comms-setup
  connect_wifi();
  connect_mqtt();

  publish_log("Connected to WiFi and MQTT!");

  // movement
  setup_movement_pins();


  // Infra-red setup
  pinMode(interruptPin, INPUT_PULLDOWN);
  pinMode(18, INPUT);
  //pulls pin to GND to avoid noise triggering false interrupts

  //attachInterrupt(digitalPinToInterrupt(interruptPin), pulseInterrupt, RISING);
  //triggers interrupt on rising edge
  Serial.println("Pulse counter started");
  firstPulse = millis();
  //define time rel. to start of loop()
}

void loop() {
  mqtt.poll();
  //Serial.println(digitalRead(6));
  //Serial.println("hello world");
  //Serial.println(analogRead(18));

  if (Serial1.available()) {

    // Keep reading as long as data is coming in and we haven't collected 3 chars yet
    while (Serial1.available() && buf.length() < 3) {
        char c = Serial1.read();

        // 1. If we haven't seen '#' yet, look for it
        if (!store) {
            if (c == '#') {
                store = true; // Found it! Start storing the next characters
            }
        }
        // 2. If we have already seen '#', store the next characters
        else {
            buf += c;
        }

        // Small delay to allow slow serial data to arrive in the buffer
        delay(2);
    }

    // 3. Once we have our 3 characters, print them out and reset
    if (buf.length() == 3) {
        Serial.print("Stored characters: ");
        Serial.println(buf);

        std::string standardString(buf.c_str());
        publish_sensor_data("radio", std::stoi(standardString));

        // Reset state for the next message
        buf = "";
        store = false;
    }
}

  if (mqtt.available()) {
    const String topic = mqtt.messageTopic();
    const String message = mqtt.readString();

    if (topic == mqtt_move_topic) {
      if (message == "measure") {
        Serial.println("MEASURE SENSORS NOW!!");
        //Serial.println(read_ultrasound());
        publish_sensor_data("hfe", read_hfe());
        publish_sensor_data("ultrasound", read_ultrasound());
        std::string standardString(buf.c_str());
        publish_sensor_data("radio", std::stoi(standardString));
        pulseCount = 0;
      } else {
        const char code = message[0];
        handle_movement_command(code);
      }
    }
  }

  unsigned long currentTime;
  const float upperconst = 1000.5;

  if(!rockdetermined){

    if(!vectorsinitialised){
      Gauss gauss547;
      Gauss gauss312;

      gauss547.begin(547, 23.388);
      gauss312.begin(312, 17.664);
      //standard dev = sqrt(mean) for Poisson distro

      for(float lowerbound = -0.5; lowerbound<upperconst; lowerbound++){
        float upperbound = lowerbound + 1;
        probabilities547.push_back(gauss547.P_between(lowerbound,upperbound));
      }

      for(float lowerbound = -0.5; lowerbound<upperconst; lowerbound++){
        float upperbound = lowerbound + 1;
        probabilities312.push_back(gauss312.P_between(lowerbound,upperbound));
      }
      //creating probability vectors
      vectorsinitialised = true;
    }

    currentTime = millis();

    //triggers only if x time has elapsed
    if (currentTime - firstPulse >= 1000){
      noInterrupts();
      long int finalPulseCount = pulseCount;
      pulseCount = 0;
      firstPulse = currentTime;
      //reset pulse count and change clock start time
      long int dscrtupperconst = (int)upperconst;


      if(finalPulseCount > 0 && finalPulseCount < dscrtupperconst){
        float prob547 = probabilities547[finalPulseCount];
        float prob312 = probabilities312[finalPulseCount];

        if(prob547>prob312){
            rocktype = "rock 547";
        }
        else{
            rocktype = "rock 312";
        }

        //checking statistical significance
        //3 standard dev rule, 99.73% of vals lie in this region
        if(rocktype == "rock 547"){
            if(finalPulseCount<617 && finalPulseCount>477){
                rockdetermined = true;
            }
        }
        else{
            if(finalPulseCount<365 && finalPulseCount>260){
                rockdetermined = true;
            }
        }

        if(rockdetermined){
            if (rocktype == "rock 547") {
              publish_sensor_data("infrared", 547);
            } else {
              publish_sensor_data("infrared", 312);
            }
        }
      }
      interrupts();
    }
  }
}