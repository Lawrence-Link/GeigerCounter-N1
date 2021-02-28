/*
    ©LPD Lawrence Link 2021
    All rights reserved
    Geiger Counter N1
    Licensed under GNU General Public License v3.0
    -------CONTACT AT-------
    Lawrence-Link@outlook.com
*/

#include <Arduino.h>
#include "leds.h"
#include "global.h"
void led_initialize(){
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);
  /* Turn all lights off(Common Anode) */
  digitalWrite(PIN_LED_RED, HIGH);
  digitalWrite(PIN_LED_GREEN, HIGH);
  digitalWrite(PIN_LED_BLUE, HIGH);
}

void led_blue(bool state){
  digitalWrite(PIN_LED_BLUE, !state);
}

void led_red(bool state){
  digitalWrite(PIN_LED_RED, !state);
}

void led_green(bool state){
  digitalWrite(PIN_LED_GREEN, !state);
}

void led_flash(LED_WARN _buf){
  switch (_buf){
    case LED_GREEN : {
      led_green(true);
      led_red(false);
      led_blue(false);
      
      led_green(false);
      led_red(false);
      led_blue(false);
      break;
    }
    case LED_YELLOW : {
      led_green(true);
      led_red(true);
      led_blue(false);
      
      led_green(false);
      led_red(false);
      led_blue(false);
      break;
    }
    case LED_RED : {
      led_green(false);
      led_red(true);
      led_blue(false);
      
      led_green(false);
      led_red(false);
      led_blue(false);
      break;
    }
  }
}
