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
#define TIME_ELAPSE_COEFFICIENT 0.8
void toneSOS(){
  static int i;
  i++;
  /* Not a clever way to do task managing */
  /* BUT It works anyway */
  if (i == TIME_ELAPSE_COEFFICIENT * 50 || i == TIME_ELAPSE_COEFFICIENT * 60 || i == TIME_ELAPSE_COEFFICIENT * 70 || i == TIME_ELAPSE_COEFFICIENT * 130 || i == TIME_ELAPSE_COEFFICIENT * 140 || i == TIME_ELAPSE_COEFFICIENT * 150)
  tone(PIN_BUZZER, 1000, 60); // shorter beep
  if (i == TIME_ELAPSE_COEFFICIENT * 80 || i == TIME_ELAPSE_COEFFICIENT * 100 || i == TIME_ELAPSE_COEFFICIENT * 120)
  tone(PIN_BUZZER, 1000, 120); // longer beep
  if (i > TIME_ELAPSE_COEFFICIENT * 260)
  i = 0;
}

void toneClick(){
  tone(PIN_BUZZER, 1000, 15);
}
