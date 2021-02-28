/*
    ©LPD Lawrence Link 2021
    All rights reserved
    Geiger Counter N1
    Licensed under GNU General Public License v3.0
    -------CONTACT AT-------
    Lawrence-Link@outlook.com
*/

#define BOARD_VER 1

/* Must have the interrupt function */
#define PIN_GEIGER 2

#define PIN_BATT_LVL        A7
#define PIN_CHG_DETECT      4
#define PIN_DIVIDER_ENABLE  3

/* Common-Cathode RGB LED */
#define PIN_LED_RED      A0
#define PIN_LED_GREEN    A1
#define PIN_LED_BLUE     A2

#define PIN_BUZZER 7

/* ALL PULLED UP */
//enum {PIN_BUTTON_UP = 10, PIN_BUTTON_OK = 9, PIN_BUTTON_DN = 8};
#define PIN_BUTTON_UP 10
#define PIN_BUTTON_OK 9
#define PIN_BUTTON_DN 8
