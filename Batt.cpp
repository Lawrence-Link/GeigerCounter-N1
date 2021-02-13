#include <Arduino.h>

void batt_init(){
  pinMode(4, INPUT_PULLUP);
}

float GetBatteryVolt() {
  return (analogRead(A7) * 5.0) / 1024.0;
}

bool isCharging(){
  return !(digitalRead(4));
}
