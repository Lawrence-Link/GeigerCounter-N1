#include "Buttons.h"
#include <Arduino.h>

enum {up_btn = 10, ok_btn = 9, dn_btn = 8} button;
enum {Short, Long} OK_BTN_STATE;

#define longPressTime 200

void buttons_init() {
  pinMode(up_btn, INPUT_PULLUP);
  pinMode(ok_btn, INPUT_PULLUP);
  pinMode(dn_btn, INPUT_PULLUP);
}

buttonReturnDef refresh_button() {
  static long buttonTimer = 0;
  static boolean buttonActive = false;
  static boolean longPressActive = false;
  buttonReturnDef _buf;
  
  if (digitalRead(up_btn) == LOW){
    _buf = UPPER;
  } else if (digitalRead(dn_btn) == LOW){
    _buf = LOWER;
  } else if (digitalRead(ok_btn) == LOW) {
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      longPressActive = true;
      // Long press
      _buf = MID_LONG;
    }
  } else if(digitalRead(ok_btn) == HIGH) {
    if (buttonActive == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {
        // short_press
        _buf = MID_SHORT;
      }
      buttonActive = false;
    }
  }else {
    _buf = NONE;
  }
  return _buf;
}
