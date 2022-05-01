
#ifndef _PEOPLE_COUNT_H_
#define _PEOPLE_COUNT_H_

#include <Arduino.h>
#include "wifi_app.h"
#include <EEPROM.h>
#include "config.h"
#include "display.h"
#include "main.h"


#define DISTANCES_ARRAY_SIZE                         5    // nb of samples
#define MAX_DISTANCE                                 2400 // mm
#define MIN_DISTANCE                                 0   // mm
//#define DIST_THRESHOLD                               1600  // mm
#define ROWS_OF_SPADS                                8 // 8x16 SPADs ROI
// People Counting defines
#define NOBODY                    0
#define SOMEONE                   1
// #define LEFT                      0
// #define RIGHT                     1
#define RIGHT                     0
#define LEFT                      1
#define RIGHT_ZONE_CENTER                       167 //31//231  // 175
//35 //175 // was 167, see UM2555 on st.com, centre = 175 has better return signal rate for the ROI #1
#define LEFT_ZONE_CENTER                         231 //95 //151  //175  //231 


#define OUTDOOR 1
#define INDOOR  2
#define DEVICE_LOCATION OUTDOOR
//Optional interrupt and shutdown pins.
#define SHUTDOWN_PIN 2
#define INTERRUPT_PIN 3
#define BuiltInLED 2

#define NOT_YET 0
#define DONE  1

#define PEOPLECOUNT_EDIT_FROM_ISR            5
#define PEOPLECOUNT_EDIT_FROM_SENSOR         10
extern BluetoothSerial SerialBT;
extern int8_t PeopleCount;


extern const uint8_t PeopleCount_queue_len , WebFlag_queue_len;
extern QueueHandle_t PeopleCount_queue , WebFlag_queue;

extern TaskHandle_t DisplayHandle;


uint16_t PeopleCountAlgorithm(uint16_t distance, uint8_t zone, uint16_t DistanceThreshold);

#endif