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

#include "Batt.h"
#include "buzz.h"
#include "leds.h"
#include "Buttons.h"
#include "resources.h"
#include <Wire.h>
#include <U8g2lib.h>
#include <EEPROM.h>
#include <TimerOne.h>
#include <avr/wdt.h>  // AVR-GCC lib to enable the Watchdog Timer

/* Global variants' definations */
bool IsRunning = true;
bool IsRELModeOn = false;
enum menuSTG{START, COUNT, SETTINGS, BRIGHTNESS, SOUND} menu;
enum soundfx{SOS, ON, OFF} SoundEffect;
enum buttonReturnDef;
enum {addrBRI = 0, addrSOUND = 10};

/* declarations */
float outputSieverts(float x);
void sensorISR();
void ISR_Timer1();

unsigned long counts = 0; //variable for GM Tube events, +1 when every pulse occur
float averageCPM = 0; // average count per minutes.
int currentCPM = 0;
float calcCPM = 0;
float CPMArray[100];
float REF_REFERENCE = 0;

volatile short tm1_counter = 0;
volatile float usvHr = 0;

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup(void) {
  u8g2.begin();
  pinMode(2, INPUT_PULLUP); // pin 2(INT0) PULLUP

  batt_init();
  led_initialize();
  buttons_init();
  menu = START;
  attachInterrupt(digitalPinToInterrupt(PIN_GEIGER), sensorISR, FALLING); //Detect pulse at the falling edge
  Timer1.initialize(1000000); // 1s interrupt
  Timer1.attachInterrupt(ISR_Timer1);
  u8g2.setContrast(EEPROM.read(0));
  SoundEffect = soundfx(EEPROM.read(10));

  wdt_enable(WDTO_2S); // ENABLE WATCHDOG TIMER, NEEDED TO BE FED EVERY 2 SECONDS.
}

/*
  #ifdef DEBUG
        static bool led_debug = false;
        led_debug = !led_debug;
        led_red(led_debug);
  #endif
*/

/* * * * * * * * * * * * * * * * * * The UI initially I i thought was
   LPD |■■■ | 70%        RUNNING  * something like this. Logo up left
 *                                 * and battery usage followed. It
   ---   ---   |  |  |----  |  |   * also has the freezing mode that can
   | |   | |   |  |  |---|  |  |   * be useless in a geiger counter
   | | . ---   |--   ----|  |__|   * anyway.  By holding the center
   ---   ---   |                   * button, you can enter the setting
                          🔈    β/γ * screen, which you can adjust the bright-
 * * * * * * * * * * * * * * * * * *-ness, and also the sound effect,
  something like SOS alarm and typical geiger clicks.
*/


void drawTitle(char* title) {
  u8g2.setFont(u8g2_font_luBIS08_tf);
  u8g2.drawStr(0, 9, "LPD");
  u8g2.drawLine(0, 11, 127, 11); // Upper dividing line
  u8g2.drawLine(26, 0, 26, 11);  // dividing line between 'LPD' & others
  u8g2.setFont(u8g2_font_saikyosansbold8_8u);
  u8g2.drawStr(29, 9, title);
}

void drawUICounting( float endResult ) { // When Counter works..
  u8g2.setFont(u8g2_font_luBIS08_tf);
  u8g2.drawLine(0, 11, 127, 11); // Upper dividing line
  u8g2.drawStr(0, 10, "LPD");
  u8g2.drawLine(0, 0, 127, 0);
  u8g2.drawLine(26, 0, 26, 11);
  u8g2.drawLine(44, 0, 44, 11);
  u8g2.drawLine(44, 0, 44, 11);
#if (BOARD_VER == 2)
  enableDivider();
#endif
  // Batt-icon determine
  if (GetBatteryVolt() < 3.6) {
    u8g2.drawXBMP(29, 3, BATT_ERR_WIDTH, BATT_ERR_HEIGHT, BATT_ERR);
  } else if (GetBatteryVolt() < 3.8) {
    u8g2.drawXBMP(29, 3, BATT_25_WIDTH, BATT_25_HEIGHT, BATT_25);
  } else if (GetBatteryVolt() < 3.9) {
    u8g2.drawXBMP(29, 3, BATT_75_WIDTH, BATT_75_HEIGHT, BATT_75);
  } else if (GetBatteryVolt() >= 3.9) {
    u8g2.drawXBMP(29, 3, BATT_100_WIDTH, BATT_100_HEIGHT, BATT_100);
  }
#if (BOARD_VER == 2)
  disableDivider();
#endif
  u8g2.setFont(u8g2_font_timB08_tf);
  char _str_ans[10];
  char _str_avr_ans[10];
  
  if (IsRunning == true) { // Showing running state
    u8g2.drawStr(75, 10, "RUNNING");
    if (IsRELModeOn){
      dtostrf(usvHr - REF_REFERENCE, 2, 2, _str_ans);
    } else {
      dtostrf(usvHr, 2, 2, _str_ans);
    }
    dtostrf(outputSieverts(averageCPM), 2, 2, _str_avr_ans);
  } else {
    u8g2.drawStr(75, 10, " PAUSED");
  }
#if (BOARD_VER == 1)
  if (isCharging()) { // if is charging, then draw
    u8g2.drawStr(36, 63, "CHG");
    u8g2.drawLine(0, 53, 60, 53); // Lower dividing line
    u8g2.drawLine(60, 53, 60, 63);
  } else {
    u8g2.drawLine(0, 53, 33, 53); // Lower dividing line
    u8g2.drawLine(33, 53, 33, 63);
  }
#endif

#if (BOARD_VER == 2)
  u8g2.drawLine(0, 53, 33, 53); // Lower dividing line
  u8g2.drawLine(33, 53, 33, 63);
#endif

  char _str_timing_buffer[5]; // ..xx[\a]
  sprintf(_str_timing_buffer, "..%d", 15 - tm1_counter);
  u8g2.drawStr(53, 10, _str_timing_buffer); // Timing left

  if (IsRELModeOn){
    u8g2.drawStr(79, 55, "REL:REF:");
    u8g2.drawXBMP(63, 49, DELTA_WIDTH, DELTA_HEIGHT, DELTA_SIGN);
    char _str_REF_buffer[6];
    dtostrf(REF_REFERENCE, 2, 2, _str_REF_buffer);
    u8g2.drawStr(79, 63, _str_REF_buffer);
    //u8g2.drawStr(90, 80, "");
  } else {
  u8g2.drawStr(80, 54, "AVG:");
  u8g2.drawStr(80, 63, _str_avr_ans);
  }
  u8g2.drawStr(103, 63, "uSv/h");

  switch (SoundEffect) { // Show sound effect of current
    case SOS: {
        u8g2.drawXBMP(18, 56, VOL_WITH_SOS_WIDTH, VOL_WITH_SOS_HEIGHT, VOL_With_SOS);
        break;
      }
    case ON: {
        u8g2.drawXBMP(18, 56, VOL_ON_WIDTH, VOL_ON_HEIGHT, Vol_ON);
        break;
      }
    case OFF: {
        u8g2.drawXBMP(18, 56, VOL_OFF_WIDTH, VOL_OFF_HEIGHT, Vol_OFF);
        break;
      }
  }
  // u8g2.setFont(u8g2_font_logisoso20_tr);
  // u8g2.setCursor(4, 41);
  // print endResult
  //u8g2.print(endResult);

  u8g2.setFont(u8g2_font_logisoso20_tr);
  u8g2.drawStr(1, 41, _str_ans);  // draw answers (The reason that I don't use print function is that it can cause a weird bug, which influent buttons and the UI)
  // if you have any idea how it was happened, please tell me =3
  u8g2.drawXBMP(65, 19, uSvH_WIDTH, uSvH_HEIGHT, uSvH); //draw unit of μSv/h
  u8g2.drawXBMP(0, 55, RAD_TYPE_WIDTH, RAD_TYPE_HEIGHT, Rad_Type); //draw β & γ icon
}

void drawSettings(int _curr) {
  u8g2.setFont(u8g2_font_saikyosansbold8_8u);
  u8g2.drawStr(28, 26, "BRIGHTNESS");
  u8g2.drawStr(28, 40, "SOUND");
  u8g2.drawXBMP(0, 48, DELTA_WIDTH, DELTA_HEIGHT, DELTA_SIGN);
  u8g2.drawStr(14, 58, "REL");
  if (IsRELModeOn){
    u8g2.drawXBMP(37, 51, CHECK_SIGN_WIDTH, CHECK_SIGN_HEIGHT, CHECK_SIGN);
  }
  switch (_curr) {
    case 0: u8g2.drawStr(17, 26, ">"); break;
    case 1: u8g2.drawStr(17, 40, ">"); break;
    case 2: u8g2.drawFrame(0, 47, 48, 16); break;
    //case 2: u8g2.drawFrame(); break;  //delta 
  }
}

void ISR_Timer1() { /* a interrupt event happens every 1s */
  ++tm1_counter;
  if (tm1_counter == 15) {
    tm1_counter = 0;
    CPMArray[currentCPM] = counts * 4;
    usvHr = outputSieverts(CPMArray[currentCPM]);

    counts = 0;
    averageCPM = 0;

    for (int x = 0; x < currentCPM + 1; x++)  { // Take average result from each data in the array
      averageCPM = averageCPM + CPMArray[x];
    }
    
    averageCPM = averageCPM / (currentCPM + 1);
    
    if (currentCPM < 100) { // the array has 100 elements
      currentCPM = currentCPM + 1; // move cursor to the next element
    } else {
      currentCPM = 0;
    }
  }
}

bool hasTrigged = false; // turn to true when the first pulse occured, then display the counting screen

void sensorISR() {
  hasTrigged = true;
  counts++;
  //led_red(true); //turn red led on
  if (SoundEffect == SOS || SoundEffect == ON)
    toneClick();
  //toneSOS();

  if (usvHr < 1.75) {
    led_flash(LED_GREEN);
  } else if (usvHr > 1.75 && usvHr < 2.5) {
    led_flash(LED_YELLOW);
  } else if (usvHr > 2.5) {
    led_flash(LED_RED);
  }
}

#define setting_total_options 3 // How many items in the setting list (started from 1)

extern bool buttonActiveOK;
extern bool longPressActive;

void drawBrightness(uint8_t BRIGHTNESS) {
  u8g2.setFont(u8g2_font_timB08_tf);
  u8g2.drawFrame(9, 41, 100, 11);
  u8g2.drawBox(9, 41, BRIGHTNESS, 11);
  u8g2.drawStr(8, 61, "0");
  u8g2.drawStr(97, 61, "100");
  u8g2.drawXBMP(BRIGHTNESS + 7, 36, dn_arr_WIDTH, dn_arr_HEIGHT, dn_arr); // draw down arror which show the current selection
  u8g2.setCursor(BRIGHTNESS + 7, 33);
  u8g2.print(BRIGHTNESS);
}

void drawSoundSettings(int curr) {
  u8g2.drawXBMP(12, 27, LARGE_SOS_WIDTH, LARGE_SOS_HEIGHT, LARGE_SOS);
  u8g2.drawXBMP(54, 27, LARGE_VOL_ON_WIDTH, LARGE_VOL_ON_HEIGHT, LARGE_VOL_ON);
  u8g2.drawXBMP(92, 27, LARGE_VOL_OFF_WIDTH, LARGE_VOL_OFF_HEIGHT, LARGE_VOL_OFF);
  switch (curr) {
    case 0: u8g2.drawFrame(9, 25, 33, 25); break;
    case 1: u8g2.drawFrame(47, 25, 33, 25); break;
    case 2: u8g2.drawFrame(89, 25, 33, 25); break;
  }
}

void loop(void) {
  wdt_reset();
  switch (menu) {
    case START: { // Start
        u8g2.firstPage();
        do {
          if (hasTrigged == false) {
            u8g2.drawXBMP(1, 0, 128, 64, Start_IMG); // intro
          } else { // If the first pulse has detected.
            menu = COUNT;
          }
        } while ( u8g2.nextPage() );
        break;
      }

    case COUNT: { // Counting Screen
        u8g2.firstPage();
        do {
          buttonReturnDef curr = refresh_button();
          if (curr != NONE) {
            switch ( curr ) {
              case MID_SHORT : {
                  IsRunning = !IsRunning; // freezing function
                  break;
                }
              case MID_LONG : {  // long push on OK button will take user to setting menu
                  menu = SETTINGS;
                  break;
                }
            }
          }
          //noInterrupts();
          drawUICounting(usvHr);
          if (usvHr > 2.5 && SoundEffect == SOS) {
            toneSOS();
          }
          //interrupts();
        } while (u8g2.nextPage());
        break;
      }

    case SETTINGS: { // Settings Selecting
        static int curr_sett = 0; //0 - brightness 1 - sound 2-RELMODE, started from zero
        u8g2.firstPage();
        do {
          drawTitle( "SETTINGS" );
          drawSettings(curr_sett);

          buttonReturnDef curr = refresh_button();
          if (curr != NONE) {
            switch ( curr ) {
              case UPPER : {
                  if (curr_sett > 0) {
                    curr_sett--;
                  }
                  else {
                    curr_sett = setting_total_options - 1;
                  }
                  break;
                }
              case LOWER : {
                  if (curr_sett < setting_total_options - 1) {
                    curr_sett++;
                  }
                  else {
                    curr_sett = 0;
                  }
                  break;
                }
              
              case MID_LONG : {
                  //if (longPressActive == true && buttonActiveOK == false)
                  if (curr_sett == 0) {
                    menu = BRIGHTNESS;
                  }
                  if (curr_sett == 1) {
                    menu = SOUND;
                  }
                  if (curr_sett == 2){
                    IsRELModeOn = !IsRELModeOn;
                    REF_REFERENCE = usvHr;
                    menu = COUNT;
                  }
                  break;
                }
            }
          }

        } while ( u8g2.nextPage() );
        break;
      }

    case BRIGHTNESS: { // Brightness Adjusting
        u8g2.firstPage();
        static uint8_t _brightness = EEPROM.read(addrBRI);
        do {
          drawTitle( "BRIGHTNESS" );
          buttonReturnDef curr = refresh_button();
          if (curr != NONE) {
            switch ( curr ) {
              case UPPER : {
                  if (_brightness < 100) {
                    _brightness++;
                  } else {
                    _brightness = 0;
                  }
                  break;
                }
              case LOWER : {
                  if (_brightness > 0) {
                    _brightness--;
                  } else {
                    _brightness = 100;
                  }
                  break;
                }
              case MID_LONG : {
                  EEPROM.write(addrBRI, _brightness);
                  menu = COUNT;
                  break;
                }
                u8g2.setContrast(map(_brightness, 0, 100, 1, 255));
            }
          }
          drawBrightness(_brightness);
        } while ( u8g2.nextPage() );
        break;
      }

    case SOUND: { // Sound Effect
        u8g2.firstPage();
        do {
          static int curr_sound = EEPROM.read(addrSOUND);
          drawTitle("SOUND");
          buttonReturnDef curr = refresh_button();
          if (curr != NONE) {
            switch ( curr ) {
              case UPPER : {
                  if (curr_sound > 0) {
                    curr_sound--;
                  }
                  else {
                    curr_sound = 2;
                  }
                  break;
                }
              case LOWER : {
                  if (curr_sound < 2) {
                    curr_sound++;
                  }
                  else {
                    curr_sound = 0;
                  }
                  break;
                }
              case MID_LONG : {
                  EEPROM.write(addrSOUND, curr_sound);
                  SoundEffect = soundfx(curr_sound);
                  menu = COUNT;
                  break;
                }
            }
          }
          drawSoundSettings(curr_sound);
        } while ( u8g2.nextPage() );
      }
  }
}

float outputSieverts(float x)  {
  float y = x * 0.0057;
  return y;
}
