#include <Arduino.h>

float GetBatteryVolt() {
  return (analogRead(A7) * 5.0) / 1024.0;
}
