#include <Arduino.h>
#include <Gauss.h>
#include <vector>

const byte interruptPin = 15; 
//Corresponds to A1 pin on board
volatile long int pulseCount = 0;
unsigned long firstPulse;
bool rockdetermined = false, vectorsinitialised = false;
String rocktype;
std::vector<float> probabilities547;
std::vector<float> probabilities312;


//ISR
void pulseInterrupt() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  pinMode(interruptPin, INPUT_PULLDOWN); 
  //pulls pin to GND to avoid noise triggering false interrupts
  
  attachInterrupt(digitalPinToInterrupt(interruptPin), pulseInterrupt, RISING);
  //triggers interrupt on rising edge
  Serial.println("Pulse counter started");
  firstPulse = millis();
  //define time rel. to start of loop()
}

void loop(){
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
            Serial.println(rocktype);
        }
      }

      interrupts();

    }
  }
}