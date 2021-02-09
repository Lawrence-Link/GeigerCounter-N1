#include <Arduino.h>
void toneSOS(){
  static int i;
  i++;
  /* Not a clever way to do task managing */
  /* BUT It works anyway */
  if (i == 50 || i == 70 || i == 90 || i == 220 || i == 240 || i == 260)
  tone(7, 1000, 60);
  if (i == 110 || i == 150 || i == 190)
  tone(7, 1000, 120);
  if (i > 260)
  i = 0;
}

void toneClick(){
  tone(7, 1000, 20);
}
