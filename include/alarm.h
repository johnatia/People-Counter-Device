#ifndef _ALARM_H_
#define _ALARM_H_
#include <Arduino.h>

#define ALARM_PIN   14

#define Alarm_Init()        pinMode(ALARM_PIN , OUTPUT);\ 
                            digitalWrite(ALARM_PIN , LOW)

#define Alarm_ON()              digitalWrite(ALARM_PIN , !digitalRead(ALARM_PIN));  \
                                vTaskDelay(1000)

#define Alarm_OFF()           digitalWrite(ALARM_PIN , LOW)


// void Alarm_Init(void);
// void Alarm_ON(void);
// void Alarm_OFF(void);


#endif