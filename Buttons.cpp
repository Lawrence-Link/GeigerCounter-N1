#include "Buttons.h"
#include <Arduino.h>

enum {up_btn = 10, ok_btn = 9, dn_btn = 8} button;
enum {Short, Long} OK_BTN_STATE;
bool buttonActiveOK = false; //needed to been accessable in the main loop
bool longPressActive = false;
#define longPressTime 200

void buttons_init() {
  pinMode(up_btn, INPUT_PULLUP);
  pinMode(ok_btn, INPUT_PULLUP);
  pinMode(dn_btn, INPUT_PULLUP);
}

buttonReturnDef refresh_button() {
  static long buttonTimer = 0;

  buttonReturnDef _buf;
  
  static bool buttonActiveUP = false;
  static bool buttonActiveDN = false;
  
  if (digitalRead(up_btn) == LOW){
    buttonActiveUP = true;
  }else if (digitalRead(up_btn) == HIGH && buttonActiveUP == true){
    buttonActiveUP = false;
    _buf = UPPER;
  }
  
  else if (digitalRead(dn_btn) == LOW){
    buttonActiveDN = true;
  }else if (digitalRead(dn_btn) == HIGH && buttonActiveDN == true){
    buttonActiveDN = false;
    _buf = LOWER;
  }
  
  else if (digitalRead(ok_btn) == LOW) {
    if (buttonActiveOK == false) {
      buttonActiveOK = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      longPressActive = true;
      // Long press
      _buf = MID_LONG;
    }
  } else if(digitalRead(ok_btn) == HIGH) {
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
