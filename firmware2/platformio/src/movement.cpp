#include "movement.hpp"
#include <algorithm>
#include <math.h>
#include <Arduino.h>

std::vector<int> wasd_array = {0, 0, 0, 0};

void move(const int left, const int right) {
  digitalWrite(left_dir_pin, (left >0));
  digitalWrite(right_dir_pin, (right<0));
  analogWrite(left_pwm_pin,abs(left));
  analogWrite(right_pwm_pin,abs(right));
}

void handle_movement_command(char code)
{
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
  if (code == 'w' || code == 'a' || code == 's' || code == 'd')
  {
    value = 1;
  }
  else
  {
    code = char(code - 1);
    value = 0;
  }

  switch (code)
  {
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
  const int turn = (throttle <= 0 ? 1 : -1) * (wasd_array[1] - wasd_array[3]); // steering of car is mirrored in backward motion
  const double turn_factor = 0.4;
  double left = throttle + turn_factor * turn;
  double right = throttle - turn_factor * turn;
  double max_val = max(abs(left), abs(right));
  if (max_val != 0)
  {
    left = 255 * left / max_val;
    right = 255 * right / max_val;
  }
  else
  {
    left = right = 0;
  }

  move(left, right);

  Serial.print("Current motion_array: ");
  if (!wasd_array.empty())
  {
    for (int x : wasd_array)
    {
      Serial.print(int(x));
    }
  }
  Serial.println("");
  Serial.print("Now executing: ");
  Serial.print(left);
  Serial.print(" ");
  Serial.print(right);
}

void setup_movement_pins() {
  pinMode(left_dir_pin, OUTPUT);
  pinMode(left_pwm_pin, OUTPUT);
  pinMode(right_dir_pin, OUTPUT);
  pinMode(right_pwm_pin, OUTPUT);

  digitalWrite(left_dir_pin,LOW);
  analogWrite(left_pwm_pin, 0);
  digitalWrite(right_dir_pin,LOW);
  analogWrite(right_pwm_pin, 0);
};