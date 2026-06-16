#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <vector>

const int left_dir_pin = 2;
const int left_pwm_pin = 3;
const int right_dir_pin = 8;
const int right_pwm_pin = 9;

extern std::vector<int> wasd_array;

void move(const int left, const int right);
void handle_movement_command(char code);
void setup_movement_pins();

#endif