#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <Arduino.h>
#include "SerialToParallel.h"

#define DISP_STP_CLK                            12
#define DISP_STP_LATCH                          13
#define DISP_STP_DATA                           27

#define Display_Init()                          STP_Init()
#define Display_OFF()                           Serial.println("Display is OFF");\
                                                STP_Shift(0xffff)
void Display_Show(uint8_t NumToDisplay);


//void Display_Init(void);
//void Display_OFF(void);


#endif