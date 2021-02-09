#include <Arduino.h>

void led_initialize(){
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  /* Turn all lights off(Common Anode) */
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
}

void led_blue(bool state){
  digitalWrite(A2, !state);
}

void led_red(bool state){
  digitalWrite(A0, !state);
}

void led_green(bool state){
  digitalWrite(A1, !state);
}
