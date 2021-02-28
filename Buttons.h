/*
    ©LPD Lawrence Link 2021
    All rights reserved
    Geiger Counter N1
    Licensed under GNU General Public License v3.0
    -------CONTACT AT-------
    Lawrence-Link@outlook.com
*/

#ifndef _BUTTONS_H_
#define _BUTTONS_H_

//extern enum buttonReturnDef;
void buttons_init();
enum buttonReturnDef {NONE, UPPER, MID_SHORT, MID_LONG, LOWER}; 

buttonReturnDef refresh_button();

#endif
