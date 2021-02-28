/*
    ©LPD Lawrence Link 2021
    All rights reserved
    Geiger Counter N1
    Licensed under GNU General Public License v3.0
    -------CONTACT AT-------
    Lawrence-Link@outlook.com
*/

#include "global.h"
#include "Buttons.h"
#include <Arduino.h>

enum {Short, Long} PIN_BUTTON_OK_STATE;
bool buttonActiveOK = false; //needed to been accessable in the main loop
bool longPressActive = false;
#define longPressTime 200

void buttons_init() {
  pinMode(PIN_BUTTON_UP, INPUT_PULLUP);
  pinMode(PIN_BUTTON_OK, INPUT_PULLUP);
  pinMode(PIN_BUTTON_DN, INPUT_PULLUP);
}

buttonReturnDef refresh_button() {
  static long buttonTimer = 0;

  buttonReturnDef _buf;
  
  static bool buttonActiveUP = false;
  static bool buttonActiveDN = false;
  
  if (digitalRead(PIN_BUTTON_UP) == LOW){
    buttonActiveUP = true;
  }else if (digitalRead(PIN_BUTTON_UP) == HIGH && buttonActiveUP == true){
    buttonActiveUP = false;
    _buf = UPPER;
  }
  
  else if (digitalRead(PIN_BUTTON_DN) == LOW){
    buttonActiveDN = true;
  }else if (digitalRead(PIN_BUTTON_DN) == HIGH && buttonActiveDN == true){
    buttonActiveDN = false;
    _buf = LOWER;
  }
  
  else if (digitalRead(PIN_BUTTON_OK) == LOW) {
    if (buttonActiveOK == false) {
      buttonActiveOK = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      longPressActive = true;
      // Long press
      _buf = MID_LONG;
    }
  } else if(digitalRead(PIN_BUTTON_OK) == HIGH) {
    if (buttonActiveOK == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {
        // short_press
        _buf = MID_SHORT;
      }
      buttonActiveOK = false;
    }
  }else {
    _buf = NONE;
  }
  return _buf;
}
