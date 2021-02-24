#ifndef _LEDS_H_
#define _LEDS_H_

enum LED_WARN {LED_GREEN, LED_YELLOW, LED_RED};

void led_initialize();
void led_blue(bool state);  
void led_red(bool state);
void led_green(bool state);
void led_flash(LED_WARN _buf);

#endif
