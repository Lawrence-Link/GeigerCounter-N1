#ifndef _BUTTONS_H_
#define _BUTTONS_H_

//extern enum buttonReturnDef;
void buttons_init();
enum buttonReturnDef {NONE, UPPER, MID_SHORT, MID_LONG, LOWER}; 

buttonReturnDef refresh_button();

#endif
