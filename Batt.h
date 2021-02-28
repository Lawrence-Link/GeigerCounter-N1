/*
    ©LPD Lawrence Link 2021
    All rights reserved
    Geiger Counter N1
    Licensed under GNU General Public License v3.0
    -------CONTACT AT-------
    Lawrence-Link@outlook.com
*/

#ifndef _BATT_H_
#define _BATT_H_

float GetBatteryVolt();
void batt_init();
bool isCharging();
#ifdef BOARD_VER 2
void enableDivider();
void disableDivider();
#endif
#endif
