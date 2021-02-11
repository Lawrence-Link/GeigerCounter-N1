/*
    All rights reserved
    ©LPD Lawrence Link 2021
    Geiger Counter N1
*/

#define DEBUG
#include "Batt.h"
#include "buzz.h"
#include "leds.h"
#include "Buttons.h"
#include "resources.h"
#include <Wire.h>
#include <U8g2lib.h>
#include <TimerOne.h>

bool IsRunning = true;
enum {START, COUNT, SETTINGS, BRIGHTNESS, SOUND} menu = START;
enum {SOS, ON, OFF} SoundEffect = SOS;
enum buttonReturnDef;

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup(void) {
  u8g2.begin();
  pinMode(2, INPUT_PULLUP);
  led_initialize();
  buttons_init();
  attachInterrupt(INT0, sensorISR, FALLING); //Detect pulse at the falling edge
  Timer1.initialize(100000); //~0.1s
  Timer1.attachInterrupt(ISR_Timer1);
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
  u8g2.setFont(u8g2_font_sirclivethebold_tr);
  u8g2.drawStr(29, 9, title);
}

void drawUICounting(float endResult = NULL) { // When Counter works..
  u8g2.setFont(u8g2_font_luBIS08_tf);
  u8g2.drawLine(0, 11, 127, 11); // Upper dividing line
  u8g2.drawLine(0, 53, 127, 53); // Lower dividing line
  u8g2.drawStr(0, 9, "LPD");
  u8g2.drawLine(26, 0, 26, 11);

  // Batt-icon determine
  if (GetBatteryVolt() < 3.6) {
    u8g2.drawXBMP(29, 2, BATT_ERR_WIDTH, BATT_ERR_HEIGHT, BATT_ERR);
  } else if (GetBatteryVolt() < 3.8) {
    u8g2.drawXBMP(29, 2, BATT_25_WIDTH, BATT_25_HEIGHT, BATT_25);
  } else if (GetBatteryVolt() < 3.9) {
    u8g2.drawXBMP(29, 2, BATT_75_WIDTH, BATT_75_HEIGHT, BATT_75);
  } else if (GetBatteryVolt() >= 3.9) {
    u8g2.drawXBMP(29, 2, BATT_100_WIDTH, BATT_100_HEIGHT, BATT_100);
  }

  if (IsRunning == true) { // Showing running state
    u8g2.setFont(u8g2_font_timB08_tf);
    u8g2.drawStr(75, 9, "RUNNING");
  } else {
    u8g2.setFont(u8g2_font_timB08_tf);
    u8g2.drawStr(75, 9, "STOPPED");
  }

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
  u8g2.setFont(u8g2_font_logisoso20_tr);
  u8g2.setCursor(4, 41);
  if (endResult == NULL) { // print endResult
    u8g2.print("? --");
  } else {
    u8g2.print(endResult);
  }

  u8g2.drawXBMP(0, 55, RAD_TYPE_WIDTH, RAD_TYPE_HEIGHT, Rad_Type); //draw β & γ icon
  u8g2.drawXBMP(62, 21, uSvH_WIDTH, uSvH_HEIGHT, uSvH); //draw unit of μSv/h
}

void drawSettings(int _curr) {
  u8g2.setFont(u8g2_font_t0_13b_mf);
  u8g2.drawStr(28, 26, "Brightness");
  u8g2.drawStr(28, 40, "Sound");
  switch (_curr) {
    case 0: u8g2.drawStr(17, 26, ">"); break;
    case 1: u8g2.drawStr(17, 40, ">"); break;
  }
}

void ISR_Timer1() {

}

bool hasTrigged = false;
// volatile bool SOS = false;

void sensorISR() {
  hasTrigged = true;
  //led_red(true); //turn red led on
  toneClick();
}

#define total_options 2 // How many items in the setting list (started from 1)

extern bool buttonActiveOK;
extern bool longPressActive;

void drawBrightness(uint8_t BRIGHTNESS) {
  u8g2.setFont(u8g2_font_profont10_tr);
  u8g2.drawFrame(9, 45, 110, 11);
  u8g2.drawBox(9, 45, BRIGHTNESS, 11);
  u8g2.drawStr(8, 38, "0");
  u8g2.drawStr(107, 38, "100");
}

void loop(void) {
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
          drawUICounting();
        } while (u8g2.nextPage());
        break;
      }

    case SETTINGS: { // Settings Selecting
        static int curr_sett = 0; //0 - brightness 1 - sound, started from zero
        u8g2.firstPage();
        do {
          drawTitle("Settings");
          drawSettings(curr_sett);

          buttonReturnDef curr = refresh_button();
          if (curr != NONE) {
            switch ( curr ) {
              case UPPER : {
                  if (curr_sett > 0) {
                    curr_sett--;
                  }
                  else {
                    curr_sett = total_options - 1;
                  }
                  break;
                }
              case LOWER : {
                  if (curr_sett < total_options - 1) {
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
                  break;
                }
            }
          }

        } while ( u8g2.nextPage() );
        break;
      }

    case BRIGHTNESS: { // Brightness Adjusting
        u8g2.firstPage();
        static uint8_t _brightness = 0;
        do {
          drawTitle( "BRIGHTNESS" );

          buttonReturnDef curr = refresh_button();
          if (curr != NONE) {
            switch ( curr ) {
              case UPPER : {
                  if (_brightness < 100) {
                    _brightness++;
                  }
                  break;
                }
              case LOWER : {
                  if (_brightness > 0) {
                    _brightness--;
                  }
                  break;
                }
              case MID_LONG : {

                  break;
                }
            }
          }

          drawBrightness(_brightness);
        } while ( u8g2.nextPage() );
        break;
      }

    case SOUND: { // Sound Effect
        u8g2.firstPage();
      }
  }
}
