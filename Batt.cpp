/*
    ©LPD Lawrence Link 2021
    All rights reserved
    Geiger Counter N1
    Licensed under GNU General Public License v3.0
    -------CONTACT AT-------
    Lawrence-Link@outlook.com
*/

#include "global.h"
#include <Arduino.h>

void batt_init(){
#if BOARD_VER == 2
  pinMode(PIN_DIVIDER_ENABLE, OUTPUT); //   divider enable
  digitalWrite(PIN_DIVIDER_ENABLE, LOW); // normally switch it off.
#endif
#if BOARD_VER == 1
  pinMode(PIN_CHG_DETECT, INPUT_PULLUP); //charging detect only for ver1.
#endif
}

float GetBatteryVolt() {
#if (BOARD_VER == 1)
  return (analogRead(PIN_BATT_LVL) * 5.0) / 1024.0;
#endif 

#if (BOARD_VER == 2)
  return ((analogRead(PIN_BATT_LVL) * 5.0) / 1024.0) * 2;
#endif
}

bool isCharging(){
  return !(digitalRead(PIN_CHG_DETECT));
}

#if (BOARD_VER == 2)
void enableDivider(){
  digitalWrite(PIN_DIVIDER_ENABLE, HIGH);
}
void disableDivider(){
  digitalWrite(PIN_DIVIDER_ENABLE, LOW);
}
#endif
