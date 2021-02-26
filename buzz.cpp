#include <Arduino.h>
void toneSOS(){
  static int i;
  i++;
  /* Not a clever way to do task managing sound like this ... --- ... */
  /* BUT It works anyway */
  if (i == 50 || i == 60 || i == 70 || i == 130 || i == 140 || i == 150)
  tone(7, 1000, 60); // shorter beep
  if (i == 80 || i == 100 || i == 120)
  tone(7, 1000, 120); // longer beep
  if (i > 260)
  i = 0;
}

void toneClick(){
  tone(7, 2000, 15);
}
