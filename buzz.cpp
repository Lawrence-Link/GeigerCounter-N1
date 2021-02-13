#include <Arduino.h>
void toneSOS(){
  static int i;
  i++;
  /* Not a clever way to do task managing */
  /* BUT It works anyway */
  if (i == 50 || i == 60 || i == 70 || i == 130 || i == 140 || i == 150)
  tone(7, 1000, 60);
  if (i == 80 || i == 100 || i == 120)
  tone(7, 1000, 120);
  if (i > 260)
  i = 0;
}

void toneClick(){
  tone(7, 1000, 20);
}
