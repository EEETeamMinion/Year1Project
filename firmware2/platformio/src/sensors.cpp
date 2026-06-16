#include "sensors.hpp"
#include <string>

const byte ultrasoundPin = 5;
const byte hfePin = 13;

bool read_ultrasound() {
  const uint16_t reading = digitalRead(ultrasoundPin);
  return (reading == HIGH);
}

bool read_hfe() {
  const uint16_t reading = digitalRead(hfePin);
  return (reading == HIGH);
}

// String read_radio() {
//   String buf = "";
//   bool shouldRead = false;
//   while (Serial1.available()) {
//     const String c = Serial1.readString();
//     if (c == "#") {
//       shouldRead = true;
//     } else {
//       if (shouldRead) {
//         buf += c;
//       }

//       if (buf.length() == 3) {
//         return buf;
//       }
//     }
//   }
// }

String read_radio() {
  return Serial1.readString();
}